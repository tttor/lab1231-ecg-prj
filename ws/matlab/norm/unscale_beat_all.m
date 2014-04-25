function [ ecg_out ] = unscale_beat_all( ecg_in, r_ann, scaled )
    lead_num     = size(ecg_in,1);
    fprintf('\n');
    for k = 1:lead_num
        ecg_out(k,:) = unscale_beat(ecg_in(k,:),r_ann,scaled);
        fprintf('Beat Scaling lead-%i     : [OK]\n\n',k);
    end
end