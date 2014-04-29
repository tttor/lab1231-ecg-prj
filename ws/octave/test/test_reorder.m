function [ ecg_mat_ord ] = test_reorder( ecg_mat )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    
    percent = 0;
    clust_num = 4;
    
    frame_num = size(ecg_mat,1);
    fprintf('Beat reordering         : ');
    ten_percent = floor(frame_num/10);
    
    for k = 1:frame_num
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        [ecg_mat_ord_1, order] = reorder_beat (cell2mat(ecg_mat(k)), clust_num);
        ecg_mat_ord(k) = {ecg_mat_ord_1};
    end
    fprintf(' [OK]\n');

end

