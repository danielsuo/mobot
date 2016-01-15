
    write2path = '/n/fs/sun3d/robot_in_a_room/data/scan002/pano_sfm/';
    load(fullfile(write2path,'sfm_single_seq.mat'),'cameraRtC2WAlign','dataArray','Calibration','cameraRtC2W');
    
    cameraRtC2W = transformcameraRtC2W(cameraRtC2WAlign{1},cameraRtC2WAlign{1}(:,:,1),1);
    cameraCenters = reshape(cameraRtC2W(:,4,:),3,[]);
    Rt_rect = PCAplanefit(cameraCenters');
    cameraRtC2W_rect = transformcameraRtC2W(cameraRtC2W,Rt_rect,0);
    cameraCenters_rect = reshape(cameraRtC2W_rect(:,4,:),3,[]);
    [axiscenter(1),axiscenter(3),R,a] = circfit(cameraCenters_rect(1,:),cameraCenters_rect(3,:));
    axiscenter(2)  = median(cameraCenters_rect(2,:));
    
    clf;
    plot3(cameraCenters_rect(1,:),cameraCenters_rect(2,:),cameraCenters_rect(3,:));
    hold on;
    plot3(axiscenter(1),axiscenter(2),axiscenter(3),'x');
    axis equal;
    
    
    Rt_rect(:,4) = -axiscenter';
    for i =1:4
        cameraRtC2W = transformcameraRtC2W(cameraRtC2WAlign{i},cameraRtC2WAlign{1}(:,:,1),1);
        cameraCenters = transformcameraRtC2W(cameraRtC2W,Rt_rect,0);
        cameraCenters =reshape(cameraCenters(:,4,:),3,[]);
        Cali_rect.RT{device_id} = cameraCenters(:,:,1);
        hold on;
        plot3(cameraCenters(1,:),cameraCenters(2,:),cameraCenters(3,:))
    end
    axis equal;
    save('/n/fs/sun3d/robot_in_a_room/data/scan002/Cali_rect.mat','Cali_rect');