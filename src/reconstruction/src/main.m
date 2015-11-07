cd(fileparts(which('main')));
directory = '/home/danielsuo/Downloads/scan004/2015-10-10T21.48.29.256';
directory = '/home/danielsuo/Downloads/scan005/2015-11-06T12.43.50.161';
% directory = '/home/danielsuo/Downloads/scan006/2015-11-06T21.28.14.013';
% directory = '/home/danielsuo/Downloads/scan007/2015-11-06T21.34.09.632';

RGBDsfm(directory, 3, [], 1);