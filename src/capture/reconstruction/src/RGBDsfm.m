function [cameraRtC2W,pointCloud]=RGBDsfm(sequenceName, BAmode,frameIDs, SUN3Dpath,writeExtrinsics)
%{
RGBDsfm('/2015-07-14T20.08.19.564/',5, [],'/Users/shuran/Documents/SUNrgbd_v2_data/',1);
%
RGBDsfm('/2015-09-02T09.16.06.847/',5, [],'/net/pvd00/p/sunrgbd/iPhone-Daniel/',1);

RGBDsfm('/2015-09-02T09.16.06.847/',5, [],'/Users/shuran/Documents/SUNrgbd_v2_data/',1);
%}
doloopclosure =1;
Longtrackloopclosure =0;


%write2path = fullfile(SUN3Dpath,sequenceName,'sfm');
write2path = fullfile('./image_data',sequenceName,'sfm');

if isempty(frameIDs)
     write2path = [write2path '_BA' num2str(BAmode) '_all'];
else
     write2path = [write2path '_BA' num2str(BAmode) '_' num2str(frameIDs(1)) '_' num2str(frameIDs(end))];
end

mkdir(write2path);


%SUN3Dpath = 'http://sun3d.csail.mit.edu/data/';

if ~exist('SUN3Dpath','var')
    SUN3Dpath = '/n/fs/sun3d/robot_in_a_room/';
end


%{
if matlabpool('size') ==0
    try
        matlabpool(12)
    catch
    end
end
%}
basicSetup

%% read data

% %frameIDs = 1:3:700;
% if exist('removeduplicate','var')&&removeduplicate
%     fprintf('change detection:')
%     data = loadSUN3D(sequenceName, frameIDs, SUN3Dpath);
%     tic;frameIDs =  find(findchangeframe(data.image));toc;
% end

%data = loadSUN3D(sequenceName, frameIDs, SUN3Dpath);

data = loadStructureIOdata(sequenceName,SUN3Dpath,'depth');


% show the video
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

%% loop closure


%% loop closure detection

%length(data.image) = 10; % debug

BOWmodel = BOWtrain(data);
scores = BOWmodel.index.histograms' * BOWmodel.index.histograms;
% release memory
clear BOWmodel;

% smooth out
% worry about recall? ==> use max between several columns

% remove your self?

wDistance = tril(min(1,bwdist(eye(length(data.image)))/150),-1);

scores = double(wDistance) .* full(scores);
G = fspecial('gaussian',[5 5],2);
scores = imfilter(scores,G,'same');
clear G;

% non max suppression
scoresNMS = nmsMatrix(scores, 200); %scoresNMS = nmsMatrix(scoresNMS', 200)';
%scoresNMS = nonmaxsup(scoresNMS, 5);

ind = find(scoresNMS(:)>0.2); % threshold 


[~, perm]= sort(scoresNMS(ind),'descend');
ind = ind(perm);

% for time problem, make it no longer than the length(data.image)
% if length(ind)>length(data.image)
%     ind = ind(1:length(data.image));
% end

[cameras_i, cameras_j] = ind2sub(size(scoresNMS),ind);


% visualization
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
    
    pause(0.2);
end
%}

% show the video
%{
figure
for cameraID=1:length(data.image)
    imshow(readImage(data,cameraID));
    title(sprintf('Frame %d',cameraID));
    drawnow;
end
%}




%% run loop matching
MatchPairsLoop = cell(1,length(cameras_i));
for cameraID = 1:length(cameras_i)
    MatchPairsLoop{cameraID} = align2view(data, cameras_i(cameraID), cameras_j(cameraID));
end
minAcceptableSIFT = 20;
 
cntLoopEdge = 0;
for pairID=1:length(MatchPairsLoop)
    if size(MatchPairsLoop{pairID}.matches,2)>minAcceptableSIFT
        cntLoopEdge = cntLoopEdge+1;
    end
