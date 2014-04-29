function codec_all

%setting path octave only
%root_dataset_path = '~/lab1231-ecg-prj/dataset/';
%incartdb_dataset_path = 'incartdb/';
%addpath (strcat(root_dataset_path,incartdb_dataset_path));


tic;

    group = 0; % apply lead grouping?
    order = 0; % apply beat reordering?
    res   = 0; % apply residual calculation?
    sample_num = 128; % number of sample per beat on normalized ECG
    
    %rec_name = {'I01m3min','I12m3min','I12m10sec'};
    rec_name = {'I12m'};
    
    %proc_list = [1 8 9 12 13 14 18 19 22 27 33 34 36 39 42 43]
    %new_proc_list = [5! 6! 10 11 15 16! 20! 21! 23! 24 25! 26! 28! 29! 30! 
                   %  31 32! 37! 38 40! 41! 45! 46! 47! 48! 49! 50! 51 52!
                   %  53 54 55 56! 59! 60! 61! 62! 63 64! 65 66!];
                   
    %proc_list = [10 11 15 24 31 38 51 53 54 55 63 65];
    %hang on records 51 53
    proc_list = [1];
    
    %CR = [4 8 12 16 20 24 28 32 36 40];
    CR = [8];
    
    if order == 1
        repeat = 3; % reduce variation of random initialization on clustering
    else
        repeat = 1;
    end
    
    for k = proc_list
        try
            clc;
            fprintf('%s\n',cell2mat(rec_name(k)));
            load(char(rec_name(k)));
            ecg_in = val;
            rr_fname = [char(rec_name(k)) '-rr.txt'];
            load(rr_fname,'-ASCII');
            rr_vname  = [char(rec_name(k)) '_rr'];
            rr_ann_in = eval(rr_vname);    
            
            prd_arr = [];
            min_arr = [];
            for l = 1:size(CR,2);
                min     = realmax;
                min_idx = 1;
                sigma = 0;
                for m = 1:repeat
                    fprintf('\nRepeat : %i\n',m);
                    prd1 = codec(char(rec_name(k)),ecg_in,rr_ann_in,sample_num, CR(l),group,order,res,m);
                    if prd1 < min
                        min = prd1;
                        min_idx = m;
                    end
                    sigma = sigma + prd1;
                end
                prd_mean = sigma/repeat;
                %prd_arr = [prd_arr prd_mean];
                prd_arr = [prd_arr min];
                min_arr = [min_arr min_idx];
            end
            %prd_arr
            prd_arr = [prd_arr; min_arr]
            
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
            
            folder_name = 'C:\Users\asus\Documents\GitHub\lab1231-ecg-prj\ws\octave\result-exp\';
            f_name = [ folder_name 'G-' char(rec_name(k)) '-' num2str(sample_num) ...
                      group_fname order_fname res_fname '-prd.txt'];
            save(f_name,'prd_arr','-ascii','-tabs');
            
        catch err
            rethrow(err);
        end
    end 
    
toc;
    
end


