# -*- coding: utf-8 -*-
from collections import OrderedDict
import numpy as np
from matplotlib import ticker
from matplotlib import pyplot as plt

for version in [1, 2]:
    data = OrderedDict([
        ('Intel Xeon X7550\n(Single Threaded)', (185, 'single')),
        ('Intel i7-4770\n(Single Threaded)', (80, 'single')),
        ('Nvidia NVS 3100M', (38.755955, 'gpu')),
        ('Intel i7-4770', (21.453833, 'cpu')),
        ('Intel Xeon X7550\n(Intel OpenCL)', (6.259197, 'cpu')),
        ('Intel Xeon X7550\n(AMD OpenCL)', (2.516944, 'cpu')),
        ('Nvidia Tesla M2090', (1.405895, 'gpu')),
        ('Nvidia GeForce GTX 760', (1.207794, 'gpu')),
    ])

    plt.figure(figsize=(12, 3.3))

    plt.ylabel('Laufzeit in Sekunden')

    ax = plt.axes()
    ax.xaxis.set_major_locator(ticker.FixedLocator(np.arange(len(data))))
    ax.xaxis.set_major_formatter(ticker.FixedFormatter(list(data.keys())))
    ax.yaxis.grid(True)

    hatches_labeled = set()
    for i, (label, (time, device)) in enumerate(data.items()):
        if device == 'gpu':
            hatch = 'xx'
            color = '.70'
            label = 'GPU'
        elif device == 'cpu':
            hatch = '--'
            color = '.90'
            label = 'CPU'
        elif device == 'single' and version == 2:
            hatch = '||'
            color = '.50'
            label = 'CPU Single Threaded'
        else:
            continue
        kwargs = {}
        if hatch not in hatches_labeled:
            kwargs = {'label': label}
            hatches_labeled.add(hatch)
        plt.bar(i, time, hatch=hatch, color=color, width=0.5, **kwargs)

    labels = ax.get_xticklabels()
    plt.setp(labels, rotation=20)

    ax.legend(loc='upper right', bbox_to_anchor=(0.97, 0.95))

    plt.savefig('speed_comparison_v%d.png' % version, bbox_inches='tight')

    print('Saved speed_comparison_v%d.png' % version)
