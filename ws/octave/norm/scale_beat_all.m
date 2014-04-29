function [ ecg_out ] = scale_beat_all( ecg_in, r_ann, sample_num )
% Normalize number of sample per hearbeat on all lead of multilead ECG signal
    lead_num     = size(ecg_in,1);
    fprintf('\n');
    for k = 1:lead_num
        ecg_out(k,:) = scale_beat(ecg_in(k,:),r_ann,sample_num);
        fprintf('Beat Scaling lead-%i     : [OK]\n\n',k);
    end
end