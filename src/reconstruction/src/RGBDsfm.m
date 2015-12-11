% RGBDsfm: full reconstruction pipeline
%
% Inputs
% - data_dir: path to data
% - BAmode: bundle adjustment mode TODO: document
% - frameIDs: array of indices indicating frames to include; [] for all
% - writeExtrinsics: TODO: document
%
% Outputs
% - cameraRtC2W: TODO: document
% - pointCloud: TODO: document

% TODO: change image to color

function [cameraRtC2W, pointCloud] = RGBDsfm(data_dir, BAmode, frameIDs, writeExtrinsics)

%% Name and create output directory
out_dir = fullfile(data_dir, 'sfm');

if isempty(frameIDs)
     out_dir = [out_dir '_BA' num2str(BAmode) '_all'];
else
     out_dir = [out_dir '_BA' num2str(BAmode) '_' num2str(frameIDs(1)) '_' num2str(frameIDs(end))];
end

mkdir(out_dir);

%% Add source paths and set up toolboxes
basicSetup

%% READ DATA
% Get full file path
data = loadStructureIOdata(data_dir, frameIDs);

% DEBUG: uncomment to show video of frames read in
%{
figure
for cameraID=1:length(data.image)
    subplot(1,2,1)
    imshow(imread(data.depth{cameraID}));
    subplot(1,2,2)
    imshow(imread(data.image{cameraID}));
    title(sprintf('Frame %d',cameraID));
    drawnow;
   % pause;
end
%}

%% TIME-BASED RECONSTRUCTION
[MatchPairs, cameraRtC2W] = alignTimeBased(data);

save(fullfile(out_dir, 'cameraRt_RANSAC.mat'),'cameraRtC2W','MatchPairs','-v7.3');
fprintf('ransac all finished\n');
outputPly(fullfile(out_dir, 'time.ply'), cameraRtC2W, data);

% DEBUG: uncomment to plot the pose graph
%{
cameraCenters = reshape(cameraRtC2W(:,4,:),3,[]);
figure(100)
%plot3(cameraCenters(1,:),cameraCenters(2,:),cameraCenters(3,:),'-');
axis equal;
hold on;
grid on;
plot3(cameraCenters(1,:),cameraCenters(2,:),cameraCenters(3,:),'.r', 'markersize', 0.1);
%}

%% BUILD BAG OF WORDS DICTIONARY
BOWmodel = visualindex_build(data.image, 1:length(data.image), false, 'numWords', 4000);

%% FIND LOOP CLOSURE CANDIDATES

% Build an NxN similarity matrix where N is number of images (dot product
% BOW histogram between any two images to get relevant entry)
scores = BOWmodel.index.histograms' * BOWmodel.index.histograms;

% Release memory
clear BOWmodel;

% Create NxN lower triangular matrix with 0s in the diagonal, where the
% entries are Euclidean distance from the diagonal of the matrix.
% Forces greater weights on images that are far apart in index (assumes
% that images close in index are close in time and space; images far in
% index are at least far in time; better chance to be loop closure).
wDistance = tril(min(1, bwdist(eye(length(data.image))) / 150), -1);

% Weight the similarity matrix by distance and apply gaussian filter
scores = double(wDistance) .* full(scores);
G = fspecial('gaussian', [5 5], 2);
scores = imfilter(scores, G, 'same');
clear G;

% Use non maximum suppression to narrow down field of loop closure
% candidates (see nmsMatrix for more documentation)
scoresNMS = nmsMatrix(scores, 200);

% Get indices of scores over our threshold (0.2)
ind = find(scoresNMS(:) > 0.2);

% Sort scores above threshold and get indices
[~, perm] = sort(scoresNMS(ind), 'descend');
ind = ind(perm);

% for time problem, make it no longer than the length(data.image)
% if length(ind)>length(data.image)
%     ind = ind(1:length(data.image));
% end

% Get camera pose pairs that are potentially loop closure pairs
[cameras_i, cameras_j] = ind2sub(size(scoresNMS),ind);

