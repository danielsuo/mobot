    
% TODO: we are calculating tanx/tany twice in main.m unnecessarily

% Inputs
% - depth: 16-bit greyscale image in meters
% - K: intrinsic matrix of depth camera
%
% Outputs
% - XYZcamera: 4 x size(depth) matrix with x, y, z, and crop mask
function XYZcamera = depth2XYZcamera(depth, K)
    % Get image dimensions
    [height, width] = size(depth);
    
    % Initialize X-coordinate (ignore calibrated principal point)
    xcoord = repmat([1:width], height, 1) - width / 2; % - K(1,3);
    
    % Initialize Y-coordinate (ignore calibrated principal point)
    ycoord = repmat([1:height]', 1, width) - height / 2; % - K(2,3);

    % Get angles
    tanx = xcoord / K(1,1);
    tany = ycoord / K(2,2);

    xcoord = depth .* tanx;
    ycoord = depth .* tany;
    zcoord = depth;

    XYZcamera = cat(3, xcoord, ycoord, zcoord, zcoord ~= 0);
end