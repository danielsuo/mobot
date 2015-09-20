% caliCamera
sequenceRoot =['/Jan23_data/2015_1_23_16_45_41/4/'];
SUN3Dpath = '/n/fs/sun3d/robot_in_a_room/data/';
basicSetup;
for deviceId = 1:4
    dataArry{deviceId} = loadSUN3D(fullfile(sequenceRoot,[ 'device' num2str(deviceId-1)]),[],SUN3Dpath);
    tmp = load([fullfile(SUN3Dpath,sequenceRoot,[ 'device' num2str(deviceId-1)],'sfm') '_BA' num2str(5) ...
            '_all' ...%'_' num2str(calibrateFrameId(1)) '_' num2str(calibrateFrameId(end)) ...
             '/BA_variables.mat'],'cameraRtC2W');
    % make first camera transformation identity 
    cameraRtC2W{deviceId} = transformcameraRtC2W(tmp.cameraRtC2W,tmp.cameraRtC2W(:,:,1),1);
    % assign 
    dataArry{deviceId}.extrinsicsC2W =  cameraRtC2W{deviceId};
end

RtRANSAC{1} = [eye(3),zeros(3,1)];
Scale = ones(1,4);
data.K = dataArry{1}.K;
for deviceId = 2:4
    fprintf('cali device %d with device %d',deviceId-1,deviceId);
    [RtRANSAC{deviceId}, Scale(deviceId)] = align2seq(dataArry{deviceId-1},dataArry{deviceId},1);
end

RtRANSACTofirst{1} = RtRANSAC{1};
for i =2:4
    RtRANSACTofirst{i} = transformcameraRtC2W(RtRANSAC{i},RtRANSACTofirst{i-1},0); 
end

for deviceId = 1:4
    % transform camera by scale
    cameraRtC2WAlign{deviceId} = transformcameraRtC2Wscale(dataArry{deviceId}.extrinsicsC2W,Scale(deviceId));
    % algin to the same device
    cameraRtC2WAlign{deviceId}= transformcameraRtC2W(cameraRtC2WAlign{deviceId},RtRANSACTofirst{deviceId}(1:3,:),0); % inverse or not ? 
    outputPly(fullfile(SUN3Dpath,sequenceRoot,['device' num2str(deviceId-1) '_cali_fun.ply']), cameraRtC2WAlign{deviceId}, dataArry{deviceId},Scale(deviceId))
end
save(fullfile(SUN3Dpath,sequenceRoot,'Cali_fun'),'RtRANSAC','Scale','cameraRtC2W');
