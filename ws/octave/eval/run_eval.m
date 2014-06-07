function PRD = run_eval(out_dir, rec_dir, rec_name)
    fprintf('\nRUNNING EVALUATION =========================================\n');
    
    % Reconstruct
    bit_str_dir = strcat(out_dir, 'bit-str/');
    n_frame = csvread( strcat(out_dir, 'param/n_frame') );
    n_sample = csvread( strcat(out_dir, 'param/n_sample') );
    n_ecg1_col = csvread( strcat(out_dir, 'param/n_ecg1_col') );
    rr_ann = csvread( strcat(out_dir, 'preprocessed/rr_ann_matlab.csv') );
    
    recg8_pre = decode(out_dir, rr_ann, n_frame, n_sample, n_ecg1_col);
    csvwrite(strcat(out_dir,'eval/recg8_pre.csv'), recg8_pre);

    %% Calculate PRD
    ecg8_pre = csvread( strcat(out_dir, 'preprocessed/ecg8_pre_matlab.csv') );
    normalize = 1;
    PRD = get_PRD_all(ecg8_pre', recg8_pre, normalize);
    csvwrite(strcat(out_dir,'eval/PRD'), PRD);
    
    % Plot
    duration = 10;
    plot_cmpr(ecg8_pre', recg8_pre, duration, out_dir);
end
