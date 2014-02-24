for (int i = 0; i < n; i++) {
	F	pivot = M(a, 2 * n, i, i);
	for (int j = minj + 1; j < 2 * n; j++) {
		M(a, 2 * n, i, j) /= pivot;
	}
	for (int k = i + 1; k < n; k++) {
		F	b = M(a, 2 * n, k, i);
		for (int j = minj + 1; j < 2 * n; j++) {
			M(a, 2 * n, k, j) -= b * M(a, 2 * n, i, j);
		}
	}
}

for (int i = n - 1; i >= 0; i--) {
	for (int k = i - 1; k >= 0; k--) {
		F	b = M(a, 2 * n, k, i);
		for (int j = n; j < 2 * n; j++) {
			M(a, 2 * n, k, j) -= b * M(a, 2 * n, i, j);
		}
	}
}
