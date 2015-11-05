cd(fileparts(which('main')));
addpath('WarpMesh');
addpath('tools');

scan_path = '~/Downloads/iPhone-Daniel/2015-11-04T11.38.50.485';
scan_path = '~/Downloads/(null)/2015-11-05T13.48.33.190';

device = 'iPhone 5s';
calib_path = sprintf('%s/Calib_Results_stereo.mat', device);

image2PLY(scan_path, calib_path, true);