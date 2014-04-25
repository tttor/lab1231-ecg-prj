function [mat_out] = SPIHT_dec(bit_str, S,wv_type, dec_lvl, nRow, nCol)

    n = min(nRow,nCol);
    n_log = log2(n);
    %level = floor(n_log);
    level = dec_lvl;
    
    %fprintf('-----------   Decoding   ----------------\n');
    img_dec = func_SPIHT_Dec(bit_str);
    %fprintf('done!\n');

    %fprintf('-----------   Wavelet Reconstruction   ----------------\n');
    [Lo_D,Hi_D,Lo_R,Hi_R] = wfilters(wv_type);
    mat_out = func_InvDWT(img_dec, S, Lo_R, Hi_R, level);
        
    %fprintf('done!\n');
end