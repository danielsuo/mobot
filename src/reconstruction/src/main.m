cd(fileparts(which('main')));
basicSetup;

%{
% directory = '/home/danielsuo/Downloads/scan004/2015-10-10T21.48.29.256';
% directory = '/home/danielsuo/Downloads/scan005/2015-11-06T12.43.50.161';
% directory = '/home/danielsuo/Downloads/scan006/2015-11-06T21.28.14.013';
% directory = '/home/danielsuo/Downloads/scan007/2015-11-06T21.34.09.632';

data.image{1} = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg';
data.depth{1} = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png';
data.image{2} = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.381-0000000164.jpg';
data.depth{2} = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.381-0000000164.png';
data.K = reshape(readValuesFromTxt('/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/INTRINSICS'), 3, 3)';
%}
out_dir = '/home/danielsuo/Dropbox/mobot/src/server/result/sift';

data_dir = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340';
RGBDsfm(data_dir, 500, []);
% vl_sift_tofile(data_dir, out_dir);

% load('/home/danielsuo/Dropbox/mobot/src/server/result/kdtree/model.mat');

% data = loadStructureIOdata(data_dir, []);
% [MatchPairs, cameraRtC2W] = alignTimeBased(data);

% outputPly(fullfile(out_dir, 'time.ply'), cameraRtC2W, data);


%{
data_dir = '/home/danielsuo/Dropbox/mobot/src/server/build/scan006_2';
image_data_dir = fullfile(data_dir, 'image_data');

device_dirs = {fullfile(image_data_dir, 'device1', '2016-02-08T14.45.20.424'), ...
    fullfile(image_data_dir, 'device2', '2016-02-08T14.45.20.418'), ...
    fullfile(image_data_dir, 'device3', '2016-02-08T14.45.20.387'), ...
    fullfile(image_data_dir, 'device4', '2016-02-08T14.45.20.540')};

RGBDsfm_pano(data_dir, image_data_dir, device_dirs);
%}