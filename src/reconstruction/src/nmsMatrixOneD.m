% Find the maximum element in an array and set all elements within radius
% indices to 0. Repeat until all elements except maximums are equal to
% zero.
function pickArray = nmsMatrixOneD(scoreArray, radius)

    % Get sort indices of the score column
    [~, ind] = sort(scoreArray);
    
    % Create result array
    pickArray = zeros(1, length(scoreArray));
    
    % Count number of maxima we find
    cnt = 1;
    
    % Loop over score array
    while sum(scoreArray > 0)
        
        % Find index of current maximum
        pickInd = ind(end);
        
        % Store the index of the maximum
        pickArray(cnt) = pickInd;
        
        % Zero out any part of the array that isn't zero
        scoreArray([max(pickInd - radius, 1):min(length(scoreArray), pickInd + radius)]) = 0;
        
        % Remove corresponding indices that are now point to zeros
        ind(ind < (pickInd + radius + 1) & ind > (pickInd - radius - 1)) = [];
        
        % Increment number of maxima found
        cnt = cnt +1;
    end
    
    % Truncate to the number of maxima found
    pickArray = pickArray(1:cnt-1);
end