cd(fileparts(which('main')));
% directory = '/home/danielsuo/Downloads/scan004/2015-10-10T21.48.29.256';
% directory = '/home/danielsuo/Downloads/scan005/2015-11-06T12.43.50.161';
% directory = '/home/danielsuo/Downloads/scan006/2015-11-06T21.28.14.013';
% directory = '/home/danielsuo/Downloads/scan007/2015-11-06T21.34.09.632';
data_dir = '/home/danielsuo/mobot/src/server/build/image_data/iPhone-Daniel/2015-11-25T16.55.55.347';
out_dir = '/home/danielsuo/mobot/src/server/result';

% RGBDsfm(data_dir, 500, [], 1);
vl_sift_tofile(data_dir, out_dir);