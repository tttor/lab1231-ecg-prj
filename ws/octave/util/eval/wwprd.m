function [ wwprd_out ] = wwprd ( ecg_ori, ecg_rec, weight )
% Wavelet based weighted PRD proposed by Fahoum, 2006
    
    wv_name = 'db2';
    N = 5;
    weight  = 1; %(1:heuristic; 2: normalized);
    % Heuristic weight
    %     A5   D5   D4   D3   D2   D1
    w = [6/27 9/27 7/27 3/27 1/27 1/27];
    
    n = min(size(ecg_ori,2),size(ecg_rec,2));
    
    
    [Ci,L] = wavedec(ecg_ori(1:n), N, wv_name);
    [Cr,L] = wavedec(ecg_rec(1:n), N, wv_name);
    
    % WPRD calculation
    Ai = appcoef(Ci,L,wv_name,N);
    Ar = appcoef(Cr,L,wv_name,N);
    wprd_arr(1) = wprd(Ai,Ar);
    for k = 2:N+1
        Di = detcoef(Ci,L,k-1);
        Dr = detcoef(Cr,L,k-1);
        wprd_arr(k) = wprd(Di,Dr);
    end
    
    % WWPRD calculation
    wwprd_out = 0;
    for k = 1:N+1
        wwprd_out = wwprd_out + w(k) * wprd_arr(N-k+2);
    end
    
end

