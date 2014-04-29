function [ bp_ecg ] = bandpass( pecg )
%   UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    
    N = size(pecg,2);
    ecg_norm = pecg - mean (pecg);    % cancel DC conponents
    ecg_norm = ecg_norm/ max(abs(ecg_norm));
    
    ecg_lpf = ecg_norm;
    
    % LPF (1-z^-6)^2/(1-z^-1)^2
    b=[1 0 0 0 0 0 -2 0 0 0 0 0 1];
    a=[1 -2 1];

    % transfer function of LPF
    h_LP=filter(b,a,[1 zeros(1,12)]); 
    ecg_lpf = conv(ecg_norm ,h_LP);
    % cancel delay
    ecg_lpf = ecg_lpf(6+[1: N]); 
    ecg_lpf = ecg_lpf/max(abs(ecg_lpf));
    
    % HPF = Allpass-(Lowpass) = z^-16-[(1-z^-32)/(1-z^-1)]
    b = [-1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32 -32 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1];
    a = [1 -1];
    
    % impulse response iof HPF
    h_HP=filter(b,a,[1 zeros(1,32)]); 

    ecg_hpf = conv(ecg_lpf,h_HP);
    % cancel delay
    ecg_hpf = ecg_hpf(16+[1:N]);
    ecg_hpf = ecg_hpf/max(abs(ecg_hpf));

    bp_ecg = ecg_hpf;
end
