cd(fileparts(which('main')));
directory = '/home/danielsuo/Downloads/image_data/Blue';
sequence = '/2015-10-04T17.50.54.789/';

RGBDsfm(sequence, 5, [1:5], directory, 1);