function [ ecg_out ] = beat_scaling_all( ecg_in, r_ann )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
    num_per_beat = 256;
    lead_num     = size(ecg_in,1);
    %ecg_out  = zeros(size(ecg_in));
    for k = 1:lead_num
        ecg_out(k,:) = scale_beat(ecg_in(k,:),r_ann,num_per_beat);
        fprintf('Beat Scaling lead-%i      : [OK]\n\n',k);
    end
end
