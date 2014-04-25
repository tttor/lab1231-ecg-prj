function dataOut = quicksort(data)
    lenD = size(data,1);
    ind = cast(floor(lenD/2),'uint8');
    j = 1;
    k = 1;
    L = [];
    R = [];
    if(lenD < 2)
      dataOut = data;
    else
      pivot = data(ind,:);
      for i=1:lenD
        if(i~=ind)
          if(norm(data(i)) < norm(pivot))
            L(j,:) = data(i,:);
            j = j+1;
          else
            R(k,:) = data(i,:);
            k = k+1;
          end
          endx1
      end
      L = quicksort(L);
      R = quicksort(R);
      dataOut = [L; pivot; R];
      whos
end