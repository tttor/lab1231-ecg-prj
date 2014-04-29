function codec ( ecg_in, rr_ann_in, CR )
% Test and verify all functions in this project    
    
    % Correlation analysis result shows that the most correlated leads are
    % (sorted descending):
    %
    % lead 4-5 (V2-V3)
    % lead 5-6 (V3-V4)
    % lead 3-7 (V1-V5)
    % lead 4-6 (V2-V4)
    % lead 1-7 (I-V5)
    % lead 7-8 (V5-V6)
    % lead 2-3 (II-V1)
    %
    % Following lead arrangement could give better compression performance
    % group-1 : lead 1,7,8
    % group-2 : lead 2,3 (inv)
    % group-3 : lead 4,5,6
    %
    % Original ECG signal bitrate = 12 bits/sample
    % Number of samples on original ECG signal   : 462.600
    % Number of samples on normalized ECG Signal : 706.048
    % Normalized ECG signal bitrate 12 * 462.600/706048 = 7.86 bits/sample 
    % -> 7 bits/sample
    %     CR      8      16     24      32      
    % bitrate = [0.875 0.4375 0.2917 0.21875];
    
    
    
    sample_num = 256; % number of sample per beat on normalized ECG
    beat_num   = 256; % number of beat per frame on normalized ECG
    wv_name = 'bior4.4';
       
    % ====================================================================
    % PREPROCESSING
    % ====================================================================
    
    fprintf('\nPREPROCESSING =========================================\n');
    base = 0;
    gain = 306;
    % Select lead I, II, V1..V6 as input signal
    ecg8 = select_lead(ecg_in);
    
    % Physical unit conversion, BWR, LPF
    [ecg8_pre,rr_ann] = preprocess(ecg8,rr_ann_in,base,gain);
    
    
    % ====================================================================
    % COMPRESSION STAGE
    % ====================================================================
    
    fprintf('\nCOMPRESSION ===========================================\n');

    % Beat normalization
    ecg8_scl  = scale_beat_all(ecg8_pre,rr_ann,sample_num);
    
    bitrate = get_bitrate(CR,12,ecg8,ecg8_scl);

    % Multi lead to single lead ECG conversion
    ecg1      = multi2single(ecg8_scl,rr_ann);
    
    % Single lead ECG segmentation into frames
    ecg1_frm  = segment_beat(ecg1,beat_num);
    
    % ECG frames to 2D ECG matrix conversion
    frame_num = size(ecg1_frm,1);
    ecg_mat = cell(frame_num,sample_num,beat_num);
    for k = 1:frame_num
        ecg_mat(k) = {ecg2mat(ecg1_frm(k,:),sample_num,beat_num)};
    end
    
    % Beat reordering
    fprintf('\nBeat reordering         : ');
    percent = 0;
    clust_num = 3;
    ten_percent = floor(frame_num/10);
    ecg_mat_ord = cell(frame_num,sample_num,beat_num);
    s_order = [];
    for k = 1:frame_num
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        [ecg_mat_ord_1,s_order1] = reorder_beat (cell2mat(ecg_mat(k)),clust_num);
        ecg_mat_ord(k) = {ecg_mat_ord_1};
        s_order = [s_order; s_order1];
    end
    fprintf(' [OK]\n');
    
    % 2D SPIHT coding
    fprintf('\n2D SPIHT coding         : ');
    percent = 0;
    ten_percent = floor(frame_num/10);
    for k = 1:frame_num
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        [s_bit_str_1,S] = SPIHT_enc(cell2mat(ecg_mat_ord(k)),wv_name,bitrate);
        s_bit_str(k) = {s_bit_str_1};
    end
    fprintf(' [OK]\n');
    
    % Save following variables for decompression stage:
    % s_order, s_bit_stream, S
    
    % ====================================================================
    % DECOMPRESSION STAGE
    % ====================================================================
    
    fprintf('\nDECOMPRESSION =========================================\n');

    % 2D SPIHT decoding
    
    fprintf('\n2D SPIHT decoding       : ');
    percent = 0;
    ten_percent = floor(frame_num/10);
    recg_mat_ord = cell(frame_num,sample_num,beat_num);
    for k = 1:frame_num
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        recg_mat_ord_1 = SPIHT_dec(cell2mat(s_bit_str(k)),S,wv_name,sample_num,beat_num);
        recg_mat_ord(k) = {recg_mat_ord_1};
    end
    fprintf(' [OK]\n');
        
    % 2D SPIHT coding verification
    err = [];
    for k = 1:frame_num
        err = [err; norm(cell2mat(ecg_mat_ord(k))-cell2mat(recg_mat_ord(k)))];
    end;
    fprintf('2D SPIHT coding error (norm)        : %f \n',mean(err));
    
    % Beat unordering
    percent = 0;
    ten_percent = floor(frame_num/10);
    recg_mat = cell(frame_num,sample_num,beat_num);
    fprintf('\nBeat unordering         : ');

    for k = 1:frame_num
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        recg_mat_1 = unorder_beat (cell2mat(recg_mat_ord(k)),s_order(k,:));
        recg_mat(k) = {recg_mat_1};
    end
    fprintf(' [OK]\n');
    
    % Beat reordering verification
    err = [];
    for k = 1:frame_num
        err = [err; norm(cell2mat(ecg_mat(k))-cell2mat(recg_mat(k)))];
    end;
    %fprintf('Beat reordering stage error (norm)  : %f \n',mean(err));
    
    % 2D ECG to ECG frames matrix conversion
    recg1_frm = [];
    for k = 1:frame_num
        recg1_frm = [recg1_frm; mat2ecg(cell2mat(recg_mat(k)),sample_num,beat_num)];
    end
    
    % verify ecg/matrix conversion stage
    err = [];
    for k = 1:frame_num
        err = [err; norm(ecg1_frm(k,:)-recg1_frm(k,:))];
    end;
    %fprintf('\nECG/matrix conversion error (norm)  : %f \n\n',mean(err));

    % Unsegment ECG frames into single lead ECG
    recg1 = unsegment_beat(recg1_frm,size(ecg1,2));
    
    % verify segmentation stage
    %fprintf('Segmentation stage error (norm)     : %f \n',norm(ecg1-recg1));
    
    recg8_scl = single2multi(recg1,rr_ann);
    
    % verify single/multi conversion stage
    %fprintf('\nSingle/multi conversion error (norm): %f \n',norm(recg8_scl-ecg8_scl));

    % Beat denormalization
    recg8_pre  = unscale_beat_all(recg8_scl,rr_ann,sample_num);
    
    %verify beat normalization stage
    fprintf('\nPRD                                 : %f \n',prd_all(recg8_pre,ecg8_pre,1));

    % save('SPIHT codec.mat');

end