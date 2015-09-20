function  cameraRtC2W= transformcameraRtC2Wscale(cameraRtC2W,Scale)
        for i =1:size(cameraRtC2W,3)
            cameraRtC2W(:,4,i) = Scale*cameraRtC2W(:,4,i);
        end
end