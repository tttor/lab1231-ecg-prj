function preprocess_2 (ecg_in, RR_ANN_IN, sample_num, CR )

%tic;
    
    % Correlation analysis result shows that the most correlated leads are
    % (sorted descending):
    %
    % lead 4-5 (V2-V3)
    % lead 5-6 (V3-V4)
    % lead 3-7 (V1-V5)
    % lead 4-6 (V2-V4)
    % lead 1-7 (I-V5)
    % lead 7-8 (V5-V6)
    % lead 2-3 (II-V1)
    %
    % Following lead arrangement could give better compression performance
    % group-1 : lead 1,7,8
    % group-2 : lead 2,3 (inv)
    % group-3 : lead 4,5,6
    %
    % Original ECG signal bitrate = 12 bits/sample
    % Number of samples on original ECG signal   : 462.600
    % Number of samples on normalized ECG Signal : 706.048
    % Normalized ECG signal bitrate 12 * 462.600/706048 = 7.86 bits/sample 
    % -> 7 bits/sample
    %     CR      8      16     24      32      
    % bitrate = [0.875 0.4375 0.2917 0.21875];    
    
    lead_num       = 8;
    beat_num       = sample_num; % number of beat per frame on normalized ECG
    tot_sample_num = 46260;
    %tot_sample_num = 2570;
    resolution     = 12;
    max_bits       = tot_sample_num*lead_num*resolution/CR;
    wv_name        = 'bior4.4';
    dec_lvl        = 7;
       
    % ====================================================================
    % PREPROCESSING
    % ====================================================================
    
    
    fprintf('\nPREPROCESSING =========================================\n');
    base = 0;
    gain = 306;
    
    % Select lead I, II, V1..V6 as input signal
    ecg8 = select_lead(ecg_in);
    
    % Physical unit conversion, BWR, LPF
    [ecg8_pre,rr_ann,baseline] = preprocess(ecg8,RR_ANN_IN,base,gain);
    
    csvwrite('ecg8_pre_matlab.csv',ecg8_pre');
    csvwrite('rr_ann_matlab.csv',rr_ann');
  
    
end