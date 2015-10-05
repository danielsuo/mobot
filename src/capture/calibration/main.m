cd(fileparts(which('main')));
addpath('WarpMesh');
addpath('tools');

% Load depth, image data and calibration (R, T, KK_left, KK_right)
index = 1;
model = 'iPod Touch 6';
output_ply = sprintf('~/Downloads/out%d.ply', index);

% Load translation data
load(strcat(model, '/Calib_Results_stereo.mat'));
depth = imread(sprintf('%s/test/depth%d.png', model, index));
image = imread(sprintf('%s/test/color%d.jpg', model, index));

% Get position of left (color) camera w.r.t. right (depth) camera
Rt = [inv(R) -T/1000];

% Better way to do bit shift
depth = double(bitor(bitshift(depth,-3), bitshift(depth,16-3))) / 1000;

% Compute x, y, z coordinates from right camera (depth) perspective
XYZcamera = depth2XYZcamera(depth, KK_right);

% Warp the depth to color
[imageWarp, depth] = WarpMeshMatlabColor(XYZcamera, double(image), KK_left, Rt);

% Compute x, y, z coordinates from left camera (color) perspective
XYZcamera = depth2XYZcamera(depth, KK_left);

% Show point cloud (MATLAB treats 0,0,0 as missing point)
points = pointCloud(XYZcamera(:,:,1:3), 'Color', image);
% showPointCloud(points);

% Write point cloud
if exist(output_ply)
    delete(output_ply);
end
pcwrite(points, output_ply, 'PLYFormat', 'binary');