end
fprintf('found %d good loop edges\n',cntLoopEdge); clear cntLoopEdge;

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
plot3(cameraCenters(1,1:1000),cameraCenters(2,1:1000),cameraCenters(3,1:1000),'.b', 'markersize', 0.1);
xlabel('x')
ylabel('y')
zlabel('z')

%}

clear cameras_i
clear cameras_j

%% time based reconstruction
MatchPairs = cell(1,length(data.image)-1);
parfor frameID = 1:length(data.image)-1
    MatchPairs{frameID} = align2view(data, frameID, frameID+1);
end

% naive approach: just put all results together
cameraRtC2W = repmat([eye(3) zeros(3,1)], [1,1,length(data.image)]);
for frameID = 1:length(data.image)-1
    cameraRtC2W(:,:,frameID+1) = [cameraRtC2W(:,1:3,frameID) * MatchPairs{frameID}.Rt(:,1:3) cameraRtC2W(:,1:3,frameID) * MatchPairs{frameID}.Rt(:,4) + cameraRtC2W(:,4,frameID)];
end

save(fullfile(write2path, 'cameraRt_RANSAC.mat'),'cameraRtC2W','MatchPairs','-v7.3');

fprintf('ransac all finished\n');


outputPly(fullfile(write2path, 'time.ply'), cameraRtC2W, data);



%{
% plot the pose graph
cameraCenters = reshape(cameraRtC2W(:,4,:),3,[]);
figure(100)
%plot3(cameraCenters(1,:),cameraCenters(2,:),cameraCenters(3,:),'-');
axis equal;
hold on;
grid on;
plot3(cameraCenters(1,:),cameraCenters(2,:),cameraCenters(3,:),'.r', 'markersize', 0.1);


%}



%save('all_debug.mat','-v7.3');


%% bundle adjustment

wTimePoints = 0.1;
w3D = 100;

% link track
maxNumPoints = length(data.image)*1000;
pointObserved= sparse(length(data.image),maxNumPoints);
pointObservedValue = zeros(6,maxNumPoints);

pointCloud   = zeros(3,maxNumPoints);
pointCount = 0;
pointObservedValueCount = 0;

%% time based

doLongTrack = true;

if doLongTrack

    pointCount = size(MatchPairs{1}.matches,2);
    pointObservedValueCount = size(MatchPairs{1}.matches,2)*2;
    pointObservedValue(:,1:pointObservedValueCount) = [[MatchPairs{1}.matches(1:5,:) MatchPairs{1}.matches(6:10,:)]; -wTimePoints * ones(1,pointObservedValueCount)];
    pointObserved(1,1:pointCount)=1:pointCount;
    pointObserved(2,1:pointCount)=pointCount + (1:pointCount);
    previousIndex = 1:pointCount;
    
    pointCloud(:,1:pointCount) = MatchPairs{1}.matches(3:5,:);
    
    for frameID = 2:length(data.image)-1
        [~,iA,iB] = intersect(MatchPairs{frameID-1}.matches(6:7,:)',MatchPairs{frameID}.matches(1:2,:)','rows');
        
        
        alreadyExist = false(1,size(MatchPairs{frameID}.matches,2));
        alreadyExist(iB) = true;
        newCount = sum(~alreadyExist);
        
        
        currentIndex = zeros(1,size(MatchPairs{frameID}.matches,2));
        currentIndex(iB) = previousIndex(iA);
        currentIndex(~alreadyExist) = (pointCount+1):(pointCount+newCount);
        
        pointObservedValue(1:5,pointObservedValueCount+1:pointObservedValueCount+newCount+length(currentIndex)) = [MatchPairs{frameID}.matches(1:5,~alreadyExist) MatchPairs{frameID}.matches(6:10,:)];
        pointObservedValue(6,pointObservedValueCount+1:pointObservedValueCount+newCount+length(currentIndex)) = -wTimePoints;
        
        pointObserved(frameID  ,currentIndex(~alreadyExist)) = (pointObservedValueCount+1):(pointObservedValueCount+newCount);
        pointObservedValueCount = pointObservedValueCount + newCount;
        pointObserved(frameID+1,currentIndex) = (pointObservedValueCount+1):(pointObservedValueCount+length(currentIndex));
        pointObservedValueCount = pointObservedValueCount + length(currentIndex);
        
        
        pointCloud(:,pointCount+1:pointCount+newCount) = transformRT(MatchPairs{frameID}.matches(3:5,~alreadyExist), cameraRtC2W(:,:,frameID), false);
        
        pointCount = pointCount + newCount;
        
        previousIndex = currentIndex;
    end
    
