function [ ecg_out ] = import_ecg( ecg_in, base, gain )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
    
    ecg_in  = ecg_in-base;
    ecg_in  = ecg_in/gain;
    %ecg_out = [ecg_in(1,:); ecg_in(2,:); ecg_in(7,:); ecg_in(8,:); ecg_in(9,:); ecg_in(10,:); ecg_in(11,:); ecg_in(12,:)];
    ecg_out = [ecg_in(1,:); ecg_in(2,:)];
end

