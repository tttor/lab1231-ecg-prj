function optimal_wavelet( wv_type )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    %    CR      8     16     24     32      
    bitrate = [1.375 0.6875 0.4583 0.3438];
    prd_arr = zeros(1,size(bitrate,2));
    for k = 1:size(bitrate,2)
        fprintf('\nCR = %2.0f\n',11/bitrate(k));
        prd_arr(1,k) = codec_all(wv_type, bitrate(k), 0);
    end
    prd_arr
end

