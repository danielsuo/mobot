function [matchPointsID_i, matchPointsID_j] = matchSIFTdesImagesBidirectional(X_i, X_j, distRatio)

if ~exist('distRatio','var')
    distRatio = 0.6^2;
end

% Convert all descriptors into floats
X_i = single(X_i);
X_j = single(X_j);

% Build kd-trees from SIFT descriptors in ith and jth frames
kdtree_j = vl_kdtreebuild(X_j);
kdtree_i = vl_kdtreebuild(X_i);

% Initialize data structures
numi = size(X_i,2);
matchPointsID_j = zeros(1,numi);

% Loop through all of the key points in the ith frame
for i = 1 : numi
    
    % Find the two nearest neighbors in jth frame to sift point i in ith
    % frame (should be k to avoid confusion, but whatever). idx is the
    % index in X_j where the min occurs and val is the Euclidean squared
    % distance
    [min_idx_j, min_val_j] = vl_kdtreequery(kdtree_j, X_j, X_i(:,i), 'NumNeighbors', 2);
    
    % Check to make sure the distance of the closest neighbor is at least
    % distRatio closer than the second to prevent false positive
    if (min_val_j(1) < distRatio * min_val_j(2))
        matchPointsID_j(i) = min_idx_j(1);
%{
        % Find the two nearest neighbors in the ith frame to the closest
        % neighbor in jth frame we just found
        [min_idx_i, min_val_i] = vl_kdtreequery(kdtree_i, X_i, X_j(:,min_idx_j(1)), 'NumNeighbors', 2);
        
        % Check to make sure closest neighbor from i to j is the same as
        % from j to i and that the closest neighbor is at least distRatio
        % closer than the second to prevent false positives
        if min_idx_i(1) == i && min_val_i(1) < distRatio * min_val_i(2)
            
            % Record the matches
            matchPointsID_j(i) = min_idx_j(1);
        end
%}
    end
end

% Valid matches are where matchPointsID_j are not 0
valid = (matchPointsID_j~=0);

% Return the indices of matching points from both ith and jth frames
pointsID_i = 1:numi;
matchPointsID_i = pointsID_i(valid);
matchPointsID_j = matchPointsID_j(valid);


