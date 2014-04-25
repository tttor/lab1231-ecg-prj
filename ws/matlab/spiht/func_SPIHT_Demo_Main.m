function func_SPIHT_Main
% Matlab implementation of SPIHT (without Arithmatic coding stage)
%
% Main function 
%
% input:    Orig_I : the original image.
%           rate : bits per pixel
% output:   img_spiht 
%

fprintf('-----------   Welcome to SPIHT Matlab Demo!   ----------------\n');

fprintf('-----------   Load Image   ----------------\n');
infilename = 'beat.bmp';
outfilename = 'beat_reconstruct.bmp';

%Orig_I = double(imread(infilename));
global ecg_mat_clu;
Orig_I = double(ecg_mat_clu);

%rate = 1;
rate = 0.5;

OrigSize = size(Orig_I, 1);
max_bits = floor(rate * OrigSize^2);
OutSize = OrigSize;
image_spiht = zeros(size(Orig_I));
[nRow, nColumn] = size(Orig_I);

fprintf('done!\n');
fprintf('-----------   Wavelet Decomposition   ----------------\n');
n = size(Orig_I,1);
n_log = log2(n);
level = n_log;
% wavelet decomposition level can be defined by users manually.

type = 'bior4.4';
[Lo_D,Hi_D,Lo_R,Hi_R] = wfilters(type);

[I_W, S] = func_DWT(Orig_I, level, Lo_D, Hi_D);

fprintf('done!\n');

fprintf('-----------   Encoding   ----------------\n');
img_enc = func_SPIHT_Enc(I_W, max_bits, nRow*nColumn, level);   

fprintf('done!\n');
fprintf('-----------   Decoding   ----------------\n');
img_dec = func_SPIHT_Dec(img_enc);

fprintf('done!\n');
fprintf('-----------   Wavelet Reconstruction   ----------------\n');
img_spiht = func_InvDWT(img_dec, S, Lo_R, Hi_R, level);

fprintf('done!\n');
fprintf('-----------   PSNR analysis   ----------------\n');

imshow(img_spiht);
save 'img_spiht.mat' img_spiht;
imwrite(img_spiht, gray(256), outfilename, 'bmp');

Q = 255;
MSE = sum(sum((img_spiht-Orig_I).^2))/nRow / nColumn;
fprintf('The psnr performance is %.2f dB\n', 10*log10(Q*Q/MSE));

