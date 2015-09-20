%% calibrate deviceIdpick(2) TO deviceIdpick(1)
%deviceIdpick = [3,0]; finalpickframeId =[1:167];
%deviceIdpick =  [3,1]; finalpickframeId =[1:295];
deviceIdpick =  [2,3]; finalpickframeId =[1:167];
%deviceIdpick =  [0,1];finalpickframeId =[1:167];
load(fullfile(SUN3Dpath,sequenceRoot,['scale' num2str(deviceIdpick(2)) 'To' num2str(deviceIdpick(1)) '.mat']), 'Scale')

sequenceRoot = 'DEAB_360';
SUN3Dpath = '/n/fs/sun3d/robot_in_a_room/';



data  = loadSUN3D(['DEAB_360/device' num2str(deviceIdpick(2))], finalpickframeId, SUN3Dpath);
data2 = loadSUN3D(['DEAB_360/device' num2str(deviceIdpick(1))], 1, SUN3Dpath);

data.image{end+1} = data2.image{1};
data.depth{end+1} = data2.depth{1};
try
    figure,imshow(data.image{end});
    figure,imshow(data.image{end-1});
end

cameraRtC2W = RGBDsfm(['DEAB_360/device' num2str(deviceIdpick(2))],5, finalpickframeId,'/n/fs/sun3d/robot_in_a_room/');

RT = [eye(3),zeros(3,1)];
RT = transformcameraRtC2W(RT,cameraRtC2W(:,:,end),1);
RT = transformcameraRtC2Wscale(RT,Scale(2));
pair = align2view(data, length(data.image)-1, length(data.image),[Scale(2),Scale(1)]);
RT = transformcameraRtC2W(RT,pair.Rt,1);

save(fullfile(SUN3Dpath,sequenceRoot,['RT' num2str(deviceIdpick(2)) 'To' num2str(deviceIdpick(1))]),'RT' );