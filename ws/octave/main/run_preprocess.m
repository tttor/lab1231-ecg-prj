clc;
rec_name = {'I01m'};
fprintf('%s\n',cell2mat(rec_name(1)));
load(char(rec_name(1)));
ecg_in = val;

rr_fname = [char(rec_name(1)) '-rr.txt'];
load(rr_fname,'-ASCII');
rr_vname  = [char(rec_name(1)) '_rr'];
RR_ANN_IN = eval(rr_vname);

sample_num = 128;
CR = 4;
group = 0;
order = 0;
res = 0;
rep = 1;

preprocess_2 (ecg_in, RR_ANN_IN, sample_num, CR)
