function PRD = get_PRD_all(ecg_ori, ecg_rec, normalize)
    % PRD calculation for all ECG lead

    n_lead   = size(ecg_ori,1);
    n_sample = min(size(ecg_ori,2), size(ecg_rec,2));
    
    if normalize == 1
        ecg_ori = mat2gray(ecg_ori);
        ecg_rec = mat2gray(ecg_rec);
    end

    sigma1 = 0;
    sigma2 = 0;
    ecg_mean = mean(ecg_ori);
    for k = 1:n_lead
        for l = 1:n_sample
            sigma1 = sigma1 + (ecg_ori(k,l) - ecg_rec(k,l))^2;
            sigma2 = sigma2 + ecg_ori(k,l)^2;
            %sigma2 = sigma2 + (ecg_ori(k) - ecg_mean)^2;
        end
    end
    PRD = sqrt(sigma1/sigma2) * 100;
end
