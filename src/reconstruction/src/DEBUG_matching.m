data_dir = '/home/danielsuo/Dropbox/mobot/src/server/lib/cuBoF/lib/cuSIFT/test/data/';

data.image{1} = fullfile(data_dir, 'color1.jpg');
data.image{2} = fullfile(data_dir, 'color2.jpg');
data.depth{1} = fullfile(data_dir, 'depth1.png');
data.depth{2} = fullfile(data_dir, 'depth2.png');

data.K = reshape(readValuesFromTxt(fullfile(data_dir, 'INTRINSICS')), 3, 3)';

result = align2view(data, 1, 2);

writeMatch(result, data_dir);

if ~exist(fullfile(data_dir, 'sift'), 'dir')
   mkdir(fullfile(data_dir, 'sift')); 
end

for i = 1:length(data.image)
    [loc{i}, desc{i}] = visualindex_get_features([], imread(data.image{i}));
    fname_out = fullfile(fullfile(data_dir, 'sift'), ['sift' num2str(i)]);
    fout = fopen(fname_out, 'wb');
    
    % Save number of SIFT points
    fwrite(fout, size(loc{i}, 2), 'uint32');
    fwrite(fout, single(loc{i} - 1), 'single');
    
    % Normalize length of each descriptor to 1 from 512
    fwrite(fout, single(desc{i} / 512), 'single');
    fclose(fout);
end

depth = depthRead(data.depth{1}, data);
depth = depth2XYZcamera(data.K, depthRead(data.depth{1}, data));