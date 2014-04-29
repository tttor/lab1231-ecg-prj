function ecg8 = load_ori_signal( rec_name )
% Load original ecg signal, select 8 of 12 leads, and apply preprocessing to it
    
    base = 0;
    gain = 306;
    
    f_name = [rec_name '.mat'];
    ecg12_stc = load(f_name);
    ecg12     = ecg12_stc.val;
    ecg8      = select_lead(ecg12);
    ecg8      = preprocess_sig(ecg8,base,gain);
end

