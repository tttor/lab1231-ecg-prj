function [ corr_mat ] = lead_corr( ecg_in )
% Calculate correlation between each lead of multi lead ECG signal

    lead_num = size(ecg_in,1);

    corr_mat = zeros(lead_num,lead_num);
    for k = 1:lead_num
        for l = k:lead_num
            corr_mat = xcorr(ecg_in(k),ecg_in(j));
        end
    end
end

