function [ ecg_frame ] = segment_beat( ecg_in, beat_num)
% Segment normalized ECG signal into frames
% one frame consist beat_num heartbeats
% pad zeros at last frame if needed
    fprintf('segment_beat( ecg_in, beat_num):BEGIN\n');
    size(ecg_in)
    beat_num
    
    %lead_num = 8;
    frame_num  = floor(size(ecg_in,2)/beat_num^2)
    
    for k = 1:frame_num
        start_idx  = (k-1)*beat_num^2 + 1;
        end_idx    = k*beat_num^2;
        one_frame  = ecg_in(1,start_idx:end_idx);
        ecg_frame(k,:) = one_frame;
    end
    numel(ecg_frame)
    
    % Last frame
    last_frame = ecg_in(1,end_idx+1:end);
    pad_len    = beat_num^2 - size(last_frame,2);
    pad = zeros(1,pad_len);
    ecg_frame(frame_num+1,:) = [last_frame pad];
    numel(ecg_frame)
    
    %{
    %verify that the segmentation process run correctly
    ecg_out = [];
    for k = 1:frame_num
        ecg_out = [ecg_out ecg_frame(k,:)];
    end
    t = size(ecg_out,2);
    norm(ecg_in(1:t)-ecg_out(1:t))
    %}
    
    fprintf('\nSegment ECG into frames : [OK]\n'); 
end
