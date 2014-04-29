function plot_ecg(ecg_in, base)
    t = 1:size(ecg_in,2);
    if(isempty(base))
        subplot(4,2,1); plot(ecg_in(1,:));
        subplot(4,2,2); plot(ecg_in(2,:));
        subplot(4,2,3); plot(ecg_in(3,:));
        subplot(4,2,4); plot(ecg_in(4,:));
        subplot(4,2,5); plot(ecg_in(5,:));
        subplot(4,2,6); plot(ecg_in(6,:));
        subplot(4,2,7); plot(ecg_in(7,:));
        subplot(4,2,8); plot(ecg_in(8,:));
    else
        subplot(4,2,1); plot(t,ecg_in(1,:),t,base(1,:));
        subplot(4,2,2); plot(t,ecg_in(2,:),t,base(2,:));
        subplot(4,2,3); plot(t,ecg_in(3,:),t,base(3,:));
        subplot(4,2,4); plot(t,ecg_in(4,:),t,base(4,:));
        subplot(4,2,5); plot(t,ecg_in(5,:),t,base(5,:));
        subplot(4,2,6); plot(t,ecg_in(6,:),t,base(6,:));
        subplot(4,2,7); plot(t,ecg_in(7,:),t,base(7,:));
        subplot(4,2,8); plot(t,ecg_in(8,:),t,base(8,:));
    end;
end