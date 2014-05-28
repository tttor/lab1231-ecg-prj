function [bit_str, S] = SPIHT_enc(mat_in, wv_type, dec_lvl, max_bits)

    [nRow, nCol] = size(mat_in);
    OrigSize = nRow * nCol;

    %fprintf('-----------   Wavelet Decomposition   ----------------\n');
    n = min(nRow,nCol);
    n_log = log2(n);
    %level = floor(n_log);
    level = dec_lvl;
    % wavelet decomposition level can be defined by users manually.
    [Lo_D,Hi_D,Lo_R,Hi_R] = wfilters(wv_type);
    [I_W, S] = func_DWT(mat_in, level, Lo_D, Hi_D);
        
    %fprintf('done!\n');

    %fprintf('-----------   Encoding   ----------------\n');
    bit_str = func_SPIHT_Enc(I_W, max_bits, nRow*nCol, level);
    %fprintf('done!\n');
end