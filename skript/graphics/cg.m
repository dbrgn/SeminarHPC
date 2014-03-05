

n = 3
A = [   1, 0.5,   0;
      0.5,   1, 0.5;
        0, 0.5,   1 ]
b = [  1;
      -1;
       1  ]
x = [ 1;
      0;
      0  ]

#n = 2
#A = [   1, 0.5;
#      0.5,   1 ]
#b = [  1;
#      -1  ]
#x = [  2;
#       0  ]

n = 50
A = eye(n) + 0.3 * (shift(eye(n), 1) + shift(eye(n), -1));
A(1,n) = 0;
A(n,1) = 0;
#A = eye(n) + 0.1 * rand(n, n)
#A = (A + A')/2
b = ones(n, 1);
x = zeros(n, 1);
x(1) = 1;

m = n;
V = zeros(n, m);
X = zeros(n, m+1);
X(:,1) = x;
e = zeros(1, m);

function retval = scalar(A, v, w) 
	retval = v' * A * w;
endfunction

function retval = l(A, v)
	retval = sqrt(scalar(A, v, v));
endfunction

for i = 1:m
	v = A * x - b;
	e(i) = norm(v);

if (1 == 0)
	for j = 1:(i-1)
		v = v - scalar(A, V(:, j), v) * V(:, j);
	endfor
else
	if (i > 1)
		v = v - scalar(A, V(:, i-1), v) * V(:, i-1);
	endif
endif

	v = v / l(A, v);
	V(:,i) = v;

	x = x - v * (v' * (A * x - b)) ;
	X(:,i+1) = x;
endfor

e;
e(m+1) = norm(A * x - b);
e'

#eig(A)
#norm(A) * norm(A^-1)
