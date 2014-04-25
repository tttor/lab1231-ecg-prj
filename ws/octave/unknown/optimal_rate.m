function optimal_rate( ecg_in, rr_ann_in, beat_num )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    
    first_beat = rr_ann_in(1,2);
    % Ignore signal start from first sample to the first beat
    [ecg_frm, rr] = segment_beat( ecg_in,rr_ann_in(2:end,:),beat_num);
    ecg_out = [];
    frm_num = size(ecg_frm,1);
    
    for k = 1:frm_num
        k
        ecg_k = ecg_frm(k,:);
        rr_k  = rr(k,:,:);
        rr_k  = reshape(rr_k,2,beat_num);
        ecg_scl = scale_beat(ecg_k,rr_k,beat_num);
        ecg_uns = unscale_beat(ecg_scl,rr_k,beat_num);
        ecg_out = [ecg_out ecg_uns];
    end
    
    n = size(ecg_out,2);
    t = 1:n;
    plot(t,ecg_in(first_beat:n+first_beat-1),t,ecg_out);
    prd(ecg_in(first_beat:n+first_beat-1),ecg_out,1)
end