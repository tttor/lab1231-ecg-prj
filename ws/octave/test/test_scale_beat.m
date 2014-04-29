function test_scale_beat( ecg_in, rr_ann_in, scaled )
    ecg_scl = scale_beat(ecg_in,rr_ann_in,scaled);
    recg    = unscale_beat(ecg_scl,rr_ann_in,scaled);
    prd(ecg_in,recg,0)
    t = 1:size(ecg_in,2);
    plot(t,ecg_in,t,recg);
end

