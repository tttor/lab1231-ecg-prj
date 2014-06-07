function [ecg8_pre,rr_ann,baseline] = run_preprocess(rec_name, rec_dir, out_dir)
    fprintf('\nPREPROCESSING =========================================\n');
    
    % Load readings
    load( strcat(rec_dir,rec_name{1}) );
    ecg_in = val;
    
    rr_ann = load_rr_ann(rec_dir,rec_name{1});% for now, consider only the first rec_name

    ecg8 = select_lead(ecg_in);% Select lead I, II, V1..V6 as input signal
    base = 0;
    gain = 306;
    
    % Physical unit conversion, BWR, LPF
    [ecg8_pre,rr_ann,baseline] = preprocess(ecg8,rr_ann,base,gain);
    
    csvwrite(strcat(out_dir,'preprocessed/ecg8_pre_matlab.csv'), ecg8_pre');
    csvwrite(strcat(out_dir,'preprocessed/rr_ann_matlab.csv'), rr_ann');
end

