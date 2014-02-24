for (int i = n - 1; i >= 0; i--) {
        for (int k = i - 1; k >= 0; k--) {
                float   b = M(a, 2 * n, k, i);
                for (int j = n; j < 2 * n; j++) {
                        M(a, 2 * n, k, j) -= b * M(a, 2 * n, i, j);
                }
        }
}
