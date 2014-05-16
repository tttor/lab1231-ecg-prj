function [ all_prd_table ] = get_table_PRD
%GET_TABLEII Summary of this function goes here
%   Detailed explanation goes here
% @author : GrafikaJ
% need : - file data asli exp: I01m.mat
%        - file data hasil running exp: I01-128-4.1-out.mat
%                                       rec_name-sample_num-CR.repeat.mat
%



    group = 0; % apply lead grouping?
    order = 0; % apply beat reordering?
    res   = 0; % apply residual calculation?
    sample_num = 128; % number of sample per beat on normalized ECG
    repeat = 1;
    
    rec_name = {'I01m','I02m','I03m','I04m','I05m','I06m','I07m','I08m','I09m','I10m',...
                'I11m','I12m','I13m','I14m','I15m','I16m','I17m','I18m','I19m','I20m',...
                'I21m','I22m','I23m','I24m','I25m','I26m','I27m','I28m','I29m','I30m',...
                'I31m','I32m','I33m','I34m','I35m','I36m','I37m','I38m','I39m','I40m',...
                'I41m','I42m','I43m','I44m','I45m','I46m','I47m','I48m','I49m','I50m',...
                'I51m','I52m','I53m','I54m','I55m','I56m','I57m','I58m','I59m','I60m',...
                'I61m','I62m','I63m','I64m','I65m','I66m','I67m','I68m','I69m','I70m',...
                'I71m','I72m','I73m','I74m','I75m'};
    
    %{
    proc_list = [1 8 9 12 13 14 18 19 22 27 33 34 36 39 42 43];
    CR = [4 8 12 16 20 24];
   %}
    
    %buat ujicoba             
    proc_list = [1];
    CR = [4];
    
    
    res_prd_all = 0;
    mean_all_prd_table =0;
    
  % fprintf('%i\n',length(CR)*length(proc_list));
    
    
    %get prd for all record in proc_list and all CR 
   
    for k = 1:length(CR)
        %fprintf('%i\n',CR(k));
        res_prd_all = 0;
        for j = 1:length(proc_list)
         %   fprintf('%s\n',cell2mat(rec_name(proc_list(j))));
            ecg  = load_ori_signal(cell2mat(rec_name(proc_list(j))));
            recg = load_rec_signal(cell2mat(rec_name(proc_list(j))),sample_num,CR(k),repeat,group,order,res);
            res_prd_all = res_prd_all + prd_all(recg, ecg, 1);
            
           
            
        end    
        mean_all_prd_table = mean_all_prd_table+res_prd_all;
        all_prd_table(k) = res_prd_all/length(proc_list);
       % fprintf(' prd_all %6.2f \n',all_prd_table(k));
       
    end
    
    all_prd_table(k+1) = mean_all_prd_table/(length(CR)*length(proc_list));
    
    
    
end

