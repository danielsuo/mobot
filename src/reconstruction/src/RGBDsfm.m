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

% TODO
doloopclosure = 1;
Longtrackloopclosure = 0;

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
MatchPairs = cell(1,length(data.image)-1);

% Loop through all consecutive frames and find relative poses
parfor frameID = 1:length(data.image)-1
    MatchPairs{frameID} = align2view(data, frameID, frameID+1);
end

% Naive approach: just put all results together. Accumulate Rt across
% frames
cameraRtC2W = repmat([eye(3) zeros(3, 1)], [1, 1, length(data.image)]);

% Store relative camera poses as well
cameraRt_ij = repmat([eye(3) zeros(3, 1)], [1, 1, length(data.image) - 1]);

% Keep track of the camera indices in each pair. Note column-major order
cameraRt_ij_indices = repmat(zeros(2, 1), [1, length(data.image) - 1]);

for frameID = 1:length(data.image)-1
    cameraRtC2W(:, :, frameID+1) = [cameraRtC2W(:, 1:3, frameID) * MatchPairs{frameID}.Rt(:, 1:3) ...
                                    cameraRtC2W(:,1:3,frameID) * MatchPairs{frameID}.Rt(:, 4) + cameraRtC2W(:, 4, frameID)];
    cameraRt_ij(:, :, frameID) = MatchPairs{frameID}.Rt;
    cameraRt_ij_indices(:, frameID) = [frameID, frameID + 1];
end

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
        cameraRt_ij(:, :, size(cameraRt_ij, 3) + 1) = MatchPairsLoop{pairID}.Rt;
        cameraRt_ij_indices(:, size(cameraRt_ij_indices, 2) + 1) = [MatchPairsLoop{pairID}.i, MatchPairsLoop{pairID}.j];
    end
end
fprintf('found %d good loop edges\n', cntLoopEdge); clear cntLoopEdge;

%{
figure
for pairID=ind%1:length(MatchPairsLoop)
    %if MatchPairsLoop{pairID}.i<100||MatchPairsLoop{pairID}.j<100

        %im_i = readImage(data,MatchPairsLoop{pairID}.i);
        %im_j = readImage(data,MatchPairsLoop{pairID}.j);
        im_i = imread(data.image{MatchPairsLoop{pairID}.i});
        im_j = imread(data.image{MatchPairsLoop{pairID}.j});
        depth_i = imread(data.depth{MatchPairsLoop{pairID}.i});
        depth_j = imread(data.depth{MatchPairsLoop{pairID}.j}); 

        im_ij(:,:,1)=[im_i(:,:,1) im_j(:,:,1)];
        im_ij(:,:,2)=[im_i(:,:,2) im_j(:,:,2)];
        im_ij(:,:,3)=[im_i(:,:,3) im_j(:,:,3)];
        
        
        clf
        subplot(2,1,1);
        imshow(im_ij);
        subplot(2,1,2);
        imagesc([depth_i depth_j]);
        axis equal;
        %axis off;

        title(sprintf('Frame %d and %d: matches = %d', MatchPairsLoop{pairID}.i, MatchPairsLoop{pairID}.j, size(MatchPairsLoop{pairID}.matches,2)));
        hold on;

        plot([MatchPairsLoop{pairID}.matches(1,:)' 640+MatchPairsLoop{pairID}.matches(6,:)']',[MatchPairsLoop{pairID}.matches(2,:)' MatchPairsLoop{pairID}.matches(7,:)']','-');
%         try
%                 subplot(3,1,[2 3]);
%                 imagesc(scoresNMS);
%                 axis equal;
%                 hold on;
%                 siW = 50;
%                 plot([MatchPairsLoop{pairID}.j-siW MatchPairsLoop{pairID}.j+siW MatchPairsLoop{pairID}.j+siW MatchPairsLoop{pairID}.j-siW MatchPairsLoop{pairID}.j-siW], ...    
%                 [MatchPairsLoop{pairID}.i-siW MatchPairsLoop{pairID}.i-siW MatchPairsLoop{pairID}.i+siW MatchPairsLoop{pairID}.i+siW MatchPairsLoop{pairID}.i-siW], '-r');
%                 axis tight;
%                 axis off;
%         end
        drawnow;

        pause;
    end
end
%}

%{
figure(100)
cameraCenters = reshape(cameraRtC2W(:,4,:),3,[]);
plot3(cameraCenters(1,:),cameraCenters(2,:),cameraCenters(3,:),'.r', 'markersize', 0.1);

for cameraID=1:10:length(MatchPairsLoop)
    if size(MatchPairsLoop{cameraID}.matches,2)>minAcceptableSIFT
        hold on
        plot3(cameraCenters(1,[MatchPairsLoop{cameraID}.i MatchPairsLoop{cameraID}.j]),cameraCenters(2,[MatchPairsLoop{cameraID}.i MatchPairsLoop{cameraID}.j]),...
            cameraCenters(3,[MatchPairsLoop{cameraID}.i MatchPairsLoop{cameraID}.j]),'-k');
    end
end
hold on
plot3(cameraCenters(1,1:1000),cameraCenters(2,1:1000),cameraCenters(3,1:1000)cd(fileparts(which('main')));,'.b', 'markersize', 0.1);
xlabel('x')
ylabel('y')
zlabel('z')

%}

