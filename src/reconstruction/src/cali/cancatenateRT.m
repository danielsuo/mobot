function RT =cancatenateRT(cameraRtC2W,isinverse)
RT = [eye(3),zeros(3,1)];
if ~isinverse
   
   for i =1:size(cameraRtC2W,3)
       RT = cameraRtC2W(1:3,1:3,i)*RT;
       RT(:,4) = RT(:,4)+cameraRtC2W(:,4,i);
   end
else
   for i =1:size(cameraRtC2W,3)
       RT(:,4) = RT(:,4)-cameraRtC2W(:,4,i);
       RT = cameraRtC2W(1:3,1:3,i)'*RT;
   end
end
end