function min_prd(sample_num, group, order, res)
% Calculate minimum PRD from 10 experiments

    base = 0;
    gain = 306;

    sample_num = 128;
    repeat     = 10;

    rec_name = {'I01m','I02m','I03m','I04m','I05m','I06m','I07m','I08m','I09m','I10m',...
                'I11m','I12m','I13m','I14m','I15m','I16m','I17m','I18m','I19m','I20m',...
                'I21m','I22m','I23m','I24m','I25m','I26m','I27m','I28m','I29m','I30m',...
                'I31m','I32m','I33m','I34m','I35m','I36m','I37m','I38m','I39m','I40m',...
                'I41m','I42m','I43m','I44m','I45m','I46m','I47m','I48m','I49m','I50m',...
                'I51m','I52m','I53m','I54m','I55m','I56m','I57m','I58m','I59m','I60m',...
                'I61m','I62m','I63m','I64m','I65m','I66m','I67m','I68m','I69m','I70m',...
                'I71m','I72m','I73m','I74m','I75m'};
            
    proc_list = [1 8 9 12 13 14 18 19 22 27 33 34 36 39 42 43];
    CR = [8 16 24 32 40 48 56 64];
    
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
    
    min_prd_arr = zeros(size(proc_list,2),size(CR,2));
    
    row = 0;
    for k = proc_list
        row = row + 1;
        f_name = [char(rec_name(k)) '.mat'];
        fprintf('%s \n',f_name);
        ecg12_stc = load(f_name);
        ecg12     = ecg12_stc.val;
        ecg8      = select_lead(ecg12);
        ecg8_pre  = preprocess_sig(ecg8,base,gain);
        for l = 1:size(CR,2)
            min     = realmax;
            min_idx = 1;
            for m = 1:repeat
                rf_name = [char(rec_name(k)) '-' num2str(sample_num) '-' ...
                          num2str(CR(l)) '.' num2str(m) group_fname order_fname ...
                          res_fname '-out.mat'];
                recg8_stc = load(rf_name);
                recg8_pre = recg8_stc.recg8_pre;
                prd1 = prd_all(recg8_pre,ecg8_pre,1);
                if prd1 < min
                    min = prd1;
                    min_idx = m;
                end
            end
            min_prd_arr(row,l) = min;
        end
    end
    out_fname = [num2str(sample_num) group_fname order_fname res_fname ...
                 '-min-prd.txt'];
    save(out_fname,'min_prd_arr','-ascii','-tabs');
end

