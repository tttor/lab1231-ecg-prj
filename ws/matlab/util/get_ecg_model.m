function [ ecg_model, ecg_mat_res ] = get_ecg_res( ecg_mat, c_idx )
% Calculate ECG model for a cluster of beats using wavelet approach

    clust_num = size(c_idx,1);
    for k = 1:clust_num
        beat_idx  = cell2mat(c_idx(k));
        ecg_clust = ecg_mat(beat_idx,:);
        %min_amp   = min(ecg_clust(:));
        %ecg_clust = ecg_clust - min_amp;
        ecg_model = mean(ecg_clust,1);
        hold on;
        for l = 1:size(ecg_clust,1)
            for m = 1:size(ecg_clust,2)
                ecg_mat_res(l,m) = ecg_clust(l,m) - ecg_model(m);
            end
            %plot(ecg_clust(k,:),'b');
            plot(ecg_res(l,:),'g');
        end
        plot(ecg_model,'r');
        hold off;
    end
end