else
    for pairID = 1:length(MatchPairs)
        n = size(MatchPairs{pairID}.matches,2);

        pointObservedValue(:,pointObservedValueCount+1:pointObservedValueCount+n*2) = [[MatchPairs{pairID}.matches(1:5,:) MatchPairs{pairID}.matches(6:10,:)]; -wTimePoints * ones(1,2*size(MatchPairs{pairID}.matches,2))];

        pointObserved(MatchPairs{pairID}.i,pointCount+1:pointCount+n)=pointObservedValueCount+1  :pointObservedValueCount+n;
        pointObserved(MatchPairs{pairID}.j,pointCount+1:pointCount+n)=pointObservedValueCount+1+n:pointObservedValueCount+n*2;

        pointCloud(:,pointCount+1:pointCount+n) = transformRT(MatchPairs{pairID}.matches(3:5,:), cameraRtC2W(:,:,MatchPairs{pairID}.i), false);

        pointObservedValueCount = pointObservedValueCount+n*2;
        pointCount = pointCount+n;

    end
end


%% loop closure
if doloopclosure
    if Longtrackloopclosure

       for pairID = 1:length(MatchPairsLoop)
           if size(MatchPairsLoop{pairID}.matches,2)>minAcceptableSIFT
              
              pt1 = MatchPairsLoop{pairID}.matches(1:5,:);
              pt2 =MatchPairsLoop{pairID}.matches(6:10,:);
              [~,iA1,iB1] = intersect(pt1',pointObservedValue(1:5,:)','rows');
              [~,iA2,iB2] = intersect(pt2',pointObservedValue(1:5,:)','rows');
              [~,ib1,ib2]=intersect(iA1,iA2);
              iB1 = iB1(ib1);
              iB2 = iB2(ib2);
              
              
              % link this two point
              for kk =1:length(iB1)
                  pid1 = find(pointObserved(MatchPairsLoop{pairID}.i,:)==iB1(kk));
                  pid2 = find(pointObserved(MatchPairsLoop{pairID}.j,:)==iB2(kk));
                  ind = find(pointObserved(:,pid1)==0);
                  if ~isempty(pid1)&&~isempty(pid2)&&~isempty(ind)
                      
                      pointObserved(sub2ind(size(pointObserved),ind,repmat(pid1,[length(ind),1])))...
                          =  pointObserved(sub2ind(size(pointObserved),ind,repmat(pid2,[length(ind),1])));

                      % increase the weight 
