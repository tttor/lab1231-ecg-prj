function [ mat_ecg_out ] = unorder_beat ( mat_ecg_in, order )
% Unorder heartbeat position back to the original place
    
    order = order';
    order = [order (1:size(mat_ecg_in,1))'];
    order = sortrows(order);
    mat_ecg_out = mat_ecg_in(order(:,2),:);
end


