function [ ecg_out ] = scale_beat ( ecg_in, rr_ann_in, scaled )
% Normalize number of sample per heartbeat using resampling approach
    
    padding = 1;
    orde    = 2;
    ecg_out = [];
    beat_num = size(rr_ann_in,2)-1;
    ten_percent = floor(beat_num/10);
    fprintf('Beat scaling            : ');

    for k = 1:beat_num
        if mod(k,ten_percent) == 0
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        
        if k == 1
            start_idx = 1;
            end_idx   = rr_ann_in(2,k) + padding;
            us_beat   = ecg_in(start_idx:end_idx);
            s_beat    = resample(us_beat,scaled + padding,size(us_beat,2),orde);
            ecg_out   = [ecg_out s_beat(1:scaled)];
        else
            start_idx = rr_ann_in(2,k) - rr_ann_in(1,k-1) - padding + 1;
            if start_idx == 0
                start_idx = 1;
            end
            end_idx   = rr_ann_in(2,k) + padding;
            us_beat   = ecg_in(start_idx:end_idx);
            s_beat    = resample(us_beat,scaled + 2*padding,size(us_beat,2),orde);
            ecg_out   = [ecg_out s_beat(1,1 + padding:size(s_beat,2) - padding)];
        end
    end
    
    start_idx = rr_ann_in(2,beat_num) + 1;
    end_idx = rr_ann_in(2,beat_num) + rr_ann_in(1,beat_num);
    us_beat = ecg_in(start_idx:end_idx);
    s_beat  = resample(us_beat,scaled,size(us_beat,2),orde);
    ecg_out = [ecg_out s_beat];
    
    % last beat
    start_idx = rr_ann_in(2,beat_num) + rr_ann_in(1,beat_num) + 1;
    us_beat = ecg_in(start_idx:end);
    s_beat  = resample(us_beat,scaled,size(us_beat,2),orde);
    fprintf(' [OK]\n');
    ecg_out = [ecg_out s_beat];   
end