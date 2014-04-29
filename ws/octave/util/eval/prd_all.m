function prd_out = prd_all(ecg_ori, ecg_rec, normalize)
% PRD calculation for all ECG lead

    lead_num   = size(ecg_ori,1);
    sample_num = min(size(ecg_ori,2),size(ecg_rec,2));
    ecg_ori_nn = ecg_ori;
    ecg_rec_nn = ecg_rec;
    if normalize == 1
        ecg_ori = mat2gray(ecg_ori);
        ecg_rec = mat2gray(ecg_rec);
    end

    sigma1 = 0;
    sigma2 = 0;
    ecg_mean = mean(ecg_ori);
    for k = 1:lead_num
        for l = 1:sample_num
            sigma1 = sigma1 + (ecg_ori(k,l) - ecg_rec(k,l))^2;
            sigma2 = sigma2 + ecg_ori(k,l)^2;
            %sigma2 = sigma2 + (ecg_ori(k) - ecg_mean)^2;
        end
    end
    prd_out = sqrt(sigma1/sigma2) * 100;
end