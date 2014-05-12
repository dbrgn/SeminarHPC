# -*- coding: utf-8 -*-
from matplotlib import pyplot as plt

data = [0.000510, 0.000458, 0.000330, 0.002104, 0.049636, 1.151242, 29.932286, 778.239443]

X = range(1, len(data) + 1)
Y = data

plt.figure(figsize=(9, 4.5))
plt.yscale('log')

plt.xlabel('Länge Eingabewort')
plt.ylabel('Laufzeit in Sekunden')

plt.plot(X, Y, marker='o', color='.00')
plt.savefig('runtime.pdf')

print('Saved runtime.pdf')
