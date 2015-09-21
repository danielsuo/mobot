function [pickid,focalsmeasure] = removeblurImage(image)
         focalsmeasure = zeros(1,length(image));
         for i = 1:length(image)
             focalsmeasure(i) = fmeasure(imread(image{i}), 'LAPM',[]);
         end
         pickid = focalsmeasure>3;
%{
         [maxx,ind] = max(focalsmeasure);
         imshow(image{ind});
         [maxx,ind] = min(focalsmeasure);
         figure
         imshow(image{ind});
figure,
for i =1:length(image)
    if focalsmeasure(i)<5
        imshow(image{i});
        title([num2str(i) ' : ' num2str(focalsmeasure(i))])
        %pause(0.05)
        drawnow;
    end
end
%}
         
end