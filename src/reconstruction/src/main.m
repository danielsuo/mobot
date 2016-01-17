cd(fileparts(which('main')));
% directory = '/home/danielsuo/Downloads/scan004/2015-10-10T21.48.29.256';
% directory = '/home/danielsuo/Downloads/scan005/2015-11-06T12.43.50.161';
% directory = '/home/danielsuo/Downloads/scan006/2015-11-06T21.28.14.013';
% directory = '/home/danielsuo/Downloads/scan007/2015-11-06T21.34.09.632';

data.image{1} = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg';
data.depth{1} = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png';
data.image{2} = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.381-0000000164.jpg';
data.depth{2} = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.381-0000000164.png';
data.K = reshape(readValuesFromTxt('/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/INTRINSICS'), 3, 3)';

out_dir = '/home/danielsuo/Dropbox/mobot/src/server/result/sift';

data_dir = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340';
% RGBDsfm(data_dir, 500, [], 1);
vl_sift_tofile(data_dir, out_dir);

%{
%% Add source paths and set up toolboxes
basicSetup

out_dir = '/home/danielsuo/mobot/src/server/result/match';

data = loadStructureIOdata(data_dir, []);
[MatchPairs, cameraRtC2W] = alignTimeBased(data);

%{
for i = 1:length(MatchPairs)
    % matches is a 10 x number of SIFT point matrix. The first 5 rows are image
    % and world coordinates of each SIFT point from the first camera pose
    % and the second 5 rows are the same from the second camera pose
    %
    % x1_i, y1_i, x1_w, y1_w, z1_w, x2_i, y2_i, x2_w, y2_w, z2_w
    match3DCoordinates = MatchPairs{i}.matches([3:5 8:10], :);

    fname_out = fullfile(out_dir, ['match' num2str(i)]);
    fout = fopen(fname_out, 'wb');
    fwrite(fout, size(match3DCoordinates, 2), 'uint32');
    fwrite(fout, single(match3DCoordinates), 'single');
    fclose(fout);
end

%}

out_dir = '/home/danielsuo/mobot/src/server/result/Rt';

for i = 1:length(MatchPairs)
   
    Rt = MatchPairs{i}.Rt';

    fname_out = fullfile(out_dir, ['Rt' num2str(i)]);
    fout = fopen(fname_out, 'wb');
    fwrite(fout, single(Rt), 'single');
    fclose(fout);
    
end

%}