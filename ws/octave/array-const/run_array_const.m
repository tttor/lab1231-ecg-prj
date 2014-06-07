function [ecg_mat, n_ecg1_col, n_frame] = run_array_const(ecg8_norm, rr_ann, sample_num, out_dir)
    fprintf('\nARRAY CONST =========================================\n');
    
    % Multi lead to single lead ECG conversion
    group = 0;
    ecg1 = multi2single(ecg8_norm,rr_ann,sample_num,group);
    n_ecg1_col = size(ecg1, 2);% TODO what is ecg1?
    
    % Single lead ECG segmentation into frames
    beat_num = sample_num;
    ecg1_frm = segment_beat(ecg1,beat_num);
    
    % ECG frames to 2D ECG matrix conversion
    n_frame = size(ecg1_frm,1);
    ecg_mat = cell(n_frame,sample_num,beat_num);
    for k = 1:n_frame
        ecg_mat{k} = {ecg2mat(ecg1_frm(k,:),sample_num,beat_num)};
    end
    
    % Save
    for k = 1:n_frame
        filename = strcat(out_dir, 'array-const/array-', int2str(k), '.csv');
        M = ecg_mat{k};
        
        csvwrite(filename, M);
    end
end

