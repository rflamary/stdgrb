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

print('Example from Wikipedia')
# https://en.wikipedia.org/wiki/Integer_programming


n=3
d=2

np.random.seed(0)

c=np.array([0.0,-1.0])
A=np.array([[-1.0,1],
            [3,2],
            [2,3]])
b=np.array([1.0,12,12])

lb=np.zeros(d)
ub=4*np.ones(d)


#%%


print('Continuous LP ')
tic()
x1,v1=stdgrb.lp_solve(c,A,b,lb,ub,logtoconsole=0)
toc()

print(x1)


print('Binary variables !')
tic()
x1,v1=stdgrb.lp_solve(c,A,b,lb,ub,typevar=1,logtoconsole=0)
toc()

print(x1)


print('Integer variables !')
tic()
x1,v1=stdgrb.lp_solve(c,A,b,lb,ub,typevar=2,logtoconsole=0)
toc()

print(x1)

