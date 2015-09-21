function rgbImg = showoverImage(rgbImg,depthImg)
%          depthImg(depthImg>5) =5;
%          rgbImg(:,:,1) = ((depthImg/5));
         rgbImg = double(rgbImg)/255;
         rgbImg(:,:,1) = double(depthImg)/max(double(depthImg(:)));
end