function recg8_pre = decode(out_dir, rr_ann, n_frame, n_sample, n_ecg1_col)
    % Load bit-str and S
    fprintf('Reading bit-str and S files...')
    
    bit_str = {};
    for k = 1:n_frame
        bit_str_csv = strcat(out_dir, 'bit-str/bit_str-', int2str(k), '.csv');
        bit_str{k} = csvread(bit_str_csv);
    end
    
    S_num = 1;% S is the same for all frames
    S_csv = strcat(out_dir, 'wavelet-img/S-', int2str(S_num), '.csv');
    S = csvread(S_csv);    

    % 
    recg8_pre = decode_2(bit_str, S, rr_ann, n_frame, n_sample, n_ecg1_col);
end
