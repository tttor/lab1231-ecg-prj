% % This is Differential Encoding function

% % INPUT/OUTPUT
% % vi: input vector
% % vo: differentially encoded ouput vector

% % See also diffdecodevec.m

function vo=diffencodevec(vi)
[r c]=size(vi);

if r>1 && c>1
    error('input must be a vector');
end

vo=[];
if length(vi)==0
    return
end

vo=linspace(0,0,length(vi+1));
vo(1)=vi(1);
vo(2:end)=vi(2:end)-vi(1:end-1);


if c==1 %vi row vector
    vo=vo'; %convert output to same type of vector
end
    
% % % --------------------------------
% % % Author: Dr. Murtaza Khan
% % % Email : drkhanmurtaza@gmail.com
% % % --------------------------------



