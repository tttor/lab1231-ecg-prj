% % This is Differential Decoding function

% % INPUT/OUTPUT
% % v_in: differentially encode input vector
% % v_out: decoded ouput vecor

% % See also diffencodevec.m

function v_out = diff_dec (v_in)
[r c]=size(v_in);

if r>1 && c>1
    error('input must be a vector');
end

v_out=[];
if isempty (v_in)
    return
end

v_out(1) = v_in(1);

for k = 2:length(v_in)
    v_out(k) = v_out(k-1) + v_in(k);
end

if c == 1 %v_in row vector
    v_out = v_out'; %convert output to same type of vector
end

% % % --------------------------------
% % % Author: Dr. Murtaza Khan
% % % Email : drkhanmurtaza@gmail.com
% % % --------------------------------