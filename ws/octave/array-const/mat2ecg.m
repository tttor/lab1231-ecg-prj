function [ ecg_out ] = mat2ecg ( ecg_mat, sample_num, beat_num )
% Convert matrix(sample_num,beat_number) back to ECG signal

    ecg_len = sample_num*beat_num;
    ecg_out = zeros(1,ecg_len);
    k = 0;
    for b = 1:beat_num
        for s = 1:sample_num
            k = k+1;
            ecg_out(k) = ecg_mat(b,s);
        end
    end
end