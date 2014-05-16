function prd_out = codec_non_pre( rec_name, ecg_in, RR_ANN_IN, sample_num, CR, group, order, res, rep )

%tic;
    
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
    
    lead_num       = 8;
    beat_num       = sample_num; % number of beat per frame on normalized ECG
    tot_sample_num = 46260;
    %tot_sample_num = 2570;
    resolution     = 12;
    max_bits       = tot_sample_num*lead_num*resolution/CR;
    wv_name        = 'bior4.4';
    dec_lvl        = 7;
       
    % ====================================================================
    % PREPROCESSING
    % ====================================================================
    
    
    fprintf('\nPREPROCESSING =========================================\n');
    base = 0;
    gain = 306;
    
    ecg8_pre_temp = csvread('ecg8_pre_matlab.csv');
    ecg8_pre = ecg8_pre_temp';
    
    rr_ann = csvread('rr_ann_matlab.csv');
    
    fprintf('\nREAD CSV FILE =========================================\n');
    
    % ====================================================================
    % COMPRESSION STAGE
    % ====================================================================
    
    fprintf('\nCOMPRESSION ===========================================\n');

    % Beat normalization
    %ecg8_scl = scale_beat_all(ecg8_pre,rr_ann,sample_num);
    ecg8_scl_temp = csvread('I01m.normalize.csv');
    ecg8_scl = ecg8_scl_temp';
 
    fprintf('\nREAD CSV FILE =========================================\n');
    
    
    % Multi lead to single lead ECG conversion
    ecg1 = multi2single(ecg8_scl,rr_ann,sample_num,group);
    
    
    % Single lead ECG segmentation into frames
    %ecg1_frm = segment_beat(ecg1,beat_num);
    
    ecg1_frm_temp = csvread('I01m.frame.csv');
    ecg1_frm = ecg1_frm_temp';
    
    fprintf('\nREAD CSV FILE =========================================\n');
    
    
    % ECG frames to 2D ECG matrix conversion
    frame_num = size(ecg1_frm,1);
    ecg_mat = cell(frame_num,sample_num,beat_num);
    for k = 1:frame_num
        ecg_mat(k) = {ecg2mat(ecg1_frm(k,:),sample_num,beat_num)};
    end
    
    % Beat reordering
    if order == 0
        ecg_mat_ord = ecg_mat;
    else
        fprintf('\nBeat reordering         : ');
        percent = 0;
        clust_num = 3;
        ten_percent = floor(frame_num/10);
        ecg_mat_ord = cell(frame_num,sample_num,beat_num);
        ORDER  = [];
        N_ITEM = cell(frame_num);
        C_IDX  = cell(frame_num);

        for k = 1:frame_num
            if mod(k,ten_percent) == 0 && percent < 100
                percent = 10*k/ten_percent;
                fprintf('% 3.0f',percent);
            end
            [ecg_mat_ord_1,order_1,n_item_1,c_idx_1] = reorder_beat (cell2mat(ecg_mat(k)),clust_num);
            ecg_mat_ord(k) = {ecg_mat_ord_1};
            ORDER     = [ORDER; order_1];
            N_ITEM(k) = {n_item_1};
            C_IDX(k)  = {c_idx_1};
        end
        fprintf(' [OK]\n');
    end
    
    % Residual calculation
    if res == 0
        ecg_mat_res = ecg_mat_ord;
    else
        fprintf('\nResidual calculation    : ');
        ECG_MOD     = cell(frame_num,sample_num,beat_num);
        ecg_mat_res = cell(frame_num,sample_num,beat_num);
        for k = 1:frame_num
            [ecg_mod_1, ecg_mat_res_1] = get_ecg_res(cell2mat(ecg_mat(k)),cell2mat(N_ITEM(k)),cell2mat(C_IDX(k)));
            ECG_MOD(k)     = {ecg_mod_1};
            ecg_mat_res(k) = {ecg_mat_res_1};
        end
        fprintf('[OK]\n');
    end
    
    
    %{
    % Padding
    new_dim = 2^nextpow2(sample_num);
    pad     = new_dim - sample_num;
    pecg_mat_res_1 = cell(frame_num);
    for k = 1:frame_num
       pecg_mat_res_1  = padarray(cell2mat(ecg_mat_res(k)),[pad,pad],'post','replicate');
       pecg_mat_res(k) = {pecg_mat_res_1};
    end
    ecg_mat_res = pecg_mat_res;
    %}
    
        
    % 2D SPIHT coding
    fprintf('\n2D SPIHT coding         : ');
    max_bits = round(max_bits/frame_num);
    percent = 0;
    ten_percent = floor(frame_num/10);
    for k = 1:frame_num
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        [bit_str_1,S] = SPIHT_enc(cell2mat(ecg_mat_res(k)),wv_name,dec_lvl,max_bits);
        BIT_STR(k) = {bit_str_1};
    end
    fprintf(' [OK]\n');
    

    % Save variables with all caps name for decompression stage:
    
    % ====================================================================
    % DECOMPRESSION STAGE
    % ====================================================================
    
    fprintf('\nDECOMPRESSION =========================================\n');

    
    % 2D SPIHT decoding
    fprintf('\n2D SPIHT decoding       : ');
    percent = 0;
    ten_percent = floor(frame_num/10);
    recg_mat_res = cell(frame_num,sample_num,beat_num);
    for k = 1:frame_num
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        recg_mat_res_1 = SPIHT_dec(cell2mat(BIT_STR(k)),S,wv_name,dec_lvl,sample_num,beat_num);
        recg_mat_res(k) = {recg_mat_res_1};
    end
    fprintf(' [OK]\n');
    
    %{
    % 2D SPIHT coding verification
    err = [];
    for k = 1:frame_num
        err = [err; norm(cell2mat(ecg_mat_res(k))-cell2mat(recg_mat_res(k)))];
    end;
    fprintf('2D SPIHT coding error (norm)        : %f \n',mean(err));
    %}
    
    
    %{
    % Remove padding
    pecg_mat_res = cell(frame_num);
    for k = 1:frame_num
       pecg_mat_res_1  = cell2mat(recg_mat_res(k));
       pecg_mat_res(k) = {pecg_mat_res_1(1:sample_num,1:beat_num)};
    end
    recg_mat_res = pecg_mat_res;
    %} 
    
    
    % Residual calculation
    if res == 0
        recg_mat_ord = recg_mat_res;
    else
        fprintf('\nResidual calculation    : ');
        recg_mat_ord_ = cell(frame_num,sample_num,beat_num);
        for k = 1:frame_num
            [recg_mat_ord_1] = set_ecg_res(cell2mat(ECG_MOD(k)),...
                               cell2mat(recg_mat_res(k)),cell2mat(N_ITEM(k)),...
                               cell2mat(C_IDX(k)),sample_num);
            recg_mat_ord(k) = {recg_mat_ord_1};
        end
        fprintf(' [OK]\n');    
    end
    
    
    % Beat unordering
    if order == 0
        recg_mat = recg_mat_ord;
    else
        percent = 0;
        ten_percent = floor(frame_num/10);
        recg_mat = cell(frame_num,sample_num,beat_num);
        fprintf('\nBeat unordering         : ');

        for k = 1:frame_num
            if mod(k,ten_percent) == 0 && percent < 100
                percent = 10*k/ten_percent;
                fprintf('% 3.0f',percent);
            end
            recg_mat_1 = unorder_beat (cell2mat(recg_mat_ord(k)),ORDER(k,:));
            recg_mat(k) = {recg_mat_1};
        end
        fprintf(' [OK]\n');
    end
    
    %{
    % Beat reordering verification
    err = [];
    for k = 1:frame_num
        err = [err; norm(cell2mat(ecg_mat(k))-cell2mat(recg_mat(k)))];
    end;
    fprintf('Beat reordering stage error (norm)  : %f \n',mean(err));
    %}
    
    
    % 2D ECG to ECG frames matrix conversion
    recg1_frm = [];
    for k = 1:frame_num
        recg1_frm = [recg1_frm; mat2ecg(cell2mat(recg_mat(k)),sample_num,beat_num)];
    end
    
    %{
    % verify ecg/matrix conversion stage
    err = [];
    for k = 1:frame_num
        err = [err; norm(ecg1_frm(k,:)-recg1_frm(k,:))];
    end;
    fprintf('\nECG/matrix conversion error (norm)  : %f \n\n',mean(err));
    %}
    
    
    % Unsegment ECG frames into single lead ECG
    recg1 = unsegment_beat(recg1_frm,size(ecg1,2));
    
    % verify segmentation stage
    % fprintf('Segmentation stage error (norm)     : %f \n',norm(ecg1-recg1));
    
    
    % Single to multi lead ECG conversion
    recg8_scl = single2multi(recg1,rr_ann,sample_num,group);
    
    % verify single/multi conversion stage
    % fprintf('\nSingle/multi conversion error (norm): %f \n',norm(recg8_scl-ecg8_scl));

    
    % Beat denormalization
    recg8_pre  = unscale_beat_all(recg8_scl,rr_ann,sample_num);
    
    %verify beat normalization stage
    prd_out = prd_all(recg8_pre,ecg8_pre,1);
    fprintf('\nPRD (CR = %2i) : %f \n',CR,prd_out);

    if group == 1
        group_fname = '-group';
    else
        group_fname = '';
    end

    if order == 1
        order_fname = '-order';
    else
        order_fname = '';
    end

    if res == 1
        res_fname = '-res';
    else
        res_fname = '';
    end
    %folder_name = 'C:\Users\asus\Documents\GitHub\lab1231-ecg-prj\ws\octave\result-exp\';
    folder_name = '/home/gj/lab1231-ecg-prj/dataset/graf-exp/2D/first3min/128/none/';
   
    f_name = [folder_name rec_name '-' num2str(sample_num) '-' num2str(CR) ...
              '.' num2str(rep) group_fname order_fname res_fname '-out.mat'];
    save(f_name,'recg8_pre');
    
    
    %f_name_all = [folder_name rec_name '-' num2str(sample_num) '-' num2str(CR) ...
     %         '.' num2str(rep) group_fname order_fname res_fname '-all.mat'];
    
    %save(f_name_all,'var');
    save('var.mat');
    %whos
   
%toc
    
end