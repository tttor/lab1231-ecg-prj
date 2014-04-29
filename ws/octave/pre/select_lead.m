function [ ecg8 ] = select_lead( ecg12 )
% Select 8 of 12 lead ECG
%
%   lead     idx
%     I       1
%    II       2
%    V1       3
%    V2       4
%    V3       5
%    V4       6
%    V5       7
%    V6       8
%
    ecg8(1,:) = ecg12(1,:);
    ecg8(2,:) = ecg12(2,:);
    ecg8(3,:) = ecg12(7,:);
    ecg8(4,:) = ecg12(8,:);
    ecg8(5,:) = ecg12(9,:);
    ecg8(6,:) = ecg12(10,:);
    ecg8(7,:) = ecg12(11,:);
    ecg8(8,:) = ecg12(12,:);
end

