% Load data taken from Structure IO
function data = loadStructureIOdata(directory, frameIDs)

% Get image file list
imageFiles  = dir(fullfile(directory, 'color', '*.jpg'));
depthFiles = dir(fullfile(directory, 'depth', '*.png'));

% Set default frames to go through
if length(frameIDs) == 0
    frameIDs = 1:length(imageFiles);
end

% Loop through all image files to get corresponding image and depth names
count = 0;
for frameID = frameIDs
    count = count + 1;
    data.image{count} = fullfile(fullfile(directory, 'color', imageFiles(frameID).name));
    data.depth{count} = fullfile(fullfile(directory, 'depth', depthFiles(frameID).name));
end

% Grab camera data
data.K = reshape(readValuesFromTxt(fullfile(directory, 'intrinsics.txt')), 3, 3)';
% depthCam = readValuesFromTxt(fullfile(directory, 'intrinsics_d2c.txt'));
% data.Kdepth = reshape(depthCam(1:9), 3, 3)';
% data.RT_d2c = reshape(depthCam(10:21), 4, 3)';
