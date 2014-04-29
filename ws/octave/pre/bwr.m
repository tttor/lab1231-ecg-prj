function [ecg_out, base] = bwr(ecg_in, wv_name, N, verbose)
% Remove baseline wander using Sargolzaei approach

    [c,l] = wavedec(ecg_in,N,wv_name);
    en = zeros(N);
    for k=1:N
        % wavelet energy = norm
        en(k) = norm(detcoef(c,l,k));
        if k>2
            % local min detection
            if (en(k-2) > en(k-1)) & (en(k-1) < en(k))
                l_min = k-1;
                break
            end
        end
    end
    %plot(en);
    %xlabel('Decomposition level'), ylabel('Detail Coef. Energy');
    base = wrcoef('a',c,l,wv_name,l_min-1);
    if verbose == 1
        fprintf('\nBaseline wander removal : [OK]\n');
    end
    ecg_out = ecg_in-base;
end