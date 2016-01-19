function vl_sift_tofile(data_dir, out_dir)

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
    fwrite(fout, single(loc{i}), 'single');
    
    % Normalize length of each descriptor to 1 from 512
    fwrite(fout, single(desc{i}) / 512, 'single');
    fclose(fout);
end

end