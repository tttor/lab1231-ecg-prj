function [ ecg_multi ] = single2multi( ecg_single, rr_ann_in, sample_num, grouping )
% Convert single lead ECG back into multi lead ECG

    beat_num   = size(rr_ann_in,2) + 1;
    lead_num   = 8;
    group_mode = 1;
    ecg_multi  = zeros(lead_num,beat_num*sample_num);
    
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
                for m = 1:size(group_arr,2)
                    start_idx = end_idx + 1;
                    end_idx   = start_idx + sample_num - 1;
                    one_beat  = ecg_single(start_idx:end_idx);
                    start_idx_mul = (l-1)*sample_num + 1;
                    end_idx_mul   = l*sample_num;
                    ecg_multi(group_arr(m),start_idx_mul:end_idx_mul) = one_beat;
                end
            end
        end
    
    else
        
    % Previous approach: no grouping
        for k = 1:beat_num
            for l = 1:lead_num
                start_idx = end_idx + 1;
                end_idx   = start_idx + sample_num - 1;
                one_beat  = ecg_single(start_idx:end_idx);

                start_idx_mul = (k-1)*sample_num + 1;
                end_idx_mul   = k*sample_num;
                ecg_multi(l,start_idx_mul:end_idx_mul) = one_beat;
            end
        end

    end
    
    fprintf('\nConvert multi to single : [OK]\n');
end

