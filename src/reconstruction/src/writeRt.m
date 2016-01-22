function writeRt(Rts)
    out_dir = '/home/danielsuo/Dropbox/mobot/src/server/result/Rt';
    
    for i = 1:size(Rts, 3)
        fname_out = fullfile(out_dir, ['Rt' num2str(i)]);
        fout = fopen(fname_out, 'wb');
        fwrite(fout, Rts(:, :, i)', 'double');
        fclose(fout);
    end
end