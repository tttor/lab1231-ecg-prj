function ecg8 = load_signal( rec_name, sample_num, CR, repeat_idx, group, order, res)
% load saved multilead ecg signal

    if group == 1
        group_fname = '-group';
    else
        group_fname = '';
    end

    if order == 1
        order_fname = '-order';
    else
        order_fname = '';
    end

    if res == 1
        res_fname = '-res';
    else
        res_fname = '';
    end
    
    f_name = [rec_name '-' num2str(sample_num) '-' num2str(CR) '.' ....
             num2str(repeat_idx) group_fname order_fname res_fname '-out.mat'];
    
    ecg8_stc = load(f_name);
    ecg8 = ecg8_stc.recg8_pre;
end

