function [ ecg_out, base ] = bwr_all( ecg_in, wv_name, N )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here
    lead_num = size(ecg_in,1);
    for k = 1:lead_num
        [ecg_out(k,:), base(k,:)] = bwr_wavelet(ecg_in(k,:), wv_name, N);
    end
end

