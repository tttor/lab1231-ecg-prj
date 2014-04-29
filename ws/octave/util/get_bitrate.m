function [ bitrate ] = get_bitrate( CR, rate_org, ecg_org, ecg_norm )
% Calculate bitrate for beat normalized ECG signal

    sample_num_org  = size(ecg_org,2);
    sample_num_norm = size(ecg_norm,2);
    rate_norm = floor(rate_org * sample_num_org/sample_num_norm);
    bitrate   = rate_norm/CR;
end

