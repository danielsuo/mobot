% Load data taken from Structure IO
function data = loadStructureIOdata(sequenceName, frameIDs, SUN3Dpath, depthfoldername)

% Get full file path
path2folder = fullfile(SUN3Dpath, sequenceName);

% Set default depth folder
if ~exist('depthfoldername','var')
    depthfoldername = 'depth';
end

% Get image file list
imageFiles  = dir(fullfile(path2folder,'color','*.jpg'));
depthFiles = dir(fullfile(path2folder,depthfoldername,'*.png'));

% Set default frames to go through
if length(frameIDs) == 0
    frameIDs = 1:length(imageFiles)
end
count = 0;
% Loop through all image files to get corresponding image and dpeth names
for frameID = frameIDs
    count = count + 1;
    data.image{count} = fullfile(fullfile(path2folder, 'color', imageFiles(frameID).name));
    data.depth{count} = fullfile(fullfile(path2folder, depthfoldername, depthFiles(frameID).name));
end

% Grab camera data
data.K = reshape(readValuesFromTxt(fullfile(SUN3Dpath, sequenceName, 'intrinsics.txt')), 3, 3)';
depthCam = readValuesFromTxt(fullfile(SUN3Dpath, sequenceName, 'intrinsics_d2c.txt'));
data.Kdepth = reshape(depthCam(1:9), 3, 3)';
data.RT_d2c = reshape(depthCam(10:21), 4, 3)';
