function recg8_pre = decode_2(bit_str, S, rr_ann, n_frame, n_sample, n_ecg1_col)
    % 2D SPIHT decoding
    fprintf('\n2D SPIHT decoding       : ');
    
    n_beat = n_sample;
    recg_mat_res = cell(n_frame,n_sample,n_beat);
    
    percent = 0;
    ten_percent = floor(n_frame/10);    
    for k = 1:n_frame
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        
        dec_lvl = 7;
        wave_name = 'bior4.4';
        
        local_recg_mat_res = SPIHT_dec(bit_str{k}, S, wave_name, dec_lvl, n_sample, n_beat);
        recg_mat_res{k} = local_recg_mat_res;
    end
    fprintf(' [OK]\n');
    ecg = [];
    
    % 2D ECG to ECG frames matrix conversion
    recg1_frm = [];
    for k = 1:n_frame
        recg1_frm = [recg1_frm; mat2ecg(recg_mat_res{k}, n_sample, n_beat)];
    end

    % Unsegment ECG frames into single lead ECG
    recg1 = unsegment_beat(recg1_frm, n_ecg1_col);
    
    % Single to multi lead ECG conversion
    group = 0;
    rr_ann = rr_ann';
    recg8_scl = single2multi(recg1, rr_ann, n_sample, group);
    
    % Beat denormalization
    recg8_pre  = unscale_beat_all(recg8_scl, rr_ann, n_sample);
end
