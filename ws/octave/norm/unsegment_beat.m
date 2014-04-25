function [ ecg_out ] = unsegment_beat( ecg_frame, org_sample_num )
% Unsegment framed ECG into normalized ECG signal
    
    frame_num  = size(ecg_frame,1);
    frame_size = size(ecg_frame,2);
    sampel_num = sqrt(frame_size);

    ecg_out = [];
    for k = 1:frame_num-1
        ecg_out = [ecg_out ecg_frame(k,:)];
    end
    
    % Last frame
    last_beat_idx = org_sample_num - frame_size * ...
                    floor(org_sample_num/frame_size);
    last_frame    = ecg_frame(frame_num,1:last_beat_idx);
    ecg_out = [ecg_out last_frame];
    
    fprintf('\nUnsegment frames        : [OK]\n');
end