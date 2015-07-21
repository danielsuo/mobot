cd(fileparts(which('processImages')));

model = 'iPhone 6 Plus';
path = strcat('../', model);
ext = 'png';

f = dir(strcat(path, '/*.', ext));
f = {f.name};
% f = regexpi({f.name},'infrared','match');
% f = [f{:}];

for file = f
    I = imread(strcat(path, '/', file{1}));
    I = imadjust(I);
    imwrite(I, strcat(path, '/new_', file{1}));
end

f = dir('*.png');

for file = {f.name}
    I = imread(file{1});
    file = strrep(file{1}, 'png', 'tif');
    file = strrep(file, 'new_', '');
    imwrite(I, file);
end