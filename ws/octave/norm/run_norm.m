function ecg8_norm = run_norm(ecg8_pre, rr_ann, sample_num, out_dir)
    fprintf('\nNORMALIZATION =========================================\n');
    
    ecg8_norm = scale_beat_all(ecg8_pre,rr_ann,sample_num);
    csvwrite(strcat(out_dir,'normalized/ecg8_norm_matlab.csv'), ecg8_norm');
end
