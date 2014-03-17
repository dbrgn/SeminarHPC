function retval = diagonal(A)
        n = length(A);
        result = zeros(n, n);
        for i = (1:n)
                result(i, i) = A(i, i);
        endfor
        retval = result;
endfunction

