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

nc=100
d=2000
nsamples=d*5

np.random.seed(0)

X=np.random.randn(nsamples,d)

G=X.T.dot(X)/nsamples

c=np.random.randn(d)

lb=np.zeros(d)
ub=np.ones(d)



# solve with 
tic()
sol,val=stdgrb.qp_solve(0.5*G,c,lb=lb,ub=ub,logtoconsole=True)
toc()

print(sol[:10])

