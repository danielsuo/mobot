% Toggle between active and passive transformtion (i.e., transform point or
% transform coordinate system. See
% https://en.wikipedia.org/wiki/Rotation_matrix#Ambiguities)
function RtOut = transformCameraRt(RtIn)
RtOut = [RtIn(1:3,1:3)', - RtIn(1:3,1:3)'* RtIn(1:3,4)];
end
