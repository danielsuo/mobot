function model = vl_kdtree_tofile(data_dir, out_dir)
    data = loadStructureIOdata(data_dir, []);
    model = visualindex_build(data.image, 1:length(data.image), false, 'numWords', 4000);
    centers = model.vocab.centers;
    weights = model.vocab.weights;

    f = fopen(fullfile(out_dir, 'centers.bof'), 'wb');
    
    fwrite(f, size(centers, 1), 'uint32');
    fwrite(f, size(centers, 2), 'uint32');
    fwrite(f, centers / 512, 'single');
    fwrite(f, weights, 'single');
    fwrite(f, 3, 'uint32'); % numTrees
    fwrite(f, 500, 'uint32'); % maxNumComparisons
    fwrite(f, 100, 'uint32'); % maxNumIterations
    fwrite(f, 1, 'uint32'); % numNeighbors
    
    fclose(f);
end