function writeMatch2D(frameID_i, frameID_j, matchPointsID_i, matchPointsID_j, SIFTdes_i, SIFTdes_j)
    out_dir = '/home/danielsuo/Dropbox/mobot/src/server/result/match';
    
    fname_out = fullfile(out_dir, ['orig_match' num2str(frameID_i) '_' num2str(frameID_j)]);
    fout = fopen(fname_out, 'wb');
    fwrite(fout, length(matchPointsID_i), 'uint32');
    fwrite(fout, uint32(matchPointsID_i), 'uint32');
    fwrite(fout, uint32(matchPointsID_j), 'uint32');
    fwrite(fout, single(SIFTdes_i / 512), 'single');
    fwrite(fout, single(SIFTdes_j / 512), 'single');
    
    fclose(fout);
end