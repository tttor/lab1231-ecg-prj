function [ sign ] = comp_norm( x1, x2 )
% compare 2 column vector based on its norm, return 1 if |x1| > |x2|, -1 if
% |x1| < |x2|, and 0 if |x1| = |x2|

    n1 = norm(x1);
    n2 = norm(x2);

    if n1 > n2
        sign = 1
    elseif n1 < n2
        sign = -1
    else
        sign = 0
    end
end

