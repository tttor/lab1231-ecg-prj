function [ ecg_single ] = multi2single( ecg_multi, rr_ann_in, sample_num, grouping )
% Convert multi lead ECG into single lead ECG
% this stage will simplify the matrix ECG conversion

    beat_num   = size(rr_ann_in,2) + 1;
    lead_num   = 8;
    group_mode = 1; % 1:Correlation 2:Point of view
    ecg_single = [];
    
    % 1:I 2:II 3:V1 4:V2 5:V3 6:V4 7:V5 8:V6
    if group_mode == 1
        group(1) = {[1 7 8]};
        group(2) = {[2 3]};
        group(3) = {[4 5 6]};
    else
        group(1) = {[2]};
        group(2) = {[1 6]};
        group(3) = {[3 4]};
        group(4) = {[5]};
        group(5) = {[7 8]};
    end
    
    end_idx  = 0;
    
    if grouping == 1
    
        for k = 1:size(group,2)
            group_arr  = cell2mat(group(k));
            for l = 1:beat_num
                start_idx  = (l-1)*sample_num + 1;
                end_idx    = l*sample_num;
                for m = 1:size(group_arr,2)
                    one_beat   = ecg_multi(group_arr(m),start_idx:end_idx);
                    ecg_single = [ecg_single one_beat];
                end
            end
        end
    
    else
        
    % previous approach: no grouping
        for k = 1:beat_num
            start_idx  = (k-1)*sample_num + 1;
            end_idx    = k*sample_num;
            for l = 1:lead_num
                one_beat = ecg_multi(l,start_idx:end_idx);
                ecg_single = [ecg_single one_beat];
            end
        end
    
    end
    
    fprintf('Convert multi to single : [OK]\n');
end