%                       a = full(pointObserved(:,pid1));
%                       pointObservedValue(6,a(a>0)) = -1;
                      pointObservedValue(6,[iB1(kk),iB2(kk)])=-1;
                      if pid1~=pid2,
                          % remove
                          pointObserved(:,pid2) =[];
                          pointCloud(:,pid2) =[];
                          pointCount = pointCount-1;
                      end
                  end
              end
              % new pair 
              %{
                n = size(MatchPairsLoop{pairID}.matches,2);

                pointObservedValue(:,pointObservedValueCount+1:pointObservedValueCount+n*2) = [[MatchPairsLoop{pairID}.matches(1:5,:) MatchPairsLoop{pairID}.matches(6:10,:)]; -1 * ones(1,2*size(MatchPairsLoop{pairID}.matches,2))];

                pointObserved(MatchPairsLoop{pairID}.i,pointCount+1:pointCount+n)=pointObservedValueCount+1  :pointObservedValueCount+n;
                pointObserved(MatchPairsLoop{pairID}.j,pointCount+1:pointCount+n)=pointObservedValueCount+1+n:pointObservedValueCount+n*2;

                pointCloud(:,pointCount+1:pointCount+n) = transformRT(MatchPairsLoop{pairID}.matches(3:5,:), cameraRtC2W(:,:,MatchPairsLoop{pairID}.i), false);

                pointObservedValueCount = pointObservedValueCount+n*2;
                pointCount = pointCount+n;
              %}
              
              newpairId = 1:size(pt1,2);
              newpairId(iA1(ib1))=[];
              
              n = length(newpairId);
              pointObservedValue(:,pointObservedValueCount+1:pointObservedValueCount+n*2) = [pt1(:,newpairId),pt2(:,newpairId); -1 * ones(1,2*n)];
              pointObserved(MatchPairsLoop{pairID}.i,pointCount+1:pointCount+n)=pointObservedValueCount+1  :pointObservedValueCount+n;
              pointObserved(MatchPairsLoop{pairID}.j,pointCount+1:pointCount+n)=pointObservedValueCount+1+n:pointObservedValueCount+n*2;
              pointCloud(:,pointCount+1:pointCount+n) = transformRT(pt1(3:5,newpairId), cameraRtC2W(:,:,MatchPairsLoop{pairID}.i), false);
              pointObservedValueCount = pointObservedValueCount+n*2;
              pointCount = pointCount+n;
              
           end
       end
    else
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
end


save(fullfile(write2path,'MatchPairs.mat'),'MatchPairs','MatchPairsLoop','scores','scoresNMS','-v7.3');
clear MatchPairsLoop
clear MatchPairs
clear scores
clear scoresNMS

% save some memory
pointCloud = pointCloud(:,1:pointCount);
pointObserved = pointObserved(:,1:pointCount);
pointObservedValue = pointObservedValue(:,1:pointObservedValueCount);
%{
%visulize final 
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
%% bundle adjustment
outputKeypointsPly(fullfile(write2path, 'time_key.ply'),pointCloud(:,reshape(find(sum(pointObserved~=0,1)>0),1,[])));

fprintf('bundle adjusting    ...\n');
tic
%[cameraRtC2W,pointCloud] = bundleAdjustment2D3DRobustFile(cameraRtC2W,pointCloud,pointObserved, pointObservedValue, frames.K, w3D, 3);
global objectLabel;
objectLabel.length = 0;
objectLabel.objectRtO2W = zeros(3,4,objectLabel.length);
objectLabel.objectSize = zeros(objectLabel.length,3);
objectLabel.optimizationWeight = zeros(1,objectLabel.length);
[cameraRtC2W,pointCloud] = bundleAdjustment2D3DBoxFile(cameraRtC2W,pointCloud,pointObserved, pointObservedValue, data.K, w3D, BAmode);
toc;

outputKeypointsPly(fullfile(write2path, 'BA_key.ply'),pointCloud(:,reshape(find(sum(pointObserved~=0,1)>0),1,[])));

outputPly(fullfile(write2path, 'BA.ply'), cameraRtC2W, data);

fprintf('rectifying scenes ...');
% tic
% cameraRtC2W = rectifyScene(cameraRtC2W, data);
% toc

%% save all necessary works

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

save(fullfile(write2path,'BA_variables.mat'),'-v7.3');

%% output camera text file
if exist('writeExtrinsics','var')&&writeExtrinsics
    timeStamp = getTimeStamp();
    outputCameraExtrinsics(SUN3Dpath, sequenceName, cameraRtC2W, timeStamp);
end
%% output thumbnail
% outputThumbnail(SUN3Dpath, sequenceName, cameraRtC2W, timeStamp, data);

end

