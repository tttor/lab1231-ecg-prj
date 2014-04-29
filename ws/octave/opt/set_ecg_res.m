function [ ecg_mat ] = set_ecg_res( wv_coef, ecg_mat_res, n_item, c_idx, sample_num )
% Restore the amplitudo of each residual ECG signal

    wv_name = 'bior3.1';
    wv_lvl  = 4;
    dummy   = 1:sample_num;
    [C,L]   = wavedec(dummy,wv_lvl,wv_name);
    clust_num = size(n_item,2);
    ecg_mat = [];
    j = 0; % Non-empty cluster number
    n = 0;
    start_idx = 1;
    for k = 1:clust_num
        ecg_mat_1 = [];        
        if n_item(k) > 0
            j = j + 1;
            ecg_model(j,:) = waverec(wv_coef(j,:),L,wv_name);
            end_idx   = start_idx + n_item(k) - 1;
            beat_idx  = start_idx:end_idx;
            start_idx = end_idx + 1;
            for l = 1:size(beat_idx,2)
                n = n + 1;
                for m = 1:size(ecg_mat_res,2)
                    ecg_mat_1(l,m) = ecg_mat_res(n,m) + ecg_model(j,m);
                end
            end
            ecg_mat = [ecg_mat; ecg_mat_1];
        end
    end
end