% DEBUG: uncomment to show loop closure candidates side by side + scores
%{
figure
for cameraID=1:length(cameras_i)
    im_i = imread(data.image{cameras_i(cameraID)});
    im_j = imread(data.image{cameras_j(cameraID)});
    
    im_ij(:,:,1)=[im_i(:,:,1) im_j(:,:,1)];
    im_ij(:,:,2)=[im_i(:,:,2) im_j(:,:,2)];
    im_ij(:,:,3)=[im_i(:,:,3) im_j(:,:,3)];
    
    clf
    subplot(3,1,1);
    imshow(im_ij);
    title(sprintf('Frame %d and %d: score = %f', cameras_i(cameraID), cameras_j(cameraID), full(scores(cameras_i(cameraID),cameras_j(cameraID)))));
    
    subplot(3,1,[2 3]);
    imagesc(scores);
    axis equal;
    hold on;
    siW = 50;
    plot([cameras_j(cameraID)-siW cameras_j(cameraID)+siW cameras_j(cameraID)+siW cameras_j(cameraID)-siW cameras_j(cameraID)-siW], ...    
    [cameras_i(cameraID)-siW cameras_i(cameraID)-siW cameras_i(cameraID)+siW cameras_i(cameraID)+siW cameras_i(cameraID)-siW], '-r');
    axis tight;
    axis off;
    drawnow;
    
    pause;
end
%}

%% TEST LOOP CLOSURE CANDIDATES

MatchPairsLoop = cell(1,length(cameras_i));

% Check all loop closure candidates and make sure we are greater than the
% minimum acceptable number of SIFT matches
for cameraID = 1:length(cameras_i)
    MatchPairsLoop{cameraID} = align2view(data, cameras_i(cameraID), cameras_j(cameraID));
end

minAcceptableSIFT = 20;
 
cntLoopEdge = 0;
for pairID=1:length(MatchPairsLoop)
    if size(MatchPairsLoop{pairID}.matches, 2) > minAcceptableSIFT
        cntLoopEdge = cntLoopEdge + 1;
    end
end
fprintf('found %d good loop edges\n', cntLoopEdge); clear cntLoopEdge;

clear cameras_i
clear cameras_j


%% PREPARE BUNDLE ADJUSTMENT DATA STRUCTURES

% Store relative camera poses as well
cameraRt_ij = repmat([eye(3) zeros(3, 1)], [1, 1, length(data.image) - 1]);

% Keep track of the camera indices in each pair. Note column-major order
cameraRt_ij_indices = repmat(zeros(2, 1), [1, length(data.image) - 1]);

% Keep track of SIFT key points that are common to each pair. We want
% points in i's and j's coordinate system (R_ij * p is C2C from j to i). We don't
% preallocate memory. Oh well.
cameraRt_ij_points_observed_i = zeros(3, 1);
cameraRt_ij_points_observed_j = zeros(3, 1);

% Keep track of the observations (i.e., points, but transformed to world
% coordinates) according to i's Rt
cameraRt_ij_points_predicted = zeros(3, 1);

% Keep track of the number of SIFT key points that are common to each pair
% (so we can look up later)
cameraRt_ij_points_count = [];

% Keep track of total number of points we've added
cameraRt_ij_points_total = 0;

% Grab all key point locations
for frameID = 1:length(data.image) - 1
    cameraRt_ij(:, :, frameID) = MatchPairs{frameID}.Rt;
    cameraRt_ij_indices(:, frameID) = [frameID, frameID + 1];
    
    matches = MatchPairs{frameID}.matches(3:5, :);
    cameraRt_ij_points_count(frameID) = size(matches, 2);
    cameraRt_ij_points_observed_i(:, (cameraRt_ij_points_total + 1):(cameraRt_ij_points_total + size(matches, 2))) = matches;
    cameraRt_ij_points_observed_j(:, (cameraRt_ij_points_total + 1):(cameraRt_ij_points_total + size(matches, 2))) = MatchPairs{frameID}.matches(8:10, :);
    cameraRt_ij_points_predicted(:, (cameraRt_ij_points_total + 1):(cameraRt_ij_points_total + size(matches, 2))) = transformRT(matches, cameraRtC2W(:,:,frameID), false);
    cameraRt_ij_points_total = cameraRt_ij_points_total + size(matches, 2);
end

%% STORE SIFT MATCHES FROM LOOP-CLOSURES
for pairID = 1:length(MatchPairsLoop)
    if size(MatchPairsLoop{pairID}.matches,2)>minAcceptableSIFT
        cameraRt_ij(:, :, size(cameraRt_ij, 3) + 1) = MatchPairsLoop{pairID}.Rt;
        cameraRt_ij_indices(:, size(cameraRt_ij_indices, 2) + 1) = [MatchPairsLoop{pairID}.i, MatchPairsLoop{pairID}.j];
        
        matches = MatchPairsLoop{pairID}.matches(3:5, :);
        cameraRt_ij_points_count(size(cameraRt_ij_points_count, 2) + 1) = size(matches, 2);
        cameraRt_ij_points_observed_i(:, (cameraRt_ij_points_total + 1):(cameraRt_ij_points_total + size(matches, 2))) = matches;
        cameraRt_ij_points_observed_j(:, (cameraRt_ij_points_total + 1):(cameraRt_ij_points_total + size(matches, 2))) = MatchPairsLoop{pairID}.matches(8:10, :);;
        cameraRt_ij_points_predicted(:, (cameraRt_ij_points_total + 1):(cameraRt_ij_points_total + size(matches, 2))) = transformRT(matches, cameraRtC2W(:,:,MatchPairsLoop{pairID}.i), false);
        cameraRt_ij_points_total = cameraRt_ij_points_total + size(matches, 2);
    end
end

save(fullfile(out_dir,'MatchPairs.mat'),'MatchPairs','MatchPairsLoop','scores','scoresNMS','-v7.3');
clear MatchPairsLoop
clear MatchPairs
clear scores
clear scoresNMS

% DEBUG: uncomment to visualize loop closures
%{
imagesc(full(pointObserved(:,1000:4000)))
pointIDall = find(pointObservedValue(6,:)==-1)
for idd =1:300:length(pointIDall)
    close all
    [~,pointID] = find(pointObserved ==pointIDall(idd));
    cameraID = find(pointObserved(:,pointID))
    for i =1:length(cameraID)
        figure
        imshow(data.image{cameraID(i)});
        obs = pointObservedValue(:,pointObserved(cameraID(i),pointID));
        hold on;
        plot(obs(1),obs(2),'xr','MarkerSize',12);
        %reprojection = visulizereprojection(obs(3:4),data,cameraRtC2W(:,:,cameraID));
        %plot(reprojection(1),reprojection(2),'xr');
        pause;
    end
end
%}

%% RUN BUNDLE ADJUSTMENT

fprintf('bundle adjusting    ...\n');
tic

weight = 100;
mode = BAmode;

% Number of poses
nCam = uint32(size(cameraRtC2W,3));

% Number of frame pairs
nPairs = uint32(size(cameraRt_ij, 3));

% Number of matched points for the pairs
nMatchedPoints = cameraRt_ij_points_total;

% Get temp file name for input and output to bundle adjustment stage
fname_inout = tempname;
fname_in = [fname_inout '.in'];
fname_out = [fname_inout '.out'];

fin = fopen(fname_in, 'wb');
fwrite(fin, nCam, 'uint32');
fwrite(fin, nPairs, 'uint32');
fwrite(fin, nMatchedPoints, 'uint32');

fwrite(fin, cameraRtC2W, 'double');
fwrite(fin, cameraRt_ij, 'double');
fwrite(fin, cameraRt_ij_indices, 'uint32');
fwrite(fin, cameraRt_ij_points_observed_i, 'double');
fwrite(fin, cameraRt_ij_points_observed_j, 'double');
fwrite(fin, cameraRt_ij_points_predicted, 'double');
fwrite(fin, cameraRt_ij_points_count, 'uint32');

fclose(fin);

save(fullfile(out_dir, 'data.mat'));

cmd = sprintf('./ba2D3D %d %f %s %s', mode, weight, fname_in, fname_out);
fprintf('%s\n',cmd);
system(cmd);
system(sprintf('cp %s ~/Downloads/tmp/results.in', fname_in));
system(sprintf('cp %s ~/Downloads/tmp/results.out', fname_out));

% read the result back;
fout = fopen(fname_out, 'rb');
cameraRtC2W_PoseGraph = fread(fout,12*nCam,'double');

fclose(fout);

cameraRtC2W_PoseGraph = reshape(cameraRtC2W_PoseGraph,3,4,[]);

delete(fname_in);
delete(fname_out);

toc;

save(fullfile(out_dir, 'results.mat'));
outputPly(fullfile(out_dir, 'PG.ply'), cameraRtC2W_PoseGraph, data);

% DEBUG: uncomment to plot the pose graph
%{
cameraCenters = reshape(cameraRtC2W(:,4,:),3,[]);
figure(100)
%plot3(cameraCenters(1,:),cameraCenters(2,:),cameraCenters(3,:),'-');
axis equal;
hold on;
grid on;
plot3(cameraCenters(1,:),cameraCenters(2,:),cameraCenters(3,:),'.r', 'markersize', 0.1);
%}

% fprintf('rectifying scenes ...');
% tic
% cameraRtC2W = rectifyScene(cameraRtC2W, data);
% toc

%% SAVE OUT DATA

% clean all unnecessary variables
clear wDistance
clear frame_begin
clear frame_end
clear frame_interval
clear ind
clear jsonfiles
clear maxNumPoints
clear minAcceptableSIFT
clear n
clear pairID
clear perm

save(fullfile(out_dir,'BA_variables.mat'),'-v7.3');

%% output camera text file

%% output thumbnail
% outputThumbnail(data_dir, frame_dir, cameraRtC2W, timeStamp, data);

end

