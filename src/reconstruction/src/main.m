cd(fileparts(which('main')));
% directory = '/Users/danielsuo/Downloads/image_data/Blue';
% sequence = '/2015-10-04T17.50.54.789/';

diary('log.txt');
directory = '/Users/danielsuo/Downloads/calib_data/scan004';
sequence = '/2015-10-10T21.48.29.256/';

RGBDsfm(sequence, 5, [], directory, 1);

% for i = 229:229
%     RGBDsfm(sequence, 5, [i:i+1], directory, 1);
% end

% Y = [cos(pi/2)        0 -sin(pi/2)        0; ...
%             0         1         0         0; ...
%      sin(pi/2)        0         cos(pi/2) 0; ...
%             0         0                   0 1];
% ty = affine3d(Y);
% 
% Z = [cos(pi/2) sin(pi/2) 0 0; ...
%     -sin(pi/2) cos(pi/2) 0 0; ...
%             0         0  1 0; ...
%             0         0 0 1];
% tz = affine3d(Z);
% 
% image_data = 'image_data/2015-10-04T17.50.23.987';
% image_dirs = dir(image_data);
% 
% for i = 4:length(image_dirs)
%     path = fullfile(image_data, image_dirs(i).name, 'time.ply');
%     pc = pcread(path);
% %     pc = pctransform(pc,ty);
% %     pc = pctransform(pc,tz);
%     pcshow(pc);
% 
%     name = strsplit(path, '/');
%     image_path = char(fullfile(image_data, strcat(name(3), '.jpg')));
% 
%     F = getframe(gcf);
%     [X, Map] = frame2im(F);
% 
%     imwrite(X, image_path);
% end
% 


diary OFF