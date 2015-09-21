% Usage:   [Rt, inliers] = ransacfitRtScale(x1, x2, t)
%
% Arguments:
%          x1  - 3xN set of 3D points.
%          x2  - 3xN set of 3D points such that x1<->x2.
%          t   - The distance threshold between data point and the model
%                used to decide whether a point is an inlier or not.
%
% Note that it is assumed that the matching of x1 and x2 are putative and it
% is expected that a percentage of matches will be wrong.
%
% Returns:
%          Rt      - The 3x4 transformation matrix such that x1 = R*x2 + t.
%          inliers - An array of indices of the elements of x1, x2 that were
%                    the inliers for the best model.
%
% See Also: RANSAC
% Author: Jianxiong Xiao

function [Rt, Scale,inliers] = ransacfitRtScale(x, t, feedback)
    s = 3;  % Number of points needed to fit a Rt matrix.
    
    if size(x,2)==s
        inliers = 1:s;
        RtScale = estimateRtScale(x);
        Rt = RtScale(1:3,:);
        Scale = RtScale(4,4);
        return;
    end

    fittingfn = @estimateRtScale;
    distfn    = @euc3Ddist;
    degenfn   = @isdegenerate;
    % x1 and x2 are 'stacked' to create a 6xN array for ransac
    [RtScale,inliers] = ransac(x, fittingfn, distfn, degenfn, s, t, feedback);
    
    if length(inliers)<s
        Rt = [eye(3) zeros(3,1)];
        Scale = 1;
        inliers = [];
        return;
    end
    
    % Now do a final least squares fit on the data points considered to
    % be inliers.
    [RtScale] = estimateRtScale(x(:,inliers));
    Rt = RtScale(1:3,:);
    Scale = RtScale(4,4);
    
end

function [RtScale] = estimateRtScale(x, npts)
    pointset1 = x(1:3,:);
    pointset2 = x(4:6,:);
    n = floor(length(pointset2)/2);
    a = sqrt(sum((pointset1(:,1:2:2*n-1) - pointset1(:,2:2:2*n)).^2));
    b = sqrt(sum((pointset2(:,1:2:2*n-1) - pointset2(:,2:2:2*n)).^2));
    allscale = a./b;
    Scale = nanmean(allscale(allscale>0&allscale<5));

%     area1 = tri_area(x(1:3,1),x(1:3,2),x(1:3,3));
%     area2 = tri_area(x(4:6,1),x(4:6,2),x(4:6,3));
%     Scale = sqrt(area1/area2);
    [T, Eps] = estimateRigidTransform(x(1:3,:), x(4:6,:));
    Rt = T(1:3,:);
    RtScale = zeros(4);
    RtScale(1:3,:) = Rt;
    RtScale(4,4) = Scale;
end

%--------------------------------------------------------------------------
% Note that this code allows for Rt being a cell array of matrices of
% which we have to pick the best one.

function [bestInliers, bestRt] = euc3Ddist(Rt, x, t)
    if iscell(Rt)  % We have several solutions each of which must be tested
        nRt = length(Rt);   % Number of solutions to test
        bestRt = Rt{1};     % Initial allocation of best solution
        ninliers = 0;     % Number of inliers
        for k = 1:nRt
            d =  sum((x(1:3,:) - (Rt{k}(:,1:3)*x(4:6,:)+repmat(Rt{k}(:,4),1,size(x,2)))).^2,1).^0.5;
            inliers = find(abs(d) < t);     % Indices of inlying points
            if length(inliers) > ninliers   % Record best solution
                ninliers = length(inliers);
                bestRt = Rt{k};
                bestInliers = inliers;
            end
        end

    else     % We just have one solution
        Scale = Rt(4,4);
        d =  sum((x(1:3,:) - (Rt(1:3,1:3)*Scale*x(4:6,:)+repmat(Rt(1:3,4),1,size(x,2)))).^2,1).^0.5;
        bestInliers = find(abs(d) < t);     % Indices of inlying points
        bestRt = Rt;                        % Copy Rt directly to bestRt
    end
end

%----------------------------------------------------------------------
% (Degenerate!) function to determine if a set of matched points will result
% in a degeneracy in the calculation of a fundamental matrix as needed by
% RANSAC.  This function assumes this cannot happen...

function r = isdegenerate(x)
    r = 0;
end
