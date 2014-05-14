function comp_multi_signal( rec_name, CR, group, order, res)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
   repeat = 3;
   for lead = 1:8
        min = realmax;
        min_idx = 0;
        for k = 1:repeat
            prd = comp_signal(rec_name,lead,128,CR,k,group,order,res,10);
            if prd < min
                min = prd;
                min_idx = k;
            end
        end
        fprintf('%6.2f index = %i\n',min,min_idx);
    end
end