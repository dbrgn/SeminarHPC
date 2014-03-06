#
# cgexample.m -- Beispiel zum CG Algorithmus.
#
# In diesem Beispiel wird der Fehler des CG-Algorithmus in Abh"angigkeit
# vom Parameter alpha bestimmt.
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#

# Berechnung der tridiagonal Matrix tridiag(alpha,1,alpha)

function retval = Aalpha(n, alpha) 
	A = eye(n) + alpha * (shift(eye(n), 1) + shift(eye(n), -1));
	A(1,n) = 0;
	A(n,1) = 0;
	retval = A;
endfunction

# Berechnung des Startwertes
function retval = x1(n)
	x = zeros(n, 1);
	x(1) = 1;
	retval = x;
endfunction

# Skalaprodukt bezueglich der Matrix A, fuer CG Algorithmus
function retval = scalar(A, v, w) 
	retval = v' * A * w;
endfunction

# Laengenmessung bezueglich der Matrix A, fuer CG Algorithmus
function retval = l(A, v)
	retval = sqrt(scalar(A, v, v));
endfunction

# Durchfuehrung des CG Algorithmus, Berechnung der Fehler
function retval = cgerrors(A, b, x)
	n = length(b);
	m = n;
	V = zeros(n, m);
	X = zeros(n, m+1);
	X(:,1) = x;
	e = zeros(1, m);
	for i = 1:m
		v = A * x - b;
		e(i) = norm(v);

		for j = 1:(i-1)
			v = v - scalar(A, V(:, j), v) * V(:, j);
		endfor

		v = v / l(A, v);
		V(:,i) = v;

		x = x - v * (v' * (A * x - b)) ;
		X(:,i+1) = x;
	endfor

	e;
	e(m+1) = norm(A * x - b);
	retval = e;
endfunction

# Konditionszahl der Matrix, fuer Vergleich
function retval = k2(A) 
	retval = norm(A) * norm(A^-1);
endfunction

n = 20;
s = 20;
fid = fopen("cgresults.csv", "w");
fprintf(fid, "j,alpha,k2,q,k,e\n");
E = zeros(n+1,s);
alpha = 0.5;
b = ones(n,1);
for j = (1:s)
	A = Aalpha(n, alpha);
	#printf("alpha = %f, det = %f\n", alpha, det(A));
	x = x1(n);
	e = cgerrors(A, b, x);
	E(:,j) = e';
	k2val = k2(A);
	q = (sqrt(k2val) - 1) / (sqrt(k2val) + 1);
	startfehler = e(1,1);
	for k = 1:s
		fprintf(fid, "%d,%f,%f,%f,%d,%f\n", j, alpha, k2val, q, k,e(1,k)/startfehler);
	endfor
	alpha = alpha * 0.92;
endfor

E;
fclose(fid);
