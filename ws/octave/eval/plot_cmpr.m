function plot_cmpr(ecg, recg, duration, out_dir)
    % Overlay original and reconstructed ecg signal also
    % plot the residual signal
    
    split_fig = 0; 
    
    ymin = -1;
    ymax = 2;
    
    sampling_rate = 257;
    t_min = 1;
    t_max = sampling_rate*duration;
    t = t_min:t_min + t_max;

    n_lead = 8;
    for k= 1:n_lead
        local_ecg = ecg(k, :);
        local_recg = recg(k, :);
        
        figure('color','w');
        
        subplot(2,1,1); 
        plot(t,local_ecg(t),'r',t,local_recg(t),'b');
        legend('Original','Reconstructed');
        legend('boxoff');
        ylim('manual');
        ylim([ymin,ymax]);

        subplot(2,1,2); 
        plot(t,local_ecg(t)-local_recg(t));
        legend('Error');
        legend('boxoff');
        ylim('manual');
        ylim([ymin,ymax]);
        
        [ax4,h3]=suplabel(strcat('Lead ith= ', int2str(k)) ,'t');
        set(h3,'FontSize', 15)
       
        filename = strcat(out_dir,'eval/cmpr_lead_', int2str(k));
        saveas(gcf, filename, 'pdf');
        close gcf;
    end
end

