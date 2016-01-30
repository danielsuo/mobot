frameID_i = 163;
frameID_j = 212;
image_i_path = '/home/danielsuo/Dropbox/mobot/src/server/lib/cuBoF/lib/cuSIFT/build/color1.jpg';
image_j_path = '/home/danielsuo/Dropbox/mobot/src/server/lib/cuBoF/lib/cuSIFT/build/color2.jpg';
depth_i_path = '/home/danielsuo/Dropbox/mobot/src/server/lib/cuBoF/lib/cuSIFT/build/depth1.png';
depth_j_path = '/home/danielsuo/Dropbox/mobot/src/server/lib/cuBoF/lib/cuSIFT/build/depth2.png';

error3D_threshold = 0.05;
error3D_threshold2 = error3D_threshold ^ 2;
K = reshape(readValuesFromTxt('/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/INTRINSICS'), 3, 3)';

outdir = '/home/danielsuo/Dropbox/mobot/src/server/result/ransac';
fname_out = fullfile(outdir, ['ransac' num2str(frameID_i) '_' num2str(frameID_j)]);

image_i = imread(image_i_path);
image_j = imread(image_j_path);

XYZcam_i = depth2XYZcamera(K, depthRead(depth_i_path, []));
XYZcam_j = depth2XYZcamera(K, depthRead(depth_j_path, []));

[SIFTloc_i,SIFTdes_i] = visualindex_get_features([], image_i);
SIFTloc_i = SIFTloc_i([2,1],:);

[SIFTloc_j,SIFTdes_j] = visualindex_get_features([], image_j);
SIFTloc_j = SIFTloc_j([2,1],:);

[matchPointsID_i, matchPointsID_j] = matchSIFTdesImagesBidirectional(SIFTdes_i, SIFTdes_j);
fprintf('with %d matching \n', length(matchPointsID_i));

SIFTloc_i = SIFTloc_i(:,matchPointsID_i);
SIFTloc_j = SIFTloc_j(:,matchPointsID_j);

% Make sure the sift points are greater than 1 and less than the size of
% the image for both images
posSIFT_i = round(SIFTloc_i);
valid_i = (1<=posSIFT_i(1,:)) & (posSIFT_i(1,:)<=size(image_i,1)) & (1<=posSIFT_i(2,:)) & (posSIFT_i(2,:)<=size(image_i,2));
posSIFT_j = round(SIFTloc_j);
valid_j = (1<=posSIFT_j(1,:)) & (posSIFT_j(1,:)<=size(image_i,1)) & (1<=posSIFT_j(2,:)) & (posSIFT_j(2,:)<=size(image_i,2));
valid = valid_i & valid_j;

posSIFT_i = posSIFT_i(:,valid);
SIFTloc_i = SIFTloc_i(:,valid);
posSIFT_j = posSIFT_j(:,valid);
SIFTloc_j = SIFTloc_j(:,valid);        
        
Xcam_i = XYZcam_i(:,:,1);
Ycam_i = XYZcam_i(:,:,2);
Zcam_i = XYZcam_i(:,:,3);

validM_i = logical(XYZcam_i(:,:,4));

ind_i = sub2ind([size(image_i,1) size(image_i,2)],posSIFT_i(1,:),posSIFT_i(2,:));
valid_i = validM_i(ind_i);

Xcam_j = XYZcam_j(:,:,1);
Ycam_j = XYZcam_j(:,:,2);
Zcam_j = XYZcam_j(:,:,3);

validM_j = logical(XYZcam_j(:,:,4)); 
ind_j = sub2ind([size(image_i,1) size(image_i,2)],posSIFT_j(1,:),posSIFT_j(2,:));
valid_j = validM_j(ind_j);

valid = valid_i & valid_j;

ind_i = ind_i(valid);
P3D_i = [Xcam_i(ind_i); Ycam_i(ind_i); Zcam_i(ind_i)];
ind_j = ind_j(valid);
P3D_j = [Xcam_j(ind_j); Ycam_j(ind_j); Zcam_j(ind_j)];

SIFTloc_i = SIFTloc_i(:,valid);
SIFTloc_j = SIFTloc_j(:,valid);
fprintf('with %d matching with 3D points\n', size(SIFTloc_i, 2));

% try
    [RtRANSAC, inliers, indices] = ransacfitRt([P3D_i; P3D_j], error3D_threshold, 1);
    fprintf('frame %d + %d: # ransac inliers = %d/%d = %f%%', frameID_i, frameID_j, length(inliers), size(P3D_i,2), length(inliers)/size(P3D_i,2)*100);
% catch
%     fprintf('frame %d + %d: # ransac FAILURE = 0/%d = %f%%', frameID_i, frameID_j, size(P3D_i,2), 0);
%     fprintf(' [I|0]');
%     
%     RtRANSAC = [eye(3) zeros(3,1)];
% end

RtRANSAC

fout = fopen(fname_out, 'wb');
fwrite(fout, size(P3D_i, 2), 'uint32');
fwrite(fout, size(indices, 2), 'uint32');
fwrite(fout, P3D_i, 'single');
fwrite(fout, P3D_j, 'single');
fwrite(fout, indices, 'uint32');
fwrite(fout, RtRANSAC', 'single');
fclose(fout);