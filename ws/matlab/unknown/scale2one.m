function [ ecg_out ] = scale2one( ecg_in )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
    M = max(abs(ecg_in));
    ecg_out  = ecg_in/ max(abs(ecg_in));
end

