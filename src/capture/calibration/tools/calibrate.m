function stereoParams = calibrate(directory)
    color_path = fullfile(directory, 'color');
    infra_path = fullfile(directory, 'infrared');
    
    color_images = fullfile(color_path, getFilesByExtension(color_path, 'jpg'));
    infra_images = fullfile(infra_path, getFilesByExtension(infra_path, 'jpg'));
    
    % Detect checkerboards in images
    [imagePoints, boardSize, imagesUsed] = detectCheckerboardPoints(color_images, infra_images);

    % Generate world coordinates of the checkerboard keypoints
    squareSize = 30;  % in units of 'mm'
    worldPoints = generateCheckerboardPoints(boardSize, squareSize);

    % Calibrate the camera
    [stereoParams, pairsUsed, estimationErrors] = estimateCameraParameters(imagePoints, worldPoints, ...
        'EstimateSkew', false, 'EstimateTangentialDistortion', false, ...
        'NumRadialDistortionCoefficients', 2, 'WorldUnits', 'mm');
    
    % Save data
    KK_left = stereoParams.CameraParameters1.IntrinsicMatrix';
    KK_right = stereoParams.CameraParameters2.IntrinsicMatrix';

    R = stereoParams.RotationOfCamera2';
    T = stereoParams.TranslationOfCamera2';

    save(fullfile(directory, 'Calib_Results_stereo.mat'), 'R', 'T', 'KK_left', 'KK_right');
end

function files = getFilesByExtension(directory, extension)
    files = dir(directory);
    files = regexpi({files.name}, strcat('.*', extension), 'match');
    files = [files{:}];
end