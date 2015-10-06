cd(fileparts(which('main')));
directory = '/Users/danielsuo/mobot/src/capture/server/image_data/Blue';
sequence = '/2015-10-04T17.50.54.789/';

RGBDsfm(sequence, 5, [1:2], directory, 1);