clear cameras_i
clear cameras_j


%% PREPARE BUNDLE ADJUSTMENT DATA STRUCTURES

% Weight to give time points
wTimePoints = 0.1;

% Max out at 1000 points per image in the point cloud
maxNumPoints = length(data.image) * 1000;

pointObserved = sparse(length(data.image), maxNumPoints);

% Hold all SIFT results 6 x length(data.image) * 1000
pointObservedValue = zeros(6,maxNumPoints);

% Initialize point cloud data structure
pointCloud = zeros(3,maxNumPoints);
pointCount = 0;
pointObservedValueCount = 0;

%% STORE SIFT MATCHES FROM TIME-BASED RECONSTRUCTION

% Get number of SIFT matches in the first time-based pair
pointCount = size(MatchPairs{1}.matches, 2);

% Get number of matches in both images (i.e., 2 x pointCount)
pointObservedValueCount = pointCount * 2;

% Store image x, y and point cloud x, y, z and a -wTimePoints weight
% for 6 x pointObservedValueCount matrix
pointObservedValue(:,1:pointObservedValueCount) = [[MatchPairs{1}.matches(1:5,:) MatchPairs{1}.matches(6:10,:)]; -wTimePoints * ones(1,pointObservedValueCount)];

% Keep track of SIFT indices we've seen
pointObserved(1, 1:pointCount) = 1:pointCount;
pointObserved(2, 1:pointCount) = pointCount + (1:pointCount);

% Keep column of indices for the SIFT keypoints we have in the current
% frame
previousIndex = 1:pointCount;

% Get point cloud of first frame (x, y, z) matches
pointCloud(:,1:pointCount) = MatchPairs{1}.matches(3:5,:);

