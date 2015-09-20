% ssh ionic.cs.princeton.edu
% cd /n/fs/sun3d/code/SUN3Dsfm/
% /n/fs/vision/ionic/starter.sh RotateSceneScript 3000mb 5:00:00 1 415 1 /n/fs/sun3d/code/ionic_log/

% to check jobs: showq
% qstat
% to kill jobs: qdel

function RotateSceneScript(i)
    load('/n/fs/sun3d/SUN3Dv1.mat');
    if exist(fullfile('/n/fs/sun3d/sfm',SUN3D{i}),'dir')
        try
            disp(i);
            [~,hname] = system('hostname');
            fprintf('RotateSceneScript(%d: %s) at %s\n',i,SUN3D{i},hname);

            % do something
            RotateScene(SUN3D{i});

            fprintf('%d done\n',i);
        catch
            fprintf('%d error\n',i);
        end
    else
        fprintf('%d exists\n',i);
    end
end
