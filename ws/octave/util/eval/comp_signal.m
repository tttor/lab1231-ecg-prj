function prd_out = comp_signal( rec_name, lead_num, sample_num, CR, repeat_idx, group, order, res, time )
% Overlay original and reconstructed ecg signal also
% plot the residual signal
    
    show_plot = 1;
    split_fig = 1; 
    
    ymin = -1;
    ymax = 4;

    ecg  = load_ori_signal(rec_name);
    ecg  = ecg(lead_num,:);
    %ecg  = mat2gray(ecg(lead_num,:));
    recg = load_rec_signal(rec_name,sample_num,CR,repeat_idx,group,order,res);
    recg = recg(lead_num,:);
    %recg = mat2gray(recg(lead_num,:));
    
    sampling_rate = 257;
    t_min = 1;
    t_max = sampling_rate*time; %2570;
    %t_max = size(ecg,2) - 1;
    
    t = t_min:t_min + t_max;

    prd_out = wwprd(ecg(t),recg(t),1);
    
    if show_plot
        figure('color','w');
        if split_fig
            subplot(3,1,1); plot(t,ecg(t),'k');
            legend('Original');
            legend('boxoff');
            ylim('manual');
            ylim([ymin,ymax]);
            
            subplot(3,1,2); plot(t,recg(t),'k');
            legend('Reconstructed');
            legend('boxoff');
            ylim('manual');
            ylim([ymin,ymax]);

            subplot(3,1,3); plot(t,ecg(t)-recg(t),'k');
            legend('Error');
            legend('boxoff');
            ylim('manual');
            ylim([ymin,ymax]);
        else
            subplot(2,1,1); plot(t,ecg(t),'r',t,recg(t),'b');
            legend('Original','Reconstructed');
            legend('boxoff');
            ylim('manual');
            ylim([ymin,ymax]);

            subplot(2,1,2); plot(t,ecg(t)-recg(t));
            legend('Error');
            legend('boxoff');
            ylim('manual');
            ylim([ymin,ymax]);
        end
    end
end

