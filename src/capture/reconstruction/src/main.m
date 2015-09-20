cd(fileparts(which('main')));
directory = '/Users/danielsuo/Downloads/data/projects/sunrgbd/iPhone-Daniel';
sequence = '2015-09-02T09.16.06.847';

RGBDsfm(sequence,5, [],directory, 1);