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
import scipy as sp
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

n=1000
d=10000

np.random.seed(0)

c=-np.random.rand(d)
A=np.random.rand(n,d)
b=np.random.rand(n)

lb=np.zeros(d)
ub=np.ones(d)


#%%


print('Default method')
tic()
x1,v1=stdgrb.lp_solve(c,A,b,lb,ub,logtoconsole=0)
toc()

print(x1)

As=sp.sparse.csr_matrix(A)

tic()
x2,v2=stdgrb.lp_solve_sparse(c,As,b,lb,ub,logtoconsole=0)
toc()
print(x2)
print('err=',np.sum(np.square(x1-x2)))


#%%

