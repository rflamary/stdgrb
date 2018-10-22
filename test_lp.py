#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May 11 10:08:27 2018

@author: rflamary
"""

import numpy as np
import pylab as pl
import scipy
import scipy.optimize

import stdgrb
import time

t_start=time.clock()

def tic():
    global t_start
    t_start=time.clock()

def toc():
    global t_start
    t=time.clock()-t_start
    print('Elapsed time: {:1.3f}s'.format(t))
    return t

#%%

n=100
d=10

np.random.seed(0)

c=-np.random.rand(d)
A=np.random.rand(n,d)
b=np.random.rand(n)

lb=np.zeros(d)
ub=np.ones(d)


#%%

print('Scipy simplex solver')
tic()
sol=scipy.optimize.linprog(c,A,b)
x0=sol.x
v0=sol.fun
toc()

print('Scipy interior point solver')
tic()
sol=scipy.optimize.linprog(c,A,b,method='interior-point')
x00=sol.x
v00=sol.fun
toc()

print('Default method')
tic()
x1,v1=stdgrb.lp_solve(c,A,b,lb,ub,logtoconsole=0)
toc()


print('Simplex method')
tic()
x2,v2=stdgrb.lp_solve(c,A,b,lb,ub,method=1,logtoconsole=0)
toc()

print('Interior point method')
tic()
x3,v3=stdgrb.lp_solve(c,A,b,lb,ub,method=2,logtoconsole=0,crossover=0)
toc()
#%%

