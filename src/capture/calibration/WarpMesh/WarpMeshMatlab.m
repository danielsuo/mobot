function [label,depth] = WarpMeshMatlab(XYZcamera, labelNow, K, Rt)

%mex WarpMesh.cpp -lGLU -lOSMesa

XYZcamera = double(XYZcamera);

XYZcamera(:,:,4) = XYZcamera(:,:,4) .* double(labelNow);

X = reshape(XYZcamera,[],4)';
X = X(1:3,:);
X = transformPointCloud(X,Rt);
X = reshape(X',[480,640,3]);
XYZcamera(:,:,1) = X(:,:,1);
XYZcamera(:,:,2) = X(:,:,2);
XYZcamera(:,:,3) = X(:,:,3);

P = [K [0;0;0]];
XYZcamera(:,:,1) = -XYZcamera(:,:,1);

[label,depth]=WarpMesh(P,640,480,XYZcamera);

label = label';
label = label(:,end:-1:1);

depth = depth';
depth = depth(end:-1:1,end:-1:1);

if ~any(label(:))
    depth = single(zeros(size(depth)));
    return
end

% z_near = 0.3;
% z_far_ratio = 1.2;
% depth = z_near./(1-single(depth)/2^32);
% maxDepth = max(depth(abs(depth) < 100));
% 
% cropmask = (depth < z_near) | (depth > z_far_ratio * maxDepth);
% depth(cropmask) = 0;
