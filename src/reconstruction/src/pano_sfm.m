clear all
addpath /n/fs/modelnet/SUN3DV2/prepareGT;
setup_benchmark;
sequenceRoot = 'DEAB_360'
sequenceName = '/DEAB/device0';
SUN3Dpath = '/n/fs/sun3d/robot_in_a_room/Jan12_data';
%{
if ~exist(fullfile(SUN3Dpath,sequenceRoot,'finalpick'),'file')
    data = loadSUN3D(sequenceName, [], SUN3Dpath);
    [pickidnonblur,focalsmeasure] = removeblurImage(image);
    pickidnonblurind = find(pickidnonblur);
    pickids = findchangeframe(data.image(pickidnonblurind));
    finalpickframeId = pickidnonblurind(find(pickids));
    save(fullfile(SUN3Dpath,sequenceRoot,'finalpick'),'finalpickframeId');
else
    load(fullfile(SUN3Dpath,sequenceRoot,'finalpick'),'finalpickframeId');
end
%}
%load(fullfile(SUN3Dpath,sequenceRoot,'calib_linguang.mat'));
%% calibrate 
deviceIdpick = [2,3];
clear calib;
clear Scale;
T = load(fullfile(SUN3Dpath,sequenceRoot,['RT' num2str(deviceIdpick(2)) 'To' num2str(deviceIdpick(1))]),'RT' );
S = load(fullfile(SUN3Dpath,sequenceRoot,['scale' num2str(deviceIdpick(2)) 'To' num2str(deviceIdpick(1)) '.mat']), 'Scale' );
calib{deviceIdpick(1)+1} = [eye(3),zeros(3,1)];
calib{deviceIdpick(2)+1} = T.RT; 
Scale = ones(1,4);
Scale(deviceIdpick(2)+1) = S.Scale(2);
for deviceId = deviceIdpick
    sequenceName =fullfile(sequenceRoot,['device' num2str(deviceId)]);
    try
        sfm = load(fullfile(SUN3Dpath,sequenceName,'sfm_less_BA5_1_700_longLC0/',sequenceName,'BA_variables.mat'));
        pointClouddevice{deviceId+1} = sfm.pointCloud;
        cameraRtC2Wdevice{deviceId+1} = sfm.cameraRtC2W;
    catch
        [cameraRtC2Wdevice{deviceId+1},pointClouddevice{deviceId+1}]=RGBDsfm(sequenceName, 5,finalpickframeId, SUN3Dpath, write2path);
    end
    pointCloudAglin{deviceId+1} = transformRT(pointClouddevice{deviceId+1},calib{deviceId+1}(1:3,:),1)';
    % algin to first camera pose
    cameraRtC2WAglin{deviceId+1}= transformcameraRtC2W(cameraRtC2Wdevice{deviceId+1},cameraRtC2Wdevice{deviceId+1}(:,:,1),1);
    % transform camera by scale
    cameraRtC2WAglin{deviceId+1} = transformcameraRtC2Wscale(cameraRtC2WAglin{deviceId+1},Scale(deviceId+1));
    % algin to the same device
    cameraRtC2WAglin{deviceId+1}= transformcameraRtC2W(cameraRtC2WAglin{deviceId+1},calib{deviceId+1}(1:3,:),0); % inverse or not ? 
    
end

finalpickframeId = [1:700];
for deviceId=deviceIdpick
    sequenceName =fullfile(sequenceRoot,['device' num2str(deviceId)]);
    data = loadSUN3D(sequenceName, finalpickframeId, SUN3Dpath);
    outputPly(fullfile(SUN3Dpath,sequenceRoot,['device' num2str(deviceId) '_scale.ply']), cameraRtC2WAglin{deviceId+1}, data,Scale(deviceId+1))
end
%{
figure,
for deviceId=0:3
    hold on;
    pointCloudAglin{deviceId+1} = transformRT(pointCloud{deviceId+1},calib{deviceId+1}(1:3,:),1)';
    vis_point_cloud(pointCloudAglin{deviceId+1},repmat(rand(1,3),[length(pointCloudAglin{deviceId+1}),1]));
pause;
end
%}
%save(fullfile(SUN3Dpath,sequenceRoot,'sfm.mat'),'cameraRtC2W','pointCloud');