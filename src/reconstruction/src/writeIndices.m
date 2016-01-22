function writeIndices(indices)
    out_dir = '/home/danielsuo/Dropbox/mobot/src/server/result/indices.bin';
    fout = fopen(out_dir, 'wb');
    fwrite(fout, size(indices, 2), 'uint32');
    fwrite(fout, indices, 'uint32');
    fclose(fout);
end