function retval = spectralradius(A)
        retval = max(abs(eig(A)));
endfunction
