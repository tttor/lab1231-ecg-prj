function [ ecg_out, rr_ann_out, baseline ] = preprocess( ecg_in, rr_ann_in, base, gain )
% 1. Convert ecg from raw units to the physical unit
% 2. Baseline wander removal
% 3. Low pass filter
% 4. Set rr interval to support last beat

    lead_num = 8;    
    for l = 1:8
        fprintf('\nLead num: %i',l);
        ecg_1    = ecg_in(l,:);
        ecg_base = ecg_1 - base;
        ecg_gain = ecg_base/gain;
        [ecg_bwr, baseline_1] = bwr(ecg_gain,'bior4.4',16,1);
        ecg_out(l,:)  = lowpass(ecg_bwr,1);
        baseline(l,:) = baseline_1;
    end
    
    rr_ann_in = rr_ann_in';
    
    % Swap rows
    temp = rr_ann_in(1,:);       
    rr_ann_in(1,:) = rr_ann_in(2,:);
    rr_ann_in(2,:) = temp;
    
    last_beat_dur = size(ecg_in,2) - rr_ann_in(2,end) - rr_ann_in(1,end);
    last_beat_idx = size(rr_ann_in,2) + 1;
    rr_ann_in(1,last_beat_idx) = last_beat_dur;
    rr_ann_in(2,last_beat_idx) = size(ecg_in,2) - last_beat_dur;
    
    rr_ann_out = rr_ann_in(:,2:end);    
end