function [ prd_out ] = prd( ecg_ori, ecg_rec, normalize )
% Percent of root mean square difference (PRD) calculation

    ecg_ori_nn = ecg_ori;
    ecg_rec_nn = ecg_rec;
    if normalize == 1
        ecg_ori = mat2gray(ecg_ori);
        ecg_rec = mat2gray(ecg_rec);
    end
    
    % First two minutes of signal
    %ecg_ori = ecg_ori(1:43200);
    %ecg_rec = ecg_rec(1:43200);
    
    sigma1 = 0;
    sigma2 = 0;
    ecg_mean = mean(ecg_ori);
    for k = 1:size(ecg_ori,2)
        sigma1 = sigma1 + (ecg_ori(k) - ecg_rec(k))^2;
        sigma2 = sigma2 + ecg_ori(k)^2;
        %sigma2 = sigma2 + (ecg_ori(k) - ecg_mean)^2;
    end
    prd_out = sqrt(sigma1/sigma2) * 100;
    
    t = 1:size(ecg_ori,2);
    %plot(t,ecg_ori(t),'r',t,ecg_rec(t),'b');
end

