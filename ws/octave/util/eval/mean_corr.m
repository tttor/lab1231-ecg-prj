function [ corr_out ] = mean_corr
% Calculate mean of coefficient correlation matrix from all ECG record

tic;

    rec_name = {'I01m','I02m','I03m','I04m','I05m','I06m','I07m','I08m','I09m','I10m',...
                'I11m','I12m','I13m','I14m','I15m','I16m','I17m','I18m','I19m','I20m',...
                'I21m','I22m','I23m','I24m','I25m','I26m','I27m','I28m','I29m','I30m',...
                'I31m','I32m','I33m','I34m','I35m','I36m','I37m','I38m','I39m','I40m',...
                'I41m','I42m','I43m','I44m','I45m','I46m','I47m','I48m','I49m','I50m',...
                'I51m','I52m','I53m','I54m','I55m','I56m','I57m','I58m','I59m','I60m',...
                'I61m','I62m','I63m','I64m','I65m','I66m','I67m','I68m','I69m','I70m',...
                'I71m','I72m','I73m','I74m','I75m'};
    
    rec_num  = size(rec_name,2);
    ecg_corr = zeros(8,8);
    
    %{
    for k = 1:rec_num
        ecg_name = char(rec_name(k));
        load(ecg_name);
        ecg8 = select_lead(val);
        ecg_corr =  ecg_corr + corrcoef(ecg8');
    end
    corr_out = ecg_corr/rec_num;
    %}
    for k = 1:rec_num
        ecg_name = char(rec_name(k));
        f_name   = [ecg_name '.txt'];
        save(f_name,'k','-ascii','-tabs');
    end
    
toc

end

