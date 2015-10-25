#!/usr/bin/python
# -*- coding: utf-8 -*-

from subprocess import Popen, PIPE
import numpy as np
import matplotlib.pyplot as plt

def measure (lengths, max_attempts):
    measurements = {}

    for length, attempts in [(l, max_attempts / l) for l in lengths]:
        command = "benchmark/rangegen --count=1 --length=" + str(length) + " | benchmark/algorithm/radix --attempts=" + str(attempts) + " --bits=32"
        results = Popen(command, shell=True, stdin=PIPE, stdout=PIPE).stdout.read().splitlines()

        for algorithm, time in [(a, float(t) / attempts) for a, t in [item.split() for item in results]]:
            times = measurements.setdefault(algorithm, [])
            times += [time]

    return measurements

def plot (lengths, measurements, file_name):
    plt.figure(num=1)
    plt.title(u'Производительность целочисленных сортировок', size=14)
    plt.xlabel(u'Размер массива', size=14)
    plt.ylabel(u'Время, с', size=14)

    xData = np.array(lengths)

    for algorithm, times in measurements.items():
        yData = np.array(times)
        plt.plot(xData, yData, label=algorithm)

    plt.legend(loc='upper left')
    plt.savefig(file_name, format='png')

max_attempts = 10000000
lengths = \
    range(10, 100, 10) + \
    range(100, 1000, 100) + \
    range(1000, 10000, 1000) + \
    range(10000, 100000, 10000) + \
    range(100000, 1000000, 100000) + \
    [1000000]

measurements = measure(lengths, max_attempts)
plot(lengths, measurements, 'intsort.png')
