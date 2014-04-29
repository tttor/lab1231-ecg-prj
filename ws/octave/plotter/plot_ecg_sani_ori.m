%% init
clear all
close all

root_dataset_path = '~/lab1231-ecg-prj/dataset/';

sani_dataset_path = 'sani-exp/2D/first3min/128/none/';
addpath (strcat(root_dataset_path,sani_dataset_path));

incartdb_dataset_path = 'incartdb/';
addpath (strcat(root_dataset_path,incartdb_dataset_path));

%% load data ecg
load('I12m.mat');
data_ori = val;clear val;

load('I12m-128-8.1-out.mat');
data_CR_8 = recg8_pre; clear recg8_pre;

load('I12m-128-16.1-out.mat');

data_CR_16 = recg8_pre; clear recg8_pre;

load('I12m-128-24.1-out.mat');

data_CR_24 = recg8_pre; clear recg8_pre;

%jumlah sample
sampling_rate = 256;
time_second = 5;
n_sample = sampling_rate*time_second;
lead = 2;

%ploting
data_ori = data_ori/306;
plot1 = plot (data_ori(2,1:n_sample))
title ('I12m')

hold on
plot1 = plot (data_CR_8(2,1:n_sample),'g')
title ('I12m')


