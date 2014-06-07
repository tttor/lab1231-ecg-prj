function wavelet_img = run_wavelet_transform(ecg_mat, CR, res, n_frame, out_dir)
    fprintf('\nWAVELET_TRANSFORM =========================================\n');
    wavelet_img = {};
    S = {};
    
    percent = 0;
    ten_percent = floor(n_frame/10);
    for k = 1:n_frame
        % Display progress
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        
        %
        wave_type = 'bior4.4';
        dec_lvl = 7;
        mat_in = cell2mat(ecg_mat{k});
        
        [local_wavelet_img, local_S] = wavelet_transform(mat_in, wave_type, dec_lvl);
        wavelet_img{k} = local_wavelet_img;
        S{k} = local_S;
    end
    
    % Save
    for k = 1:n_frame
        wavelet_img_csv = strcat(out_dir, 'wavelet-img/wavelet_img-', int2str(k), '.csv');
        csvwrite(wavelet_img_csv, wavelet_img{k});
        
        S_csv = strcat(out_dir, 'wavelet-img/S-', int2str(k), '.csv');
        csvwrite(S_csv, S{k});
    end
    
    fprintf(' [OK]\n');
end
