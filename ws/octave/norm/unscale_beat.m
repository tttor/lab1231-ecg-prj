function [ ecg_out ] = unscale_beat ( ecg_in, rr_ann_in, scaled )
% Denormalize number of sample per heartbeat to original length
% using resampling approach

    padding = 1;
    orde    = 2;
    ecg_out = [];
    beat_num = size(rr_ann_in,2)-1;
    ten_percent = floor(beat_num/10);
    fprintf('Beat unscaling          : ');

    for k = 1:beat_num
        if mod(k,ten_percent) == 0
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        if k == 1
            beat_dur  = rr_ann_in(2,1);
            start_idx = 1;
            end_idx   = scaled + padding;
            us_beat   = resample(ecg_in(start_idx:end_idx),beat_dur + padding,scaled + padding,orde);
            us_beat   = us_beat(1:beat_dur);  
        else
            beat_dur  = rr_ann_in(1,k-1);
            start_idx = (k-1) * scaled - padding + 1;
            if start_idx == 0
                start_idx = 1;
            end
            end_idx   = start_idx + scaled + padding + 1; 
            us_beat   = resample(ecg_in(start_idx:end_idx),beat_dur + 2*padding,scaled + 2*padding,orde);
            us_beat   = us_beat(1 + padding:beat_dur + padding);
        end
        ecg_out   = [ecg_out us_beat];
    end
    
    beat_dur  = rr_ann_in(1,beat_num);
    start_idx = beat_num * scaled + 1;
    end_idx   = (beat_num + 1) * scaled;
    us_beat   = resample(ecg_in(start_idx:end_idx),beat_dur,scaled,orde);
    ecg_out   = [ecg_out us_beat];
    
    % last beat
    beat_dur  = rr_ann_in(1,end) - 1;
    start_idx = size(ecg_in,2) - scaled;
    end_idx   = size(ecg_in,2);
    us_beat   = resample(ecg_in(start_idx:end_idx),beat_dur,scaled,orde);
    fprintf(' [OK]\n');
    ecg_out   = [ecg_out us_beat]; 
end