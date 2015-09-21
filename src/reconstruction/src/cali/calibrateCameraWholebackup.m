% calibrate camera whole
calibrateFrameId = 1:1000;
calibrateVideoLength = length(calibrateFrameId);
referenceDeviceId = 0;
sequenceRoot = '/Jan22_data/DEBF/';
SUN3Dpath = '/n/fs/sun3d/robot_in_a_room/data/';

for deviceId = 0:3
    dataArry{deviceId+1} = loadSUN3D(fullfile(sequenceRoot,[ 'device' num2str(deviceId)]),calibrateFrameId,SUN3Dpath);
    tmp = load([fullfile(SUN3Dpath,sequenceRoot,[ 'device' num2str(deviceId)],'sfm') ...
            '_BA' num2str(5) '_' num2str(calibrateFrameId(1)) '_' num2str(calibrateFrameId(end)) ...
             '/BA_variables.mat'],'cameraRtC2W');
    dataArry{deviceId+1}.extrinsicsC2W =  tmp.cameraRtC2W;
end

%% % find match image
allimage = [dataArry{1}.image,dataArry{2}.image,dataArry{3}.image,dataArry{4}.image];
figure
BOWmodel = visualindex_build(allimage, ...
            1:length(allimage), false, 'numWords', 4000) ;
scores = BOWmodel.index.histograms' * BOWmodel.index.histograms;


M = zeros(length(allimage));
M (calibrateVideoLength+1:end,1:calibrateVideoLength) = 1;
scores = scores.*M;
% non max suppression
scoresNMS = nonmaxsup(scores, 7);
ind = find(scoresNMS(:)>0.23); % threshold 
[score_sort, perm]= sort(scoresNMS(ind),'descend');
ind = ind(perm);
[cameras_i, cameras_j] = ind2sub([length(allimage) length(allimage)],ind);
for i =1:4
    matchFrame{i} = [];
end

for i =1:length(cameras_i)
    c_ref = min(cameras_i(i),cameras_j(i));
    c_d = max(cameras_i(i),cameras_j(i));
    sc = full(score_sort(i));
    matchFrame{floor((c_d-1)/calibrateVideoLength)+1} = ...
        [matchFrame{floor((c_d-1)/calibrateVideoLength)+1};c_ref,mod(c_d,calibrateVideoLength)+1,sc];
end
%{
for deviceId = 1:3
    for frameId = 1:200:length(matchFrame{deviceId+1})       
    data.image = {dataArry{1}.image{matchFrame{deviceId+1}(frameId,1)},dataArry{deviceId+1}.image{matchFrame{deviceId+1}(frameId,2)}};
    data.depth = {dataArry{1}.depth{matchFrame{deviceId+1}(frameId,1)},dataArry{deviceId+1}.depth{matchFrame{deviceId+1}(frameId,2)}};
    subplot(1,2,1);imshow(data.image{end});title([num2str(matchFrame{deviceId+1}(frameId,3))]);
    subplot(1,2,2);imshow(data.image{end-1});title(['device' num2str(deviceId) 'frame:' num2str(frameId)])
    pause;
    end
end
%}
%% 
RtRANSAC{1} = [eye(3),zeros(3,1)];
Scale = ones(1,4);
data.K = dataArry{1}.K;
for deviceId = [1,3]
    
    calibrateFrameIdUse = matchFrame{deviceId+1};
    
    correspondence = [];
    for frameId = 1:length(calibrateFrameIdUse(:,1))               
        data.image = {dataArry{1}.image{calibrateFrameIdUse(frameId,1)},dataArry{deviceId+1}.image{calibrateFrameIdUse(frameId,2)}};
        data.depth = {dataArry{1}.depth{calibrateFrameIdUse(frameId,1)},dataArry{deviceId+1}.depth{calibrateFrameIdUse(frameId,2)}};
        %figure,imshow(data.image{end});figure,imshow(data.image{end-1})

        MatchPairs = align2view(data, 1,2);
       
       
        if length(MatchPairs.matches)>20
            pt1 = MatchPairs.matches(3:5,:);
            pt2 = MatchPairs.matches(8:10,:);
            % trainform to W2 cordinate 
            pt1 = transformRT(pt1,dataArry{1}.extrinsicsC2W(:,:,calibrateFrameIdUse(frameId,1)));
            pt2 = transformRT(pt2,dataArry{deviceId+1}.extrinsicsC2W(:,:,calibrateFrameIdUse(frameId,2)));
            correspondence = [correspondence, [pt1;pt2]];
        end
    end
    error3D_threshold =0.05;
    [RtRANSAC{deviceId+1}, Scale(deviceId+1),inliers] = ransacfitRtScale(correspondence, error3D_threshold, 0);
%   transformPts = transformRT(Scale(deviceId+1)*correpoundence(4:6,:),RtRANSAC{deviceId+1});
%   figure,plot3(correpoundence(1,:),correpoundence(2,:),correpoundence(3,:),'.'); axis equal;hold on;plot3(transformPts(1,:),transformPts(2,:),transformPts(3,:),'.r');

%   outputKeypointsPly(fullfile(SUN3Dpath, sequenceRoot, ['device' num2str(deviceId) '_key.ply']),correpoundence(4:6,:));
%   outputPly(fullfile(SUN3Dpath,sequenceRoot,['device' num2str(deviceId) '_test.ply']), dataArry{deviceId+1}.extrinsicsC2W, dataArry{deviceId+1},1);
%     
%   outputKeypointsPly(fullfile(SUN3Dpath, sequenceRoot, ['device' num2str(0) '_key.ply']),correpoundence(1:3,:));
%   outputPly(fullfile(SUN3Dpath,sequenceRoot,['device' num2str(0) '_test.ply']), dataArry{1}.extrinsicsC2W, dataArry{deviceId+1},1);


end
for deviceId = [1,3]
    % transform camera by scale
    cameraRtC2WAlign{deviceId+1} = transformcameraRtC2Wscale(dataArry{deviceId+1}.extrinsicsC2W,Scale(deviceId+1));
    % algin to the same device
    cameraRtC2WAlign{deviceId+1}= transformcameraRtC2W(cameraRtC2WAlign{deviceId+1},RtRANSAC{deviceId+1}(1:3,:),0); % inverse or not ? 
    outputPly(fullfile(SUN3Dpath,sequenceRoot,['device' num2str(deviceId) '_cali.ply']), cameraRtC2WAlign{deviceId+1}, dataArry{deviceId+1},Scale(deviceId+1))
end
save(fullfile(SUN3Dpath,sequenceRoot,'Cali'),'RtRANSAC','Scale');

% imagesc(BOWmodel.index.histograms' * BOWmodel.index.histograms)
