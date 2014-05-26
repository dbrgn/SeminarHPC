# -*- coding: utf-8 -*-
from collections import OrderedDict
import numpy as np
from matplotlib import ticker
from matplotlib import pyplot as plt

data = OrderedDict([
    ('Nvidia NVS 3100M', (38.755955, 'gpu')),
    ('Intel i7-4770', (21.453833, 'cpu-i')),
    ('Intel Xeon X7550', (6.259197, 'cpu-i')),
    ('Intel Xeon X7550', (2.516944, 'cpu-a')),
    ('Nvidia Tesla M2090', (1.405895, 'gpu')),
    ('Nvidia GeForce GTX 760', (1.207794, 'gpu')),
])

plt.figure(figsize=(9, 4.5))

plt.xlabel('Compute Device')
plt.ylabel('Laufzeit in Sekunden')

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.FixedLocator(np.arange(len(data))))
ax.xaxis.set_major_formatter(ticker.FixedFormatter(list(data.keys())))

for i, (label, (time, device)) in enumerate(data.items()):
    if device == 'gpu':
        hatch = 'xx'
        color = '.50'
    elif device == 'cpu-i':
        hatch = 'oo'
        color = '.70'
    else:
        hatch = '**'
        color = '.70'
    plt.bar(i, time, hatch=hatch, color=color)
plt.savefig('speed_comparison_v1.pdf')

print('Saved speed_comparison_v1.pdf')
