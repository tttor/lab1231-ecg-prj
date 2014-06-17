clc;
clear all;
format long;

init_ws;

wavelet_img = csvread('./main/out/wavelet-img/wavelet_img-1.csv');
%LIS_row = [19 32 1];% CPP: WANT: 0.069672026843151644 GOT: 0.069300503444524986
LIS_row =  [31 31 1];% CPP: WANT: 0.0022652670550645398 GOT: 0.0012210965237076799

i = LIS_row(1);
j = LIS_row(2);
type = LIS_row(3);

d = func_MyDescendant(i, j, type, wavelet_img);
fprintf('d= %.19f\n', d);


