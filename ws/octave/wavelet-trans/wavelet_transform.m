function [wavelet_img, S] = wavelet_transfrom(mat_in, wv_type, level)
    % wavelet decomposition level can be defined by users manually.
    [Lo_D,Hi_D,Lo_R,Hi_R] = wfilters(wv_type);
    [wavelet_img, S] = func_DWT(mat_in, level, Lo_D, Hi_D);
end
