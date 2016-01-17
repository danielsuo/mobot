% Find best loop closure candidates for each image via non-maximum
% suppression
function scoreNMS = nmsMatrix(score, radius)

% Create result matrix
scoreNMS = zeros(size(score));

% Iterate over columns in score matrix
for i = 1:size(score, 2)
    
    % Suppress non maximums in a given column (i.e., similarity scores to
    % other images)
    pickArray = nmsMatrixOneD(score(:, i), radius);
    
    % MATLAB is column major order, so we grab the indices specified by
    % pickArray and transfer over the scores
    scoreNMS((i - 1) * size(score, 1) + pickArray) = score((i - 1) * size(score, 1) + pickArray);
end

% Return a sparse matrix to save space
scoreNMS = sparse(scoreNMS);
end