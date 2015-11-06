cd(fileparts(which('main')));

load('/home/danielsuo/Downloads/scan004/2015-10-10T21.48.29.256/sfm_BA3_all/dat.mat');
load('/home/danielsuo/Downloads/scan005/2015-11-06T12.43.50.161/sfm_BA3_all/dat.mat');

cameraCenters_TimeBased = reshape(cameraRtC2W(:,4,:),3,[]);
cameraCenters_BundleAdjustment = reshape(cameraRtW2C_BundleAdjustment(:,4,:),3,[]);
cameraCenters_PoseGraph = reshape(cameraRtC2W_PoseGraph(:,4,:),3,[]);

figure(100)
axis equal;
hold on;
grid on;

plot3(cameraCenters_TimeBased(1,:),cameraCenters_TimeBased(2,:),cameraCenters_TimeBased(3,:),'-ro', 'markersize', 5);
plot3(cameraCenters_BundleAdjustment(1,:),cameraCenters_BundleAdjustment(2,:),cameraCenters_BundleAdjustment(3,:),'-bo', 'markersize', 5);
plot3(cameraCenters_PoseGraph(1,:),cameraCenters_PoseGraph(2,:),cameraCenters_PoseGraph(3,:),'-go', 'markersize', 5);

legend('Time-based', 'Bundle Adjustment', 'Pose Graph');

for i = 1:size(cameraCenters_TimeBased, 2)
     if mod(i, 25) == 0 || i == 1 || i == size(cameraCenters_TimeBased, 2)
        text(cameraCenters_TimeBased(1,i),cameraCenters_TimeBased(2,i), sprintf('%d', i));
        text(cameraCenters_BundleAdjustment(1,i),cameraCenters_BundleAdjustment(2,i), sprintf('%d', i));
        text(cameraCenters_PoseGraph(1,i),cameraCenters_PoseGraph(2,i), sprintf('%d', i));
     end
end

% Draw loop closures
for pairID =  1:nPairs
    i = cameraRt_ij_indices(1, pairID);
    j = cameraRt_ij_indices(2, pairID);
    
    line([cameraCenters_TimeBased(1,i), cameraCenters_TimeBased(1,j)], ...
         [cameraCenters_TimeBased(2,i), cameraCenters_TimeBased(2,j)], 'LineStyle', '-.', 'Color', 'r');
    line([cameraCenters_BundleAdjustment(1,i), cameraCenters_BundleAdjustment(1,j)], ...
         [cameraCenters_BundleAdjustment(2,i), cameraCenters_BundleAdjustment(2,j)], 'LineStyle', '-.', 'Color', 'b');
     line([cameraCenters_PoseGraph(1,i), cameraCenters_PoseGraph(1,j)], ...
         [cameraCenters_PoseGraph(2,i), cameraCenters_PoseGraph(2,j)], 'LineStyle', '-.', 'Color', 'g');
     
end