for frameID = 2:length(data.image)-1

    % Find intersection of SIFT key points that matched previous frame
    % and next frame; want to get rid of duplicates
    [~, iA, iB] = intersect(MatchPairs{frameID - 1}.matches(6:7,:)', MatchPairs{frameID}.matches(1:2,:)', 'rows');

    % Get number of new SIFT key points (i.e., points that match the
    % next frame
    alreadyExist = false(1, size(MatchPairs{frameID}.matches, 2));
    alreadyExist(iB) = true;
    newCount = sum(~alreadyExist);

    % Keep the previous indices for SIFT key points we already had and
    % interleave new indices for new SIFT key point we match with the
    % next frame
    currentIndex = zeros(1, size(MatchPairs{frameID}.matches, 2));
    currentIndex(iB) = previousIndex(iA);
    currentIndex(~alreadyExist) = (pointCount + 1):(pointCount + newCount);

    % Add new SIFT key points from current frame and all SIFT key
    % points fCan't go much bigger without extra effortrom next frame
    pointObservedValue(1:5, pointObservedValueCount + 1:pointObservedValueCount + newCount + length(currentIndex)) = ...
        [MatchPairs{frameID}.matches(1:5,~alreadyExist) MatchPairs{frameID}.matches(6:10,:)];

    % Add weight row for all the new key points (-wTimePoints)
    pointObservedValue(6, pointObservedValueCount + 1:pointObservedValueCount+newCount + length(currentIndex)) = -wTimePoints;

    % Add indices for new key points we've seen for both current and
    % next frame
    pointObserved(frameID, currentIndex(~alreadyExist)) = (pointObservedValueCount + 1):(pointObservedValueCount + newCount);
    pointObservedValueCount = pointObservedValueCount + newCount;
    pointObserved(frameID+1,currentIndex) = (pointObservedValueCount+1):(pointObservedValueCount+length(currentIndex));
    pointObservedValueCount = pointObservedValueCount + length(currentIndex);

    % Update point cloud with matches we've seen
    pointCloud(:,pointCount+1:pointCount+newCount) = transformRT(MatchPairs{frameID}.matches(3:5,~alreadyExist), cameraRtC2W(:,:,frameID), false);

    % Update number of key points
    pointCount = pointCount + newCount;

    % Update index
    previousIndex = currentIndex;
end

%% STORE SIFT MATCHES FROM LOOP-CLOSURES
if doloopclosure
    for pairID = 1:length(MatchPairsLoop)
        if size(MatchPairsLoop{pairID}.matches,2)>minAcceptableSIFT
            n = size(MatchPairsLoop{pairID}.matches,2);

            pointObservedValue(:,pointObservedValueCount+1:pointObservedValueCount+n*2) = [[MatchPairsLoop{pairID}.matches(1:5,:) MatchPairsLoop{pairID}.matches(6:10,:)]; -1 * ones(1,2*size(MatchPairsLoop{pairID}.matches,2))];

            pointObserved(MatchPairsLoop{pairID}.i,pointCount+1:pointCount+n)=pointObservedValueCount+1  :pointObservedValueCount+n;
            pointObserved(MatchPairsLoop{pairID}.j,pointCount+1:pointCount+n)=pointObservedValueCount+1+n:pointObservedValueCount+n*2;
            pointCloud(:,pointCount+1:pointCount+n) = transformRT(MatchPairsLoop{pairID}.matches(3:5,:), cameraRtC2W(:,:,MatchPairsLoop{pairID}.i), false);

            pointObservedValueCount = pointObservedValueCount+n*2;
            pointCount = pointCount+n;
        end
    end
end


save(fullfile(out_dir,'MatchPairs.mat'),'MatchPairs','MatchPairsLoop','scores','scoresNMS','-v7.3');
clear MatchPairsLoop
clear MatchPairs
clear scores
clear scoresNMS

% save some memory
pointCloud = pointCloud(:,1:pointCount);
pointObserved = pointObserved(:,1:pointCount);
pointObservedValue = pointObservedValue(:,1:pointObservedValueCount);

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
outputKeypointsPly(fullfile(out_dir, 'time_key.ply'),pointCloud(:,reshape(find(sum(pointObserved~=0,1)>0),1,[])));

fprintf('bundle adjusting    ...\n');
tic

K = data.K;
weight = 100;
mode = BAmode;

% Get camera and feature indices along with the value (itself and ID in
% pointObservedValue)
[camID,ptsID,valID] = find(pointObserved);

% Number of poses
nCam = uint32(size(cameraRtC2W,3));

% Number of points
nPts = uint32(size(pointCloud,2));

% Number of SIFT feature points
nObs = uint32(length(valID));

% Number of frame pairs
nPairs = uint32(size(cameraRt_ij, 3));

% Camera intrinsics
fx = K(1,1); 
fy = K(2,2); 
px = K(1,3); 
py = K(2,3);

% Transform cameraRt from camera to world to world to camera
cameraRtW2C = zeros(3,4,size(cameraRtC2W,3));
for cameraID=1:size(cameraRtC2W,3)    
    cameraRtW2C(:,:,cameraID) = transformCameraRt(cameraRtC2W(:,:,cameraID));
end

% Get temp file name for input and output to bundle adjustment stage
fname_inout = tempname;
fname_in = [fname_inout '.in'];
fname_out = [fname_inout '.out'];

fin = fopen(fname_in, 'wb');
fwrite(fin, nCam, 'uint32');
fwrite(fin, nPts, 'uint32');
fwrite(fin, nObs, 'uint32');
fwrite(fin, nPairs, 'uint32');
fwrite(fin, fx, 'double');
fwrite(fin, fy, 'double');
fwrite(fin, px, 'double');
fwrite(fin, py, 'double');

fwrite(fin, cameraRtC2W, 'double');
fwrite(fin, cameraRtW2C, 'double');
fwrite(fin, pointCloud, 'double');
fwrite(fin, cameraRt_ij, 'double');
fwrite(fin, cameraRt_ij_indices, 'uint32');

% write observation  
ptsObservedIndex = uint32([camID,ptsID]-1)';
ptsObservedValue = pointObservedValue(:,valID);
fwrite(fin, ptsObservedIndex, 'uint32');
fwrite(fin, ptsObservedValue, 'double');

fclose(fin);

system(sprintf('cp %s ~/Downloads%s', fname_in, fname_in));
cmd = sprintf('./ba2D3D %d %f %s %s', mode, weight, fname_in, fname_out);
fprintf('%s\n',cmd);
system(cmd);

% read the result back;
fout = fopen(fname_out, 'rb');
nCam=fread(fout,1,'uint32');
nPts=fread(fout,1,'uint32');
cameraRtC2W = fread(fout,12*nCam,'double');
cameraRtW2C = fread(fout,12*nCam,'double');
pointCloud = fread(fout,3*nPts,'double');
focalLen = fread(fout,2,'double');

fclose(fout);

cameraRtW2C_BundleAdjustment = reshape(cameraRtW2C,3,4,[]);
cameraRtC2W_PoseGraph = reshape(cameraRtC2W,3,4,[]);
pointCloud = reshape(pointCloud,3,[]);

% Transform the cameraRt back
for cameraID=1:size(cameraRtW2C_BundleAdjustment,3)
    cameraRtW2C_BundleAdjustment(:,:,cameraID) = transformCameraRt(cameraRtW2C_BundleAdjustment(:,:,cameraID));
end

delete(fname_in);
delete(fname_out);

toc;

% outputKeypointsPly(fullfile(out_dir, 'BA_key.ply'),pointCloud(:,reshape(find(sum(pointObserved~=0,1)>0),1,[])));

outputPly(fullfile(out_dir, 'BA.ply'), cameraRtW2C_BundleAdjustment, data);
outputPly(fullfile(out_dir, 'PG.ply'), cameraRtC2W_PoseGraph, data);

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
if exist('writeExtrinsics','var')&&writeExtrinsics
    timeStamp = getTimeStamp();
    outputCameraExtrinsics(data_dir, cameraRtW2C_BundleAdjustment, timeStamp);
end
%% output thumbnail
% outputThumbnail(data_dir, frame_dir, cameraRtC2W, timeStamp, data);

end

