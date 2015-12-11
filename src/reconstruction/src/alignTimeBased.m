function [MatchPairs, cameraRtC2W] = alignTimeBased(data)

MatchPairs = cell(1,length(data.image)-1);

% Loop through all consecutive frames and find relative poses
parfor frameID = 1:length(data.image)-1
    MatchPairs{frameID} = align2view(data, frameID, frameID+1);
end

% Naive approach: just put all results together. Accumulate Rt across
% frames
cameraRtC2W = repmat([eye(3) zeros(3, 1)], [1, 1, length(data.image)]);

for frameID = 1:length(data.image)-1
    cameraRtC2W(:, :, frameID+1) = [cameraRtC2W(:, 1:3, frameID) * MatchPairs{frameID}.Rt(:, 1:3) ...
                                    cameraRtC2W(:,1:3,frameID) * MatchPairs{frameID}.Rt(:, 4) + cameraRtC2W(:, 4, frameID)];
end

end