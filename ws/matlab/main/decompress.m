function [ ecg_rec ] = decompress( bit_str, wv_type, order, S, rr, beat_ord)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
    
    sample_num  = 256;
    beat_num    = 256;
    ecg_mat_rec = SPIHT_dec(bit_str, wv_type, S, sample_num, beat_num);
    if beat_ord == 1
        ecg_mat_uno = unorder_beat(ecg_mat_rec, order);
    else
        ecg_mat_uno = ecg_mat_rec;
    end
    ecg_rec = mat2ecg(ecg_mat_uno, sample_num, beat_num);
end