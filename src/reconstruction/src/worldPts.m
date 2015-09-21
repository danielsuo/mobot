function [XYZ_all ij] = worldPts(cameraRt, data, frame_subset)

XYZ_all = zeros(3, 0);
%colors = uint8(zeros(3, 0));

for i = 1 : length(frame_subset)
  frame = frame_subset(i);
  XYZcam = depth2XYZcamera(data.K, depthRead(data.depth{frame}));
  %XYZcam=readDepthScale(data, frame, im_scale);
  sz = size(XYZcam);
  sz = sz(1:2);
  Xcam = XYZcam(:,:,1);
  Ycam = XYZcam(:,:,2);
  Zcam = XYZcam(:,:,3);  
  XYZ1world = cameraRt(:,1:3,frame) * [Xcam(:)'; Ycam(:)'; Zcam(:)'] + repmat(cameraRt(:,4,frame),1,sz(1)*sz(2));
  XYZworld (:,:,1) = reshape(XYZ1world(1,:),sz);
  XYZworld (:,:,2) = reshape(XYZ1world(2,:),sz);
  XYZworld (:,:,3) = reshape(XYZ1world(3,:),sz);
  XYZworld (:,:,4) = XYZcam (:,:,4);    

  Xworld = XYZworld(:,:,1);
  Yworld = XYZworld(:,:,2);
  Zworld = XYZworld(:,:,3);
  validM = logical(XYZworld(:,:,4));
  XYZworldframe = [Xworld(:)'; Yworld(:)'; Zworld(:)'];
  valid = validM(:)';
  XYZworldframe = XYZworldframe(:,valid);

  sz = size(XYZworld);
  assert(numel(valid) == size(XYZworld, 1)*size(XYZworld, 2));
  [i j] = ind2sub(sz(1:2), find(valid));
  ij = [i; j];

  assert(size(ij, 2) == size(XYZworldframe, 2));
  XYZworldframe = double(XYZworldframe);
  XYZ_all = [XYZ_all XYZworldframe];
  
  % im = imresize(readImage(data, frame), im_scale);
  % colors = [colors zip3(im, ij)];
end
