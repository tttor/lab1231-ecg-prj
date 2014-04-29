function [ mat_ecg_out, order, n_item, c_idx ] = reorder_beat ( mat_ecg_in, clust_num )
% Reorder beat in 2D ECG matrix
% 1. Fuzzy c-mean clustering
% 2. Reorder each heartbeat by its distance to centroid
    
    [IDX,C] = fcm_cluster(mat_ecg_in,clust_num); % n: cluster number
    c_idx   = [];
    order   = [];
    for k = 1:clust_num
        clust_idx  = find(IDX == k);
        clust_size = size(clust_idx,1);
        n_item(k)  = clust_size;  
        clust_mat  = zeros(clust_size,2);
        for l = 1:clust_size
            cent_dist = sqrt(sum((mat_ecg_in(clust_idx(l),:) - C(k,:)) .^ 2));
            clust_mat(l,1) = clust_idx(l);
            clust_mat(l,2) = cent_dist;
        end
        clust_mat = sortrows(clust_mat,2);
        order = [order; clust_mat(:,1)];
        c_idx = [c_idx; clust_mat(:,1)];
        
    end
    mat_ecg_out = mat_ecg_in(order(:),:);
    order = order';
end