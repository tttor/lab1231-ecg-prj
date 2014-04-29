function [ err ] = SPIHT_err( rec, org )
% calculate SPIHT coding error using norm

    frame_num = size(rec,1);
    err = [];
    for k = 1:frame_num
        rec_1 = cell2mat(rec(k));
        org_1 = cell2mat(org(k));
        err_1 = norm(rec_1-org_1);
        err   = [err; err_1];
    end
end