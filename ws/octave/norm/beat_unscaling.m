
function [ ecg_out ] = unscale_beat ( ecg_in, r_ann )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
    
    %scaled  = 128;
    scaled  = 256;
    padding = 1;
    ecg_out = [];
    beat_num = size(r_ann,2);
    for k = 1:beat_num-1
        beat_dur  = r_ann(1,k);
        if k == 1
            start_idx = 1;
            end_idx   = scaled + padding; 
            us_beat   = resample(ecg_in(start_idx:end_idx),beat_dur + padding,scaled + padding,2);
            us_beat   = us_beat(1:beat_dur);  
        else
            start_idx = (k-1) * scaled - padding + 1;
            end_idx   = start_idx + scaled + padding + 1; 
            us_beat   = resample(ecg_in(start_idx:end_idx),beat_dur + 2*padding,scaled + 2*padding,2);
            us_beat   = us_beat(1 + padding:beat_dur + padding);
        end
        ecg_out   = [ecg_out us_beat];
    end
    beat_dur  = r_ann(1,beat_num);
    start_idx = (beat_num-1) * scaled - padding + 1;
    end_idx   = beat_num * scaled;
    us_beat   = resample(ecg_in(start_idx:end_idx),beat_dur + padding,scaled + padding,2);
    us_beat   = us_beat(1 + padding:beat_dur + padding);
    ecg_out   = [ecg_out us_beat];
end

%{ 
Seharusnya input fungsi ini berasal dari sinyal hasil
resample ke 128++ (ecg21,ecg22) bukan hasil cropping 
berukuran 128 sampel (ecg21a,ecg22a)

ecg11 = ecg_lpf(1,1:116);
ecg21 = resample(ecg11,130,116,2);
ecg21a = ecg21(1:128);
ecg31 = resample(ecg21,116,130,2);
ecg31a = ecg31(1:114);
plot(t,ecg11(1:128),t,ecg31a);
prd(ecg11(1:114),ecg31a)
ans =

    0.6377

ecg12 = ecg_lpf(1,113:279);
ecg22 = resample(ecg12,132,167,2);
ecg22a = ecg22(3:130);
ecg32 = resample(ecg22,167,132,2);
ecg32a = ecg32(3:165);
plot(t,ecg12(3:165),t,ecg32a);
prd(ecg12(3:165),ecg32a)

ans =

    0.6129
%}