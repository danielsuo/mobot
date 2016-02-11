function writeRt(Rts, debug_dir)
    out_dir = fullfile(debug_dir, 'Rt');
    if ~exist(out_dir, 'dir')
        mkdir(out_dir);
    end
    
    for i = 1:size(Rts, 3)
        fname_out = fullfile(out_dir, ['Rt' num2str(i)]);
        fout = fopen(fname_out, 'wb');
        fwrite(fout, Rts(:, :, i)', 'double');
        fclose(fout);
    end
end