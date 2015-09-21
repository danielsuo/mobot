function RotateScene(sequenceName, frameIDs)


if ~exist('sequenceName','var')
    sequenceName = 'hotel_umd/maryland_hotel3';
end
if ~exist('frameIDs','var')
    frameIDs = [];
end

%basicSetup


%% read data

SUN3Dpath = '/n/fs/sun3d/data/';
data = loadSUN3D(sequenceName, frameIDs, SUN3Dpath);

if isempty(data.extrinsicsC2W)
    return;
end

fprintf('rectifying scenes ...');
tic
cameraRtC2W = rectifyScene(data.extrinsicsC2W, data);
toc

timeStamp = getTimeStamp();

%% output camera text file
outputCameraExtrinsics(SUN3Dpath, sequenceName, cameraRtC2W, timeStamp);
    

%% output thumbnail
disp('generating thumbnail...');
outputThumbnail(SUN3Dpath, sequenceName, cameraRtC2W, timeStamp, data);

return;
