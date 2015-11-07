cd(fileparts(which('main')));

recompile = true;
result_path = '/home/danielsuo/Downloads/scan005/2015-11-06T12.43.50.161/sfm_BA3_all';
% result_path = '/home/danielsuo/Downloads/scan006/2015-11-06T21.28.14.013/sfm_BA3_all';
% result_m  path = '/home/danielsuo/Downloads/scan007/2015-11-06T21.34.09.632/sfm_BA3_all';

if recompile
    load(sprintf('%s/data.mat', result_path));
    fname_in = '~/Downloads/tmp/results.in';
    fname_out = '~/Downloads/tmp/results.out';

    cmd = sprintf('./ba2D3D %d %f %s %s', mode, weight, fname_in, fname_out);
    fprintf('%s\n',cmd);
    system(cmd);

    % read the result back;
    fout = fopen(fname_out, 'rb');
    nCam=fread(fout,1,'uint32');
    nPts=fread(fout,1,'uint32');
    cameraRtC2W_PoseGraph = fread(fout,12*nCam,'double');
    cameraRtW2C_BundleAdjustment = fread(fout,12*nCam,'double');
    pointCloud = fread(fout,3*nPts,'double');
    focalLen = fread(fout,2,'double');

    fclose(fout);

    cameraRtW2C_BundleAdjustment = reshape(cameraRtW2C_BundleAdjustment,3,4,[]);
    cameraRtC2W_PoseGraph = reshape(cameraRtC2W_PoseGraph,3,4,[]);
    pointCloud = reshape(pointCloud,3,[]);

    % Transform the cameraRt back; we should really rename the variable to
    % cameraRtC2W_BundleAdjustment
    for cameraID=1:size(cameraRtW2C_BundleAdjustment,3)
        cameraRtW2C_BundleAdjustment(:,:,cameraID) = transformCameraRt(cameraRtW2C_BundleAdjustment(:,:,cameraID));
    end
    
    save(fullfile(out_dir, 'results.mat'))
    outputPly(fullfile(out_dir, 'BA.ply'), cameraRtW2C_BundleAdjustment, data);
    outputPly(fullfile(out_dir, 'PG.ply'), cameraRtC2W_PoseGraph, data);
else
    % load('/home/danielsuo/Downloads/scan004/2015-10-10T21.48.29.256/sfm_BA3_all/dat.mat');
    load(sprintf('%s/results.mat', result_path));
end
    
cameraCenters_TimeBased = reshape(cameraRtC2W(:,4,:),3,[]);
cameraCenters_BundleAdjustment = reshape(cameraRtW2C_BundleAdjustment(:,4,:),3,[]);
cameraCenters_PoseGraph = reshape(cameraRtC2W_PoseGraph(:,4,:),3,[]);

figure(100)
axis equal;
hold on;
grid on;

plot3(cameraCenters_TimeBased(1,:),cameraCenters_TimeBased(2,:),cameraCenters_TimeBased(3,:),'-ro', 'markersize', 5);
% plot3(cameraCenters_BundleAdjustment(1,:),cameraCenters_BundleAdjustment(2,:),cameraCenters_BundleAdjustment(3,:),'-bo', 'markersize', 5);
plot3(cameraCenters_PoseGraph(1,:),cameraCenters_PoseGraph(2,:),cameraCenters_PoseGraph(3,:),'-go', 'markersize', 5);

legend('Time-based', 'Bundle Adjustment', 'Pose Graph');

for i = 1:size(cameraCenters_TimeBased, 2)
     if mod(i, 25) == 0 || i == 1 || i == size(cameraCenters_TimeBased, 2)
        text(cameraCenters_TimeBased(1,i),cameraCenters_TimeBased(2,i), sprintf('%d', i));
%         text(cameraCenters_BundleAdjustment(1,i),cameraCenters_BundleAdjustment(2,i), sprintf('%d', i));
        text(cameraCenters_PoseGraph(1,i),cameraCenters_PoseGraph(2,i), sprintf('%d', i));
     end
end

% Draw loop closures
for pairID = size(cameraCenters_TimeBased, 2) + 1:nPairs
    i = cameraRt_ij_indices(1, pairID);
    j = cameraRt_ij_indices(2, pairID);
    
    line([cameraCenters_TimeBased(1,i), cameraCenters_TimeBased(1,j)], ...
         [cameraCenters_TimeBased(2,i), cameraCenters_TimeBased(2,j)], 'LineStyle', '-.', 'Color', 'm');
%     line([cameraCenters_BundleAdjustment(1,i), cameraCenters_BundleAdjustment(1,j)], ...
%          [cameraCenters_BundleAdjustment(2,i), cameraCenters_BundleAdjustment(2,j)], 'LineStyle', '-.', 'Color', 'c');
     line([cameraCenters_PoseGraph(1,i), cameraCenters_PoseGraph(1,j)], ...
         [cameraCenters_PoseGraph(2,i), cameraCenters_PoseGraph(2,j)], 'LineStyle', '-.', 'Color', 'b');
     
end