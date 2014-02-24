for (int i = 0; i < n; i++) {
        float   pivot = M(a, 2 * n, i, i);
        // Pivot-Zeile dividieren
        for (int j = minj + 1; j < 2 * n; j++) {
                M(a, 2 * n, i, j) /= pivot;
        }
        // Zeilenoperationen
        for (int k = i + 1; k < n; k++) {
                float   b = M(a, 2 * n, k, i);
                for (int j = minj + 1; j < 2 * n; j++) {
                        M(a, 2 * n, k, j) -= b * M(a, 2 * n, i, j);
                }
        }
}
