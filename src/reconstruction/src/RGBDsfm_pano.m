function RGBDsfm_pano(data_dir, sequence_dir, device_dirs)
    basicSetup;
    
    for i = 1:4
        device_dir = device_dirs{i};
        
        % Run reconstruction pipeline on each device
        RGBDsfm(device_dir, 5, []);
        
        % run code/RGBDsfm_pano/caliCamera.m
        
        % run code/RGBDsfm_pano/RGBDpanosCali.m
    end
end