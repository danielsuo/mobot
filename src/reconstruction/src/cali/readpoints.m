function Frame =readpoints(data,pickframeId,flip)
        im =  im2double(imread(data.image{pickframeId}));
        depth = imread(data.depth{pickframeId});
        depth = bitor(bitshift(depth,-3), bitshift(depth,16-3));
        depth = single(depth)/1000; 
        if flip
            depth = depth(:,end:-1:1,:);
            im = im(:,end:-1:1,:);
        end
        
        try
            K = data.camera.D.KK;
        catch
            try 
                K = data.camera.RGB.KK;
            catch 
                K = data.K;
            end
        end
        [x,y] = meshgrid(1:size(depth,2), 1:size(depth,1));
        XYZcamera(:,:,1) = (x-K(1,3)).*depth/K(1,1);
        XYZcamera(:,:,2) = (y-K(2,3)).*depth/K(2,2);
        XYZcamera(:,:,3) = depth;
        XYZcamera(:,:,4) = depth~=0;
        if size(depth,1)~=size(im,1)
            RT = data.camera.D2RGB.Rt;%[eye(3),-1*data.camera.D2RGB.T];
            [~,depth] = WarpDepthMatlab(XYZcamera,data.camera.RGB.KK, RT, size(im,2), size(im,1));
            [x,y] = meshgrid(1:size(depth,2), 1:size(depth,1));
            XYZcamerawarp(:,:,1) = (x-data.camera.RGB.KK(1,3)).*depth/data.camera.RGB.KK(1,1);
            XYZcamerawarp(:,:,2) = (y-data.camera.RGB.KK(2,3)).*depth/data.camera.RGB.KK(2,2);
            XYZcamerawarp(:,:,3) = depth;
        else
           XYZcamerawarp = XYZcamera(:,:,1:3);
        end
        
        Frame = cat(3,im,XYZcamerawarp(:,:,[3,1,2]));

end