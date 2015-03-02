#!/usr/bin/env python

from __future__ import division
from numpy import *
import pylab as plt
from matplotlib.mlab import griddata

class ColumnList(object):
    def __init__(self, filename):
        self.columnList = []
        with open(filename) as f:
            for line in f:
                self.columnList.append(line)

    def __getitem__(self, key):
        i0 = None
        for i, s in enumerate(self.columnList):
            if key in s:
                if i0 is None:
                    i0 = i
                else:
                    print "Key not unique"
                    return None
        if i0 is not None:
            return i0
        else:
            print "Key not found"
            return None

columnList = ColumnList('gambit_output.info')
data = genfromtxt("gambit_output.txt").T
x = data[columnList['mass']]
y = data[columnList['lambda']]

def plot(x, y, z, color='k'):
    plt.loglog(x, y, ls='', marker='')
    xi = logspace(log10(min(x)), log10(max(x)), 100)
    yi = logspace(log10(min(y)), log10(max(y)), 100)
    zi = griddata(x, y, z, xi, yi, interp='linear')
    zi = -(zi - zi.max())*2
    plt.contour(xi, yi, zi, levels=[1, 4, 9], linestyles=['-', '--', ':'], colors=color)

z = data[columnList['Fermi']]
plot(x, y, z, color='r')

z = data[columnList['XENON']]
plot(x, y, z, color='k')

z = data[columnList['LUX']]
plot(x, y, z, color='b')

z = data[columnList['lnL_oh2']]
plot(x, y, z, color='g')

plt.xlim([45, 100])
plt.ylim([1e-3, 1])
plt.show()
