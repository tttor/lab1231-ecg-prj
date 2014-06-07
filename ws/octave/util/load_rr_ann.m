function rr_ann = load_rr_ann(rec_dir,rec_name) 
    rr_fname = [rec_name '-rr.txt'];
    load( strcat(rec_dir,rr_fname),'-ASCII' );
    
    rr_vname  = [rec_name '_rr'];
    rr_ann = eval(rr_vname);
end
    
