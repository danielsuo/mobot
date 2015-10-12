cd(fileparts(which('main')));
addpath('WarpMesh');
addpath('tools');

% image_path = '~/Downloads/calib_data/v5/depth';
% calib_path = '~/Downloads/calib_data/v5/calib';
% 
% for device = 1:4
%    
% %     device_calib_path = sprintf('%s/%d/Calib_Results_stereo.mat', calib_path, device);
%     device_calib_path = fullfile(calib_path, 'Calib_Results_stereo.mat');
%     
%     for scan = 1:2
%         scan_path = sprintf('%s/%d/%02d', image_path, device, scan);
%         
%         image2PLY(scan_path, device_calib_path);
%     end
%     
% end