function temp_codec( rec_name, ecg_in, RR_ANN_IN, sample_num, CR, group, order, res, rep )

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
    dec_lvl        = 1;
       
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
 
    end
    
    % Residual calculation
    if res == 0
        ecg_mat_res = ecg_mat_ord;
  
    end
    

        
    % 2D SPIHT coding
    fprintf('\n2D SPIHT coding         : ');
    max_bits = round(max_bits/frame_num);
    percent = 0;
    ten_percent = floor(frame_num/10);
    for k = 1:1
        
        if mod(k,ten_percent) == 0 && percent < 100
            percent = 10*k/ten_percent;
            fprintf('% 3.0f',percent);
        end
        
        %[bit_str_1,S,I_W] = SPIHT_enc(cell2mat(ecg_mat_res(k)),wv_name,dec_lvl,max_bits);
        mat_in =  cell2mat(ecg_mat_res(k));
        [nRow, nCol] = size(mat_in);
        OrigSize = nRow * nCol;

        %fprintf('-----------   Wavelet Decomposition   ----------------\n');
        n = min(nRow,nCol);
        n_log = log2(n);
        %level = floor(n_log);
        level = dec_lvl;
        % wavelet decomposition level can be defined by users manually.
        [Lo_D,Hi_D,Lo_R,Hi_R] = wfilters(wv_name);
        [I_W, S] = func_DWT(mat_in, level, Lo_D, Hi_D);

        %fprintf('done!\n');

        %fprintf('-----------   Encoding   ----------------\n');
        %bit_str_1 = func_SPIHT_Enc(I_W, max_bits, nRow*nCol, level);
        
        %===============
        
        %-----------   Initialization  -----------------
        m = I_W; 
        block_size = nRow*nCol;
        
        bitctr = 0;
        out = 2*ones(1,max_bits);
        maxm_pentung=max(m)';
        maxmaxm=max(max(m)');
        
        n_max = floor(log2(abs(max(max(m)'))));
        
        Bits_Header = 0;
        Bits_LSP = 0;
        Bits_LIP = 0;
        Bits_LIS = 0;

        %-----------   output bit stream header   ----------------
        % image size, number of bit plane, wavelet decomposition level should be
        % written as bit stream header.
        out(1,[1 2 3]) = [size(m,1) n_max level];
        bitctr = bitctr + 24;
        index = 4;
        Bits_Header = Bits_Header + 24;

        %-----------   Initialize LIP, LSP, LIS   ----------------
        temp = [];
        bandsize = 2.^(log2(size(m, 1)) - level + 1);
        temp1 = 1 : bandsize;
        for i = 1 : bandsize
            temp = [temp; temp1];
        end
        
        LIP(:, 1) = temp(:);
    
        temp = temp';
        LIP(:, 2) = temp(:);

        LIS(:, 1) = LIP(:, 1);
        LIS(:, 2) = LIP(:, 2);
        LIS(:, 3) = zeros(length(LIP(:, 1)), 1);

        pstart = 1;
        pend = bandsize / 2;

        for i = 1 : bandsize / 2
            LIS(pstart : pend, :) = [];
            pdel = pend - pstart + 1;
            pstart = pstart + bandsize - pdel;
            pend = pend + bandsize - pdel;
        end
        LSP = [];

        n = n_max;
%%{
        %-----------   coding   ----------------
     %   while(bitctr < max_bits)

            % Sorting Pass
            LIPtemp = LIP; temp = 0;
            for i = 1:size(LIPtemp,1)
                temp = temp+1;
                if (bitctr + 1) >= max_bits
                    if (bitctr < max_bits)
                        out(length(out))=[];
                    end
                    return
                end
                if abs(m(LIPtemp(i,1),LIPtemp(i,2))) >= 2^n % 1: positive; 0: negative
                    out(index) = 1; 
                    bitctr = bitctr + 1;
                    index = index +1; 
                    Bits_LIP = Bits_LIP + 1;
                    sgn = m(LIPtemp(i,1),LIPtemp(i,2))>=0;
                    out(index) = sgn; bitctr = bitctr + 1;
                    index = index +1; Bits_LIP = Bits_LIP + 1;
                    LSP = [LSP; LIPtemp(i,:)];
                    LIP(temp,:) = []; temp = temp - 1;
                else
                    out(index) = 0; bitctr = bitctr + 1;
                    index = index +1;
                    Bits_LIP = Bits_LIP + 1;
                end
            end

            LIStemp = LIS; temp = 0; i = 1;
            while ( i <= size(LIStemp,1))
                temp = temp + 1;
                if LIStemp(i,3) == 0
                    if bitctr >= max_bits
                        return
                    end
                    max_d = func_MyDescendant(LIStemp(i,1),LIStemp(i,2),LIStemp(i,3),m);
                    if max_d >= 2^n
                        out(index) = 1; bitctr = bitctr + 1;
                        index = index +1; Bits_LIS = Bits_LIS + 1;
                        x = LIStemp(i,1); y = LIStemp(i,2);

                        if (bitctr + 1) >= max_bits
                            if (bitctr < max_bits)
                                out(length(out))=[];
                            end
                            return
                        end
                        if abs(m(2*x-1,2*y-1)) >= 2^n
                            LSP = [LSP; 2*x-1 2*y-1];
                            out(index) = 1; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                            sgn = m(2*x-1,2*y-1)>=0;
                            out(index) = sgn; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                        else
                            out(index) = 0; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                            LIP = [LIP; 2*x-1 2*y-1];
                        end

                        if (bitctr + 1) >= max_bits
                            if (bitctr < max_bits)
                                out(length(out))=[];
                            end
                            return
                        end
                        if abs(m(2*x-1,2*y)) >= 2^n
                            LSP = [LSP; 2*x-1 2*y];
                            out(index) = 1; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                            sgn = m(2*x-1,2*y)>=0;
                            out(index) = sgn; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                        else
                            out(index) = 0; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                            LIP = [LIP; 2*x-1 2*y];
                        end

                        if (bitctr + 1) >= max_bits
                            if (bitctr < max_bits)
                                out(length(out))=[];
                            end
                            return
                        end
                        if abs(m(2*x,2*y-1)) >= 2^n
                            LSP = [LSP; 2*x 2*y-1];
                            out(index) = 1; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                            sgn = m(2*x,2*y-1)>=0;
                            out(index) = sgn; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                        else
                            out(index) = 0; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                            LIP = [LIP; 2*x 2*y-1];
                        end

                        if (bitctr + 1) >= max_bits
                            if (bitctr < max_bits)
                                out(length(out))=[];
                            end
                            return
                        end
                        if abs(m(2*x,2*y)) >= 2^n
                            LSP = [LSP; 2*x 2*y];
                            out(index) = 1; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                            sgn = m(2*x,2*y)>=0;
                            out(index) = sgn; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                        else
                            out(index) = 0; bitctr = bitctr + 1;
                            index = index +1; Bits_LIS = Bits_LIS + 1;
                            LIP = [LIP; 2*x 2*y];
                        end

                        if ((2*(2*x)-1) < size(m) & (2*(2*y)-1) < size(m))
                            LIS = [LIS; LIStemp(i,1) LIStemp(i,2) 1];
                            LIStemp = [LIStemp; LIStemp(i,1) LIStemp(i,2) 1];
                        end
                        LIS(temp,:) = []; temp = temp-1;

                    else
                        out(index) = 0; bitctr = bitctr + 1;
                        index = index +1; Bits_LIS = Bits_LIS + 1;
                    end
                else
                    if bitctr >= max_bits
                        return
                    end
                    max_d = func_MyDescendant(LIStemp(i,1),LIStemp(i,2),LIStemp(i,3),m);
                    if max_d >= 2^n
                        out(index) = 1; bitctr = bitctr + 1;
                        index = index +1;
                        x = LIStemp(i,1); y = LIStemp(i,2);
                        LIS = [LIS; 2*x-1 2*y-1 0; 2*x-1 2*y 0; 2*x 2*y-1 0; 2*x 2*y 0];
                        LIStemp = [LIStemp; 2*x-1 2*y-1 0; 2*x-1 2*y 0; 2*x 2*y-1 0; 2*x 2*y 0];
                        LIS(temp,:) = []; temp = temp - 1;
                    else
                        out(index) = 0; bitctr = bitctr + 1;
                        index = index +1; Bits_LIS = Bits_LIS + 1;
                    end
                end
                i = i+1;
            end

            % Refinement Pass
            temp = 1;
            value = floor(abs(2^(n_max-n+1)*m(LSP(temp,1),LSP(temp,2))));
            while (value >= 2^(n_max+2) & (temp <= size(LSP,1)))
                if bitctr >= max_bits
                    return
                end
                s = bitget(value,n_max+2);
                out(index) = s; bitctr = bitctr + 1;
                index = index +1; Bits_LSP = Bits_LSP + 1;
                temp = temp + 1;
                if temp <= size(LSP,1)
                    value = floor(abs(2^(n_max-n+1)*m(LSP(temp,1),LSP(temp,2))));
                end
            end

            n = n - 1;
      %  end

        bit_str_1=out;
       
        %=============
%%}     
        fprintf('nyampe bawah ');
        %BIT_STR(k) = {bit_str_1};
    end
    
    fprintf(' [OK]\n');
    save('var.mat');
   
%toc
    
end