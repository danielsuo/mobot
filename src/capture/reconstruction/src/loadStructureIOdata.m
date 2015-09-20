function data = loadStructureIOdata(sequenceName,SUN3Dpath,depthfoldername)
path2folder = fullfile(SUN3Dpath,sequenceName);
if ~exist('depthfoldername','var')
    depthfoldername = 'depth';
end
imageFiles  = dir(fullfile(path2folder,'color','*.jpg'));
depthFiles = dir(fullfile(path2folder,depthfoldername,'*.png'));
cnt = 0;
for frameID=1:length(imageFiles)
    cnt = cnt + 1;        
    data.image{cnt} = fullfile(fullfile(path2folder,'color',imageFiles(frameID).name));
    data.depth{cnt} = fullfile(fullfile(path2folder,depthfoldername,depthFiles(frameID).name));
end

data.K = reshape(readValuesFromTxt(fullfile(SUN3Dpath,sequenceName,'intrinsics.txt')),3,3)';
depthCam = readValuesFromTxt(fullfile(SUN3Dpath,sequenceName,'intrinsics.txt'));
data.Kdepth = reshape(depthCam(1:9),3,3)';
data.RT_d2c = reshape(depthCam(10:21),4,3)';

%{       
load('/Users/shuran/Dropbox/RIM/calibration/iPhone 6 Plus/Calib_Results_stereo.mat');
%load('//net/pvd00/p/sunrgbd/sfm/Calib_Results_stereo.mat');       

data.K = KK_left;
data.Kdepth = KK_right;
RT_d2c = [R T/1000];
data.RT_d2c = RT_d2c;
%}
