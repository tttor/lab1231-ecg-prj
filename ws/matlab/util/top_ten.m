function [ mean_prd ] = top_ten( bitrate, n)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    
    prd_arr = zeros(n,1);
    for k = 1:n
        k
        prd_arr(k) = codec_all('bior6.8', bitrate, 1);
    end
    prd_arr  = sortrows(prd_arr);
    mean_prd = prd_arr(1:10)
end

