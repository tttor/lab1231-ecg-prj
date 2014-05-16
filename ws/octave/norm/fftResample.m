% FFTRESAMPLE Resample a real signal by the ratio p/q
function y = fftResample(x,p,q)

% --- take FFT of signal ---
f = fft(x);

% --- resize in the FFT domain ---
% add/remove the highest frequency components such that len(f) = len2
len1 = length(f);
len2 = round(len1*p/q);
lby2 = 1+len1/2;
if len2 < len1
  % remove some high frequency samples
  d = len1-len2;
  f = f(:);
  f(floor(lby2-(d-1)/2:lby2+(d-1)/2)) = [];
elseif len2 > len1
  % add some high frequency zero samples
  d = len2-len1;
  lby2 = floor(lby2);
  f = f(:);
  f = [f(1:lby2); zeros(d,1); f(lby2+1:end)];
end

% --- take the IFFT ---
% odd number of sample removal/addition may make the FFT of a real signal
% asymmetric and artificially introduce imaginary components - we take the
% real value of the IFFT to remove these, at the cost of not being able to
% reample complex signals
y = real(ifft(f))';