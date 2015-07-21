function [imageWarp, depth] = WarpMeshMatlabColor(XYZcamera, image, K, Rt)

mex WarpMesh/WarpMeshColor.cpp -outdir WarpMesh -lGLU -lOSMesa -I/opt/X11/include/ -L/opt/X11/lib/

XYZcamera = double(XYZcamera);

X = reshape(XYZcamera,[],4)';
X = X(1:3,:);
X = transformPointCloud(X,Rt);
X = reshape(X',[480,640,3]);
XYZcamera(:,:,1) = X(:,:,1);
XYZcamera(:,:,2) = X(:,:,2);
XYZcamera(:,:,3) = X(:,:,3) .* double(XYZcamera(:,:,4)~=0);

P = [K [0;0;0]];
XYZcamera(:,:,1) = -XYZcamera(:,:,1);

image = double(image);
XYZcamera(:,:,4) = image(:,:,1) + image(:,:,2) * 256  + image(:,:,3) * 256 * 256;

[imageWarp, depth]=WarpMeshColor(P, XYZcamera);

imageWarp = imageWarp';
imageWarp = imageWarp(:,end:-1:1);

imageWarp = double(imageWarp);

R = mod(imageWarp,256);
G = mod(floor(imageWarp/256),256);
B = floor(imageWarp/(256*256));

imageWarp(:,:,1) = R;
imageWarp(:,:,2) = G;
imageWarp(:,:,3) = B;

depth = depth';
depth = depth(end:-1:1,end:-1:1);

z_near = 0.3;
z_far_ratio = 1.2;
depth = z_near./(1-single(depth)/2^32);
maxDepth = max(depth(abs(depth) < 100));

cropmask = (depth < z_near) | (depth > z_far_ratio * maxDepth);
depth(cropmask) = 0;

imageWarp = uint8(imageWarp);

if ~any(imageWarp(:))
    depth = single(zeros(size(depth)));
    return
end