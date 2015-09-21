load /n/fs/sun3d/SUN3Dv1.mat

SUN3Dpath = '/n/fs/sun3d/data/';

hasThumbnail = false(1,length(SUN3D));

for i=1:length(SUN3D)
    path = fullfile(SUN3Dpath, SUN3D{i}, 'thumbnail');
    if exist(path,'dir')
        files = dir(fullfile(path,'2013*.jpg'));
        hasThumbnail(i) = ~isempty(files);
    end
end

SUN3D = SUN3D(~hasThumbnail);
save('/n/fs/sun3d/SUN3Dv3_todo.mat','SUN3D');
