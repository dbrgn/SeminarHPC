for (int i = 0; i < n; i++) {
    float   pivot = a[i + 2 * n * i];
    for (int j = minj; j < 2 * n; j++) {
        a[j + 2 * n * i] /= pivot;
    }
    for (int k = 0; k < n; k++) {
        if (k != i) {
            float   b = a[i + 2 * n * k];
            for (int j = minj; j <= minj + n; j++) {
                a[j + 2 * n * k] -= b * a[j + 2 * n * i];
            }
        }
    }
}
