function [ wwprd_out ] = wwprd_all(ecg_ori, ecg_rec, normalize)
% WWPRD calculation for multilead ECG signal

    lead_num   = size(ecg_ori,1);
    sample_num = min(size(ecg_ori,2),size(ecg_rec,2));
    ecg_ori_nn = ecg_ori;
    ecg_rec_nn = ecg_rec;
    
    if normalize == 1
        ecg_ori = mat2gray(ecg_ori);
        ecg_rec = mat2gray(ecg_rec);
    end
    
    sigma_wwprd = 0;
    for k = 1:lead_num
        wwprd(ecg_ori(k,:),ecg_rec(k,:),1)
        sigma_wwprd = sigma_wwprd + wwprd(ecg_ori(k,:),ecg_rec(k,:),1);
    end
    wwprd_out = sigma_wwprd/lead_num

end

