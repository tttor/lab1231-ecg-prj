function [ ecg_out ] = preprocess_sig( ecg_in, base, gain )
% 1. Convert ecg from raw units to the physical unit
% 2. Baseline wander removal
% 3. Low pass filter
% 4. Set rr interval to support last beat

    lead_num = 8;    
    for l = 1:8
        ecg_1    = ecg_in(l,:);
        ecg_base = ecg_1 - base;
        ecg_gain = ecg_base/gain;
        [ecg_bwr] = bwr(ecg_gain,'bior4.4',16,0);
        ecg_out(l,:)  = lowpass(ecg_bwr,0);
    end   
end