% Peak Signal-to-Noise Ratio calculations
% PSNR [SNR] = psnr(Original, Compressed)
%
% INPUT: Original - original image
% Compressed - compressed or altered image
% OUTPUT: PSNR - Peak Signal-to-Noise Ratio, in DB
% SNR - RMS (root mean square) signal-to-noise ratio,
% if specified
%
% Based on code by Chris Shoemaker
%
% Part of "Watermark embedding using asymetric keys" project
%
% (c) 2004 Geruta Kazakaviciute, <geruta@delfi.lt>
% Eugenijus Januskevicius, <ejs@delfi.lt>
%

function [p_snr, varargout] = psnr(Original, Compressed)

     % convert to doubles
     Compressed=double(Compressed);
     Original=double(Original);

     [M,N]=size(Compressed);

     % avoid divide by zero nastiness
     if ((sum(sum(Compressed-Original))) == 0)
         error('Input vectors must not be identical')
     else
          % calculate SNR numerator
         snr_num=sum(sum(Compressed.^2));
         psnr_num=255^2; % calculate PSNR numerator
          % calculate SNR denominator
         snr_den=sum(sum((Compressed-Original).^2));
         psnr_den=(1/(N*M)) * snr_den; % calculate PSNR denominator
            % calculate SNR
         snr=sqrt(snr_num/snr_den);
         p_snr = -(10 * log10( psnr_num / psnr_den));
         if nargout == 2
             varargout(1) = {snr};
         end
     end
return