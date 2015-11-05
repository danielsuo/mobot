function image2PLY(directory, calib_path, align)

    % Load calibration data
    load(calib_path);
    
    result_path = fullfile(directory, 'out.ply');
    
    % Load image data
    color = imread(fullfile(directory, 'color.jpg'));
    depth = imread(fullfile(directory, 'depth.png'));
    
    % Better way to do bit shift
    depth = double(bitor(bitshift(depth,-3), bitshift(depth,16-3))) / 1000;
    
    % Do we need to align the images or are they already fused
    if exist('align', 'var')
        % Get position of left (color) camera w.r.t. right (depth) camera
        Rt = [inv(R) -T/1000];

        % Compute x, y, z coordinates from right camera (depth) perspective
        XYZcamera = depth2XYZcamera(depth, KK_right);

        % Warp the depth to color
        [imageWarp, depth] = WarpMeshMatlabColor(XYZcamera, double(color), KK_left, Rt);
    end

    % Compute x, y, z coordinates from left camera (color) perspective
    XYZcamera = depth2XYZcamera(depth, KK_left);

    % Show point cloud (MATLAB treats 0,0,0 as missing point)
    points = pointCloud(XYZcamera(:,:,1:3), 'Color', color);

    % Write point cloud
    if exist(result_path)
        delete(result_path);
    end
    
    pcwrite(points, result_path, 'PLYFormat', 'binary');
end