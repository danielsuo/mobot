
addpath('~/mobot/src/capture/calibration/TOOLBOX_calib');
addpath('~/mobot/src/capture/calibration/WarpMesh');
cd(fileparts(which('iPad')));

% Get depth image -> depth
depth = imread('~/data/(null)/2015-07-08T14.20.14.591/depth/0000000647.png');
% Get color image -> image
image = imread('~/data/(null)/2015-07-08T14.20.14.591/color/0000000647.jpg');
% Get stereo calib -> K, Rt
load('~/mobot/src/capture/calibration/iPhone 5/Calib_Results_stereo.mat')

cd ('../WarpMesh');

% CAMERA PARAMETERS

% Image dimensions
[height, width] = size(depth);

% Depth is in 10ths of mm. Convert to meters
depth = double(depth) / 10000;

% Assuming depth is z coordinate
% X-coordinate
xcoord = repmat([1:width], height, 1) - width / 2;

% Y-coordinate`
ycoord = repmat([1:height]', 1, width) - height / 2;

% Get angles
tanx = xcoord / KK_right(1,1);
tany = ycoord / KK_right(2,2);

xcoord = depth .* tanx;
ycoord = depth .* tany;
zcoord = depth;

XYZcamera = cat(3, xcoord, ycoord, zcoord, zcoord ~= 0);

points = pointCloud(XYZcamera(:,:,1:3), 'Color', image);
showPointCloud(points);

pcwrite(points, '~/Downloads/test1_iPad.ply', 'PLYFormat', 'binary');