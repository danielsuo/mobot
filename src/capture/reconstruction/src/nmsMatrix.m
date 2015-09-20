function scoreNMS = nmsMatrix(score,radius)
scoreNMS = zeros(size(score));
for i =1:size(score,2)
    pickArray = nmsMatrixOneD(score(:,i),radius);
    scoreNMS((i-1)*size(score,1)+pickArray) = score((i-1)*size(score,1)+pickArray);
    %plot(score(:,i));hold on;plot(pickArray,score((i-1)*size(score,1)+pickArray),'xr');plot(scoreNMS(:,i),'k')
end
scoreNMS = sparse(scoreNMS);
end
function  pickArray = nmsMatrixOneD(scoreArray,radius)
        [~,ind]=sort(scoreArray);
        pickArray = zeros(1,length(scoreArray));
        cnt = 1;
        while sum(scoreArray>0)
            pickInd = ind(end);
            pickArray(cnt) = pickInd;
            scoreArray([max(pickInd-radius,1):min(length(scoreArray),pickInd+radius)]) = 0;
            ind(ind<(pickInd+radius+1)&ind>(pickInd-radius-1)) = [];
            cnt = cnt +1;
        end
        pickArray = pickArray(1:cnt-1);
end