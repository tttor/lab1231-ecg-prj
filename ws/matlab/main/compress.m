function [ bit_str, order, S ] = compress( ecg_in, wv_type, bitrate, rr, beat_ord)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
    
    sample_num = 256;
    beat_num   = 256;
    clust_num  = 3;
    ecg_mat = ecg2mat(ecg_in, sample_num, beat_num);
    %save ecg_mat ecg_mat
    if beat_ord == 1
        [ecg_mat_ord, order] = reorder_beat (ecg_mat, clust_num);
    else
        ecg_mat_ord = ecg_mat;
        order = [];
    end
    [bit_str, S] = SPIHT_enc(ecg_mat_ord, wv_type, bitrate);
end

