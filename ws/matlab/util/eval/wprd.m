function [ wprd_out ] = wprd( c_ori, c_rec )
% WPRD calculation of wavelet coefficients

    sigma_dc  = 0;
    sigma_ci2 = 0;
    
    for k = 1:size(c_ori,2)
        sigma_dc  = sigma_dc + (c_ori(k) - c_rec(k))^2;
        sigma_ci2 = sigma_ci2 + c_ori(k)^2;
    end
    
    wprd_out = 100 * sqrt(sigma_dc/sigma_ci2);
end

