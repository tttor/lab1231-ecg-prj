function [ IDX, C ] = fcm_cluster( data_in, n )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    [C, U] = fcm(data_in, n,[3 250 1e-5 0]);
    max_U = max(U);
    IDX = [];
    for k = 1:n
        index = find(U(k,:) == max_U)';
        clust_id = zeros(size(index));
        clust_id(:) = k;
        index = [clust_id index];
        IDX      = [IDX; index];
    end
    IDX = sortrows(IDX,2);
    IDX = IDX(:,1);
end

