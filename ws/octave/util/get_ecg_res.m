function [ wv_coef, ecg_mat_res ] = get_ecg_res( ecg_mat, n_item, c_idx )
% Calculate ECG model for each cluster of beats using wavelet approach

    wv_name   = 'bior3.1';
    wv_lvl    = 4;
    clust_num = size(n_item,2);
    ecg_mat_res = [];
    j = 0; % Non-empty cluster number
    start_idx = 1;
    for k = 1:clust_num
        ecg_mat_res_1 = [];
        if n_item(k) > 0
            j = j + 1;
            end_idx   = start_idx + n_item(k) - 1;
            beat_idx  = start_idx:end_idx;
            start_idx = end_idx + 1;
            ecg_clust = ecg_mat(c_idx(beat_idx),:);
            ecg_mean(j,:) = mean(ecg_clust,1);
            [c,l]     = wavedec(ecg_mean(j,:),wv_lvl,wv_name);
            c(21:end) = 0; % Use only first 20 wavelet coefficients
            ecg_mod(j,:)  = waverec(c,l,wv_name);
            wv_coef(j,:)  = c;
            %hold on;
            for l = 1:size(ecg_clust,1)
                for m = 1:size(ecg_clust,2)
                    ecg_mat_res_1(l,m) = ecg_clust(l,m) - ecg_mod(j,m);
                end
                %plot(ecg_clust(l,:),'b');
                %plot(ecg_mat_res_1(l,:),'g');
            end
            %plot(ecg_mod(j,:),'r');
            %hold off;
            ecg_mat_res = [ecg_mat_res; ecg_mat_res_1];
        end
    end
end

