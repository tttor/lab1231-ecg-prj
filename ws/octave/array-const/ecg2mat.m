function [ ecg_mat ] = ecg2mat ( ecg_in, sample_num, beat_num )
% Convert ECG to matrix(sample_num,beat_num)

    ecg_mat = zeros(beat_num,sample_num);
    for k = 1:beat_num
        start_idx = (k-1)*sample_num+1;
        end_idx   = start_idx + sample_num - 1;
        ecg_mat(k,:) = ecg_in(start_idx:end_idx);
    end
end