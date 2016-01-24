% function model = vl_kdtree_tofile(data_dir, out_dir)
%     data = loadStructureIOdata(data_dir, []);
%     model = visualindex_build(data.image, 1:length(data.image), false, 'numWords', 4000);
function vl_kdtree_tofile(model, type, factor)

if nargin < 2
    type = 'single';
end

if nargin < 3
    factor = 1 / 512;
end    
centers = model.vocab.centers;
weights = model.vocab.weights;

out_dir = '/home/danielsuo/Dropbox/mobot/src/server/result/kdtree';

f = fopen(fullfile(out_dir, 'centers.bof'), 'wb');

fwrite(f, size(centers, 1), 'uint32');
fwrite(f, size(centers, 2), 'uint32');
fwrite(f, centers * factor, type);
fwrite(f, weights, type);
fwrite(f, 3, 'uint32'); % numTrees
fwrite(f, 500, 'uint32'); % maxNumComparisons
fwrite(f, 100, 'uint32'); % maxNumIterations
fwrite(f, 1, 'uint32'); % numNeighbors

fclose(f);

end