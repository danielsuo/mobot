function depth = depthRead(filename,data)
    depth = imread(filename);
    depth = bitor(bitshift(depth,-3), bitshift(depth,16-3));
    depth = single(depth)/1000;
    if isfield(data,'R_d2c')
       XYZcamera = depth2XYZcamera(depth, data.Kdepth);
       % Warp the color to the depth
       
       [~, depth] = WarpMeshMatlab(XYZcamera, ones([size(XYZcamera,1),size(XYZcamera,2)]), data.K, data.RT_d2c);

    end
end