#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May 11 10:08:27 2018

@author: rflamary
"""

import numpy as np
import pylab as pl


import stdgrb

#%%

n=10000
d=1000

np.random.seed(0)

c=np.random.randn(d)
A=np.random.rand(n,d)
b=np.random.rand(n)

lb=np.zeros(d)
ub=np.ones(d)


#%%


x=stdgrb.lp_solve(c,A,b,lb,ub)

