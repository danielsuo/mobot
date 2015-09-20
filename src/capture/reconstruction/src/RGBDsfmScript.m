% ssh ionic.cs.princeton.edu
% cd /n/fs/sun3d/robot_in_a_room/code/RGBDsfm/
% /n/fs/vision/ionic/starter.sh RGBDsfmScript 30000mb 40:00:00 4 24 1 

% to check jobs: showq
% qstat
% to kill jobs: qdel
function RGBDsfmScript(id)
    diviceId =[0,1,2,3];
    action = [0,1,2,3,4,5,6,7,8];
    [d,a]=ndgrid(1:length(diviceId),1:length(action));
    SUN3Dpath =  '/n/fs/sun3d/robot_in_a_room/data';
    sequenceName = ['/Jan23_data/2015_1_23_16_45_41/' num2str(action(a(id)))  '/device' num2str(diviceId(d(id)))];
    if 1%~exist([fullfile(SUN3Dpath,sequenceName,'sfm') '_BA' num2str(5) '_all' '/BA_variables.mat'],'file')
       RGBDsfm(sequenceName, 5, [] , SUN3Dpath);
    end
    %{
    subSeqFrames = {[1:1000],[1001:2000],[2001:3000],[3001:4000],[4001:5000],[5001:6000],[6001:7000]};
    SUN3Dpath = '/n/fs/sun3d/robot_in_a_room/data/';
    %%
    sequenceName = ['/Jan22_data/EDBF/device' num2str(diviceId((id)))];
    RGBDsfmSub(sequenceName,subSeqFrames,SUN3Dpath)
    %}
    
    %{
        [s,f] = ndgrid(1:length(diviceId),1:length(subSeqFrames));
        frameIDs = subSeqFrames{f(id)};
        sequenceName = ['/Jan22_data/EDBF/device' num2str(diviceId(s(id)))];
        if ~exist([fullfile(SUN3Dpath,sequenceName,'sfm') '_BA' num2str(5) '_' num2str(frameIDs(1)) '_' num2str(frameIDs(end)) '/BA_variables.mat'],'file')
            display([fullfile(SUN3Dpath,sequenceName,'sfm') '_BA' num2str(5) '_' num2str(frameIDs(1)) '_' num2str(frameIDs(end)) '/BA_variables.mat'])
            RGBDsfm(sequenceName, 5, frameIDs , SUN3Dpath);
        end
    %}
end


%{
function RGBDsfmScript(i)
    load('/n/fs/sun3d/SUN3Dv2_todo.mat');
    if ~exist(fullfile('/n/fs/sun3d/sfm',SUN3D{i}),'dir') || ~exist(fullfile('/n/fs/sun3d/data',SUN3D{i},'thumbnail'),'dir')
        try
            disp(i);
            [~,hname] = system('hostname');
            fprintf('RGBDsfmScript(%d: %s) at %s\n',i,SUN3D{i},hname);

            if matlabpool('size') ==0
                try
                    matlabpool(6);
                catch
                end
            end
            % do something
            RGBDsfm(SUN3D{i});

            fprintf('%d done\n',i);
        catch
            fprintf('%d error\n',i);
        end
    else
        fprintf('%d exists\n',i);
    end
end
%}