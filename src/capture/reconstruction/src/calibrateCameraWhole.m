% calibrate camera whole
calibrateFrameId = 1:700;
calibrateVideoLength = length(calibrateFrameId);
referenceDeviceId = 0;
sequenceRoot =['/Jan23_data/2015_1_23_16_45_41/4/'];
SUN3Dpath = '/n/fs/sun3d/robot_in_a_room/data/';
basicSetup;
for deviceId = 1:4
    dataArry{deviceId} = loadSUN3D(fullfile(sequenceRoot,[ 'device' num2str(deviceId-1)]),calibrateFrameId,SUN3Dpath);
    tmp = load([fullfile(SUN3Dpath,sequenceRoot,[ 'device' num2str(deviceId-1)],'sfm') '_BA' num2str(5) ...
            '_all' ...%'_' num2str(calibrateFrameId(1)) '_' num2str(calibrateFrameId(end)) ...
             '/BA_variables.mat'],'cameraRtC2W');
    dataArry{deviceId}.extrinsicsC2W =  tmp.cameraRtC2W;
end

%% % find match image
allimage = [dataArry{1}.image,dataArry{2}.image,dataArry{3}.image,dataArry{4}.image];
figure
BOWmodel = visualindex_build(allimage, ...
            1:length(allimage), false, 'numWords', 4000) ;
scores = BOWmodel.index.histograms' * BOWmodel.index.histograms;
%% 
RtRANSAC{1} = [eye(3),zeros(3,1)];
Scale = ones(1,4);
data.K = dataArry{1}.K;
for deviceId = 2:4
    M = zeros(length(allimage));
    M ((deviceId-1)*calibrateVideoLength+1:(deviceId)*calibrateVideoLength,(deviceId-1-1)*calibrateVideoLength+1:(deviceId-1)*calibrateVideoLength) = 1;
    scoresPair = scores.*M;
    % non max suppression
    scoresPairNMS = nonmaxsup(scoresPair, 7);
    ind = find(scoresPairNMS(:)>0.2); % threshold 
    [score_sort, perm]= sort(scoresPairNMS(ind),'descend');
    ind = ind(perm);
    [cameras_i, cameras_j] = ind2sub([length(allimage) length(allimage)],ind);

    correspondence = [];
    for frameId = 1:length(cameras_i)     
        c_ref = min(cameras_i(frameId),cameras_j(frameId));
        c_d = max(cameras_i(frameId),cameras_j(frameId));
        c_ref = mod(c_ref,calibrateVideoLength)+1;
        c_d =mod(c_d,calibrateVideoLength)+1;
        data.image = {dataArry{deviceId-1}.image{c_ref},dataArry{deviceId}.image{c_d}};
        data.depth = {dataArry{deviceId-1}.depth{c_ref},dataArry{deviceId}.depth{c_d}};
        %{
        subplot(1,2,1)
        imshow(data.image{end});
        subplot(1,2,2),imshow(data.image{end-1});
        pause;
        %}
        
        MatchPairs = align2view(data, 1,2);
        if length(MatchPairs.matches)>20
            pt1 = MatchPairs.matches(3:5,:);
            pt2 = MatchPairs.matches(8:10,:);
            % trainform to W2 cordinate 
            pt1 = transformRT(pt1,dataArry{deviceId-1}.extrinsicsC2W(:,:,c_ref));
            pt2 = transformRT(pt2,dataArry{deviceId}.extrinsicsC2W(:,:,c_d));
            correspondence = [correspondence, [pt1;pt2]];
        end
        %}
    end
    error3D_threshold =0.05;
    [RtRANSAC{deviceId}, Scale(deviceId),inliers] = ransacfitRtScale(correspondence, error3D_threshold, 0);
%transformPts = transformRT(Scale(deviceId)*correspondence(4:6,:),RtRANSAC{deviceId});
%figure,plot3(correspondence(1,:),correspondence(2,:),correspondence(3,:),'.'); axis equal;hold on;plot3(transformPts(1,:),transformPts(2,:),transformPts(3,:),'.r');

%   outputKeypointsPly(fullfile(SUN3Dpath, sequenceRoot, ['device' num2str(deviceId) '_key.ply']),correpoundence(4:6,:));
%   outputPly(fullfile(SUN3Dpath,sequenceRoot,['device' num2str(deviceId) '_test.ply']), dataArry{deviceId+1}.extrinsicsC2W, dataArry{deviceId+1},1);
%     
%   outputKeypointsPly(fullfile(SUN3Dpath, sequenceRoot, ['device' num2str(0) '_key.ply']),correpoundence(1:3,:));
%   outputPly(fullfile(SUN3Dpath,sequenceRoot,['device' num2str(0) '_test.ply']), dataArry{1}.extrinsicsC2W, dataArry{deviceId+1},1);


end
%% get golbel Rt
RtRANSACTofirst{1} = RtRANSAC{1};
for i =2:4
    RtRANSACTofirst{i} = transformcameraRtC2W(RtRANSAC{i},RtRANSACTofirst{i-1},0); 
end

for deviceId = 1:4
    % transform camera by scale
    cameraRtC2WAlign{deviceId} = transformcameraRtC2Wscale(dataArry{deviceId}.extrinsicsC2W,Scale(deviceId));
    % algin to the same device
    cameraRtC2WAlign{deviceId}= transformcameraRtC2W(cameraRtC2WAlign{deviceId},RtRANSACTofirst{deviceId}(1:3,:),0); % inverse or not ? 
    outputPly(fullfile(SUN3Dpath,sequenceRoot,['device' num2str(deviceId-1) '_cali.ply']), cameraRtC2WAlign{deviceId}, dataArry{deviceId},Scale(deviceId))
end
save(fullfile(SUN3Dpath,sequenceRoot,'Cali'),'RtRANSAC','Scale');

% imagesc(BOWmodel.index.histograms' * BOWmodel.index.histograms)