function [RtRANSAC, Scale] = align2seq(dataSeq1,dataSeq2,findscale)
   % compute RT scale to algin dataSeq2 to dataSeq1
   % input : dataSeq1.image = {};
   %         dataSeq1.depth ={};
   %         dataSeq1.extrinsicsC2W;
   allimage = [dataSeq1.image,dataSeq2.image];
   f = figure;
   BOWmodel = visualindex_build(allimage, ...
                1:length(allimage), false, 'numWords', 4000) ;
   scores = BOWmodel.index.histograms' * BOWmodel.index.histograms;
   imagesc(scores);
   
   % pick the part we care 
   M = zeros(length(allimage));
   M (1:length(dataSeq1.image),length(dataSeq1.image)+1:length(dataSeq1.image)+length(dataSeq2.image)) = 1;
   scoresPair = scores.*M;
   scoresPairNMS = nonmaxsup(scoresPair, 7);
   ind = find(scoresPairNMS(:)>0.2); 
   [~, perm]= sort(scoresPairNMS(ind),'descend');
   ind = ind(perm);
   [cameras_i, cameras_j] = ind2sub([length(allimage) length(allimage)],ind);
   
   % find corespoundence
   correspondence = [];
   data.K = dataSeq1.K;
   for frameId = 1:length(cameras_i)     
        c_ref = min(cameras_i(frameId),cameras_j(frameId));
        c_d = max(cameras_i(frameId),cameras_j(frameId))-length(dataSeq1.image);
        data.image = {dataSeq1.image{c_ref},dataSeq2.image{c_d}};
        data.depth = {dataSeq1.depth{c_ref},dataSeq2.depth{c_d}};
       %{
                subplot(1,2,1)
        imshow(data.image{end});
        subplot(1,2,2),imshow(data.image{end-1});
        pause;
        %}
         MatchPairs = align2view(data, 1,2);
        if length(MatchPairs.matches)>20
            pt1 = MatchPairs.matches(3:5,:);
            pt2 = MatchPairs.matches(8:10,:);
            % trainform to W2 cordinate 
            pt1 = transformRT(pt1,dataSeq1.extrinsicsC2W(:,:,c_ref));
            pt2 = transformRT(pt2,dataSeq2.extrinsicsC2W(:,:,c_d));
            correspondence = [correspondence, [pt1;pt2]];
        end
        %}
   end
   error3D_threshold =0.05;
    if findscale,
        [RtRANSAC, Scale,inliers] = ransacfitRtScale(correspondence, error3D_threshold, 0);
    else
        [RtRANSAC,inliers] = ransacfitRtScale(correspondence, error3D_threshold, 0);
        Scale = 1;
    end
end