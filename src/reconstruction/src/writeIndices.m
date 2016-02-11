function writeIndices(indices, debug_dir)
    out_dir = fullfile(debug_dir, 'indices.bin');
    fout = fopen(out_dir, 'wb');
    fwrite(fout, size(indices, 2), 'uint32');
    fwrite(fout, indices, 'uint32');
    fclose(fout);
end