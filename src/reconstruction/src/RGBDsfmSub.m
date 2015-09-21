%cd /n/fs/sun3d/robot_in_a_room/code/RGBDsfm
function RGBDsfmSub(sequenceName,subSeqFrames,SUN3Dpath)
% subSeqFrames = {[1:900],[900:2000],[2000:3000],[3000:4000],[4000:5000],[5000:6000]};
% deviceId = 0; 
% sequenceName = [ '/DEAB/device0' num2str(deviceId)];
% SUN3Dpath = '/n/fs/sun3d/robot_in_a_room/Jan12_data/';

basicSetup;
data = loadSUN3D(sequenceName,[],SUN3Dpath);

% get extrinsicsC2W{}
for seqid = 1:length(subSeqFrames)
    frameIDs = subSeqFrames{seqid};

    tmp = load([fullfile(SUN3Dpath,sequenceName,'sfm') ...
            '_BA' num2str(5) '_' num2str(frameIDs(1)) '_' num2str(frameIDs(end)) '/BA_variables.mat'],'cameraRtC2W');
    extrinsicsC2W{seqid}=tmp.cameraRtC2W;
end

% get match
figure,
BOWmodel = visualindex_build(data.image, ...
            1:length(data.image), false, 'numWords', 4000) ;

scores = BOWmodel.index.histograms' * BOWmodel.index.histograms;
% for each seq pair find R anf T
RtRANSAC{1} = [eye(3),zeros(3,1)];
for seqid = 2:length(subSeqFrames)
    % pick scores that matters to this two  sequence
    M = zeros(length(data.image));
    M(subSeqFrames{seqid-1},subSeqFrames{seqid})=1;
    scoresPair = scores.*M;
    scoresPairNMS = nonmaxsup(scoresPair, 7);
    ind = find(scoresPairNMS(:)>0.23); % threshold 
    [cameras_i, cameras_j] = ind2sub([length(data.image) length(data.image)],ind);

    % 
    correspondence =[];
    for i =1:length(cameras_i)
        c_ref = min(cameras_i(i),cameras_j(i));
        c_d = max(cameras_i(i),cameras_j(i));
        %sc = full(score_sort(i));
        %matchFrame = [matchFrame;c_ref,mod(c_d,calibrateVideoLength)+1,sc];
        MatchPairs = align2view(data,c_ref ,c_d);
        if length(MatchPairs.matches)>100
            pt1 = MatchPairs.matches(3:5,:);
            pt2 = MatchPairs.matches(8:10,:);
            pt1 = transformRT(pt1,extrinsicsC2W{seqid-1}(:,:,c_ref-subSeqFrames{seqid-1}(1)+1));
            pt2 = transformRT(pt2,extrinsicsC2W{seqid}(:,:,c_d-subSeqFrames{seqid}(1)+1));
            correspondence = [correspondence, [pt1;pt2]];
        end
    end
    error3D_threshold =0.05;
    [RtRANSAC{seqid},inliers] = ransacfitRt(correspondence, error3D_threshold, 0);

end

%% get golbel Rt
RtRANSACTofirst{1} = RtRANSAC{1};
for i =2:length(subSeqFrames)
    RtRANSACTofirst{i} = transformcameraRtC2W(RtRANSAC{i},RtRANSACTofirst{i-1},0); 
end
% output ply
clear cameraRtC2WAll cameraRtC2WAlign
for seqid = 1:length(subSeqFrames)
    %dataSub = loadSUN3D(sequenceName,subSeqFrames{seqid},SUN3Dpath);
    cameraRtC2WAlign{seqid}= transformcameraRtC2W(extrinsicsC2W{seqid},RtRANSACTofirst{seqid},0); % inverse or not ? 
    cameraRtC2WAll(:,:,subSeqFrames{seqid}) = cameraRtC2WAlign{seqid};
    %outputPly(fullfile(SUN3Dpath,sequenceName,['sub_' num2str(seqid) '.ply']), cameraRtC2WAlign{seqid}, dataSub,1)
end
dataSub = data;
dataSub.image = data.image(1:size(cameraRtC2WAll,3));
dataSub.depth = data.depth(1:size(cameraRtC2WAll,3));
outputPly(fullfile(SUN3Dpath,sequenceName,'all.ply'), cameraRtC2WAll, dataSub,1)
timeStamp = getTimeStamp;
outputCameraExtrinsics(SUN3Dpath, sequenceName, cameraRtC2WAll, [timeStamp '_beforerect'] );
