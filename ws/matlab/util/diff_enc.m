% % This is Differential Encoding function

% % INPUT/OUTPUT
% % v_in: input vector
% % v_out: differentially encoded ouput vector

% % See also diffdecodevec.m

function v_out = diff_enc (v_in)

[r c] = size(v_in);

if r>1 && c>1
    error('input must be a vector');
end

v_out = [];
if isempty(v_in)
    return
end

v_out        = linspace(0,0,length(v_in+1));
v_out(1)     = v_in(1);
v_out(2:end) = v_in(2:end)-v_in(1:end-1);


if c == 1 %v_in row vector
    v_out = v_out'; %convert output to same type of vector
end
    
% % % --------------------------------
% % % Author: Dr. Murtaza Khan
% % % Email : drkhanmurtaza@gmail.com
% % % --------------------------------



