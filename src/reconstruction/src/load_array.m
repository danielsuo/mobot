function result = load_array(path)
    f = fopen(path, 'rb');
    rows = fread(f, 1, 'uint32');
    cols = fread(f, 1, 'uint32');
    
    result = fread(f, [cols, rows], 'single');
end