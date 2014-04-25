function [ ecg_out ] = lowpass_all( ecg_in )
%   UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    
    lead_num = size(ecg_in,1);
    ecg_out  = zeros(size(ecg_in));
    for k = 1:lead_num
        ecg_out(k,:) = lowpass(ecg_in(k,:));
    end 
end
