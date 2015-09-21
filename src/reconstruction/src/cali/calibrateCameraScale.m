%deviceIdpick = [3,0];
%deviceIdpick = [0,1];
clear all;
deviceIdpick =  [2,3];
pickframeId =[1,167];
sequenceRoot = 'DEAB_360';
sequenceName = ['DEAB_360/device' num2str(deviceIdpick(2))];
SUN3Dpath = '/n/fs/sun3d/robot_in_a_room/';

for i = 1:length(deviceIdpick)
    dataArry{i} = loadSUN3D(['DEAB_360/device' num2str(deviceIdpick(i))],pickframeId(i),SUN3Dpath);
end

for i =1:length(dataArry)
    Frame{i} =readpoints(dataArry{i},1,0);
end

for i =1:length(Frame)
figure,
rgbImgover = showoverImage(Frame{i}(:,:,1:3), Frame{i}(:,:,4));
imshow(rgbImgover);
end

for i =1:length(Frame)
    Point{i} = [reshape(Frame{i}(:,:,5),[],1),reshape(Frame{i}(:,:,6),[],1),reshape(Frame{i}(:,:,4),[],1)];
    RGB{i} = [reshape(Frame{i}(:,:,1),[],1),reshape(Frame{i}(:,:,2),[],1),reshape(Frame{i}(:,:,3),[],1)];
end

cd /n/fs/modelnet/SUN3DV2/Multisensor/align2RGBD/
[Rt,scale] = align2RGBD(Frame{1}, Frame{2},1); %R*scale*frame2+T = frame1
cd /n/fs/sun3d/code/RGBDsfm/
Scale = [1,scale];
save(fullfile(SUN3Dpath,sequenceRoot,['scale' num2str(deviceIdpick(2)) 'To' num2str(deviceIdpick(1)) '.mat']), 'Scale' )

%{
% test
XYZtransform = transformPointCloud(Point{2}',Rt,scale);
XYZtransform = XYZtransform';

figure,
vis_point_cloud(Point{1},repmat(rand(1,3),[length(Point{1}),1]));

hold on;
vis_point_cloud(XYZtransform,repmat(rand(1,3),[length(XYZtransform),1]));

hold on;
vis_point_cloud(Point{2},repmat(rand(1,3),[length(Point{2}),1]));
%}