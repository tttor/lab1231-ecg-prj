function bit_str = run_spiht(wavelet_img, CR, res, n_frame, out_dir)
    fprintf('\nCODING SPIHT =========================================\n');
    
    n_lead = 8;
    n_sample = 46260;
    max_bits = n_sample*n_lead*res/CR;
    max_bits = round(max_bits/n_frame);

    bit_str = {};
    
    percent = 0;
    ten_percent = floor(n_frame/10);
    for k = 1:n_frame
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        
        img = wavelet_img{k};
        level = 7;
        local_bit_str = SPIHT_enc(img, max_bits, numel(img), level);
        bit_str{k} = local_bit_str;
    end
    
    % Save
    for k = 1:n_frame
        filename = strcat(out_dir, 'bit-str/bit_str-', int2str(k), '.csv');
        M = bit_str{k};
        
        csvwrite(filename, M);
    end
    
    fprintf(' [OK]\n');
end
