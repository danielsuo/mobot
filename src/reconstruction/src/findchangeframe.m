function pickids = findchangeframe(image,thre,method)
% calculate gist feature 
if ~exist('thre','var')
    thre = 5;
end
if ~exist('method','var')
    method = 'raw';
end

param.imageSize = [256 256]; % it works also with non-square images
param.orientationsPerScale = [8 8 8 8];
param.numberBlocks = 4;
param.fc_prefilt = 4;


if strcmp(method,'gist')
    fea = zeros(32,length(image));
    for i =1:length(image)
        fea(:,i) =  LMgist(imread(image{i}), '', param);
    end
elseif strcmp(method,'raw')
    fea = zeros(76800,length(image));
    parfor i =1:length(image)
        im = (imread(image{i}));
        im = im(1:2:end,1:2:end,:);
        im = rgb2gray(im);
        fea(:,i) = im(:);
    end
end

% caculate the distance from this fram to previous frame
diff = fea(:,2:end) - fea(:,1:end-1);
diff = sqrt(sum(diff.*diff)/size(fea,1));
similar = [0,diff<thre];
%similar_closes = imopen(similar,ones(1,10));
pickids = ~similar;
%{
for i =1:length(similar)
    if similar(i)
        imshow(image{i});
        title(num2str(i))
        pause(0.05)
        
    end
end
%}
end