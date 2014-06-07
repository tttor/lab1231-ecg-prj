function pipeline(online)
    out_dir = './main/out/';
    rec_dir = '../../dataset/incartdb/';
    rec_name = {'I01m'};
    
    %% Main processes
    if online == 1
        %
        [ecg8_pre,rr_ann,baseline] = run_preprocess(rec_name, rec_dir, out_dir);
        
        %
        n_sample = 128;
        ecg8_norm = run_norm(ecg8_pre,rr_ann,n_sample, out_dir);
        
        %
        [ecg_mat, n_ecg1_col, n_frame] = run_array_const(ecg8_norm, rr_ann, n_sample, out_dir);
        
        %
        CR = 4;
        res = 12;
        wavelet_img = run_wavelet_transform(ecg_mat, CR, res, n_frame, out_dir);
        
        %
        bit_str = run_spiht(wavelet_img, CR, res, n_frame, out_dir);
        
        % Save encoding param
        csvwrite(strcat(out_dir,'param/n_sample'), n_sample);
        csvwrite(strcat(out_dir,'param/n_frame'), n_frame);
        csvwrite(strcat(out_dir,'param/n_ecg1_col'), n_ecg1_col);
        csvwrite(strcat(out_dir,'param/CR'), CR);
        csvwrite(strcat(out_dir,'param/res'), res);
    end
    
    %% Evaluation
    prd = run_eval(out_dir, rec_dir, rec_name);
end
