cd(fileparts(which('main')));

recompile = true;
result_path = '/home/danielsuo/Dropbox/mobot/src/server/build/image_data/device1/2016-01-11T10.10.44.340/sfm_BA500_all';
%result_path = '/home/danielsuo/Downloads/scan004/2015-10-10T21.48.29.256/sfm_BA500_all';
% result_path = '/home/danielsuo/Downloads/scan006/2015-11-06T21.28.14.013/sfm_BA3_all';
% result_m  path = '/home/danielsuo/Downloads/scan007/2015-11-06T21.34.09.632/sfm_BA3_all';

NUM_BA_POINTS = 500;

if recompile
    load(sprintf('%s/data.mat', result_path));
    fname_in = '~/Downloads/tmp/results.in';
    fname_out = '~/Downloads/tmp/results.out';

    diary '~/Downloads/tmp/diary.txt'
%     cmd = sprintf('../../server/lib/Cerberus/build/Cerberus %d %f %s %s', mode, NUM_BA_POINTS, fname_in, fname_out);
    cmd = sprintf('../../server/build/MobotServer %d %f %s %s', mode, NUM_BA_POINTS, fname_in, fname_out);
    fprintf('%s\n',cmd);
    system(cmd);
    diary off

    % read the result back;
    fout = fopen(fname_out, 'rb');
    clear cameraRtC2W_PoseGraph;
    cameraRtC2W_PoseGraph = fread(fout,12*nCam,'double');
    
    fclose(fout);

    cameraRtC2W_PoseGraph = reshape(cameraRtC2W_PoseGraph,3,4,[]);
    
    save(fullfile(out_dir, 'results.mat'))
    outputPly(fullfile(out_dir, 'PG.ply'), cameraRtC2W_PoseGraph, data);
else
    % load('/home/danielsuo/Downloads/scan004/2015-10-10T21.48.29.256/sfm_BA3_all/dat.mat');
    load(sprintf('%s/results.mat', result_path));
end
    
cameraCenters_TimeBased = reshape(cameraRtC2W(:,4,:),3,[]);
cameraCenters_PoseGraph = reshape(cameraRtC2W_PoseGraph(:,4,:),3,[]);

figure(100)
axis equal;
hold on;
grid on;

plot3(cameraCenters_TimeBased(1,:),cameraCenters_TimeBased(2,:),cameraCenters_TimeBased(3,:),'-ro', 'markersize', 5);
plot3(cameraCenters_PoseGraph(1,:),cameraCenters_PoseGraph(2,:),cameraCenters_PoseGraph(3,:),'-go', 'markersize', 5);

legend('Time-based', ...
       'Pose Graph');

for i = 1:size(cameraCenters_TimeBased, 2)
     if mod(i, 25) == 0 || i == 1 || i == size(cameraCenters_TimeBased, 2)
        text(cameraCenters_TimeBased(1,i),cameraCenters_TimeBased(2,i), sprintf('%d', i));
        text(cameraCenters_PoseGraph(1,i),cameraCenters_PoseGraph(2,i), sprintf('%d', i));
     end
end

% Draw loop closures
for pairID = size(cameraCenters_TimeBased, 2) + 1:nPairs
    i = cameraRt_ij_indices(1, pairID);
    j = cameraRt_ij_indices(2, pairID);
    
    line([cameraCenters_TimeBased(1,i), cameraCenters_TimeBased(1,j)], ...
         [cameraCenters_TimeBased(2,i), cameraCenters_TimeBased(2,j)], 'LineStyle', '-.', 'Color', 'm');
     line([cameraCenters_PoseGraph(1,i), cameraCenters_PoseGraph(1,j)], ...
         [cameraCenters_PoseGraph(2,i), cameraCenters_PoseGraph(2,j)], 'LineStyle', '-.', 'Color', 'b');
     
end