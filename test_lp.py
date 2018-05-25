#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May 11 10:08:27 2018

@author: rflamary
"""

import numpy as np
import pylab as pl
import scipy

import stdgrb
import time

t_start=time.clock()

def tic():
    t_start=time.clock()

def toc():
    t=time.clock()-t_start
    print('Elapsed time: {:1.3f}s'.format(t))
    return t

#%%

n=1000
d=100

np.random.seed(0)

c=np.random.randn(d)
A=np.random.rand(n,d)
b=np.random.rand(n)

lb=np.zeros(d)
ub=np.ones(d)


#%%

print('Default method')
tic()
x=stdgrb.lp_solve(c,A,b,lb,ub)
toc()


print('Simplex method')
tic()
x=stdgrb.lp_solve(c,A,b,lb,ub,1)
toc()

print('Interior point method')
tic()
x=stdgrb.lp_solve(c,A,b,lb,ub,2)
toc()
#%%

