function vl_sift_tofile(data_dir, out_dir, type, factor)

if nargin < 3
    type = 'single';
end

if nargin < 4
    factor = 1 / 512;
end    

data = loadStructureIOdata(data_dir, []);

loc = {};
desc = {};

parfor i=1:length(data.image)
    disp(i);
    [loc{i}, desc{i}] = visualindex_get_features([], imread(data.image{i}));
    fname_out = fullfile(out_dir, ['sift' num2str(i)]);
    fout = fopen(fname_out, 'wb');
    
    % Save number of SIFT points
    fwrite(fout, size(loc{i}, 2), 'uint32');
    fwrite(fout, single(loc{i}), type);
    
    % Normalize length of each descriptor to 1 from 512
    fwrite(fout, single(desc{i} * factor), type);
    fclose(fout);
end

end

%{
loc_read = {};
desc_read = {};

for i=1:length(data.image)
    fname_in = fullfile(out_dir, ['sift' num2str(i)]);
    fin = fopen(fname_in, 'rb');
    
    numPts = fread(fin, 1, 'uint32')
    loc_read{i} = fread(fin, numPts * 4, '*float');
    desc_read{i} = fread(fin, numPts * 128, '*float');
    
    sum(desc_read{i})
    
    fclose(fin);
end
%}