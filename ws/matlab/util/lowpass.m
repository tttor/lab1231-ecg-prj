function [ ecg_out ] = lowpass( ecg_in, verbose )
%   Low pass filter using transfer function (1-z^-6)^2/(1-z^-1)^2
    
    N = size(ecg_in,2);
    M = max(abs(ecg_in));
    ecg_norm = ecg_in - mean (ecg_in);    % cancel DC components
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
    ecg_lpf = M * ecg_lpf/max(abs(ecg_lpf));
    if verbose == 1
        fprintf('Low pass filter         : [OK]\n');
    end
    ecg_out = ecg_lpf;
end
