#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Feb 28 16:32:42 2018

@author: rflamary
"""

import numpy as np
cimport numpy as np
import cython
cimport cython

cdef extern from "gurobi_wrap.c":
    
    int solve_problem(
               int     rows,
               int     cols,
               double *c,    
               double *Q,    
               double *A,   
               double *b,  
               double *lb,   
               double *ub,   
               double *solution,
               double *objvalP) nogil


def lp_solve_0(np.ndarray[double, ndim=1] c,
             np.ndarray[double, ndim=2, mode="c"] A, 
             np.ndarray[double, ndim=1] b, 
             np.ndarray[double, ndim=1] lb,np.ndarray[double, ndim=1] ub):
    """ solve stanard linear program
    
    solve the following optimization problem:
        
        min_x  x'c
        
        s. t.  lb <= x <= up
                 Ax <= b
                 
    returns sol,val that are the solution of the optimization problem and the value (val is None if an error occured)
    
    """
    
    
    cdef int  rows = A.shape[0]
    cdef int  cols = A.shape[1]
    
    cdef double val0
    cdef np.ndarray[double, ndim=1] sol=np.zeros_like(c)

    solved=solve_problem(rows, cols,   <double*>  c.data,NULL, <double*>  A.data,
                         <double*>  b.data, <double*>  lb.data,
                         <double*>  ub.data,<double*>  sol.data, &val0)
    
    if not solved:
        val=None
    else:
        val=val0
    
    return sol,val

def qp_solve_0(np.ndarray[double, ndim=2, mode="c"] Q,np.ndarray[double, ndim=1] c,
             np.ndarray[double, ndim=2, mode="c"] A, 
             np.ndarray[double, ndim=1] b, 
             np.ndarray[double, ndim=1] lb,np.ndarray[double, ndim=1] ub):
    """ solve stanard linear program
    
    solve the following optimization problem:
        
        min_x  x'Qx + x'c
        
        s. t.  lb <= x <= up
                 Ax <= b
                 
    returns sol,val that are the solution of the optimization problem and the value (val is None if an error occured)
    
    """
    
    
    cdef int  rows = A.shape[0]
    cdef int  cols = A.shape[1]
    
    cdef double val0
    cdef np.ndarray[double, ndim=1] sol=np.zeros_like(c)

    solved=solve_problem(rows, cols,   <double*>  c.data,<double*>  Q.data, <double*>  A.data,
                         <double*>  b.data, <double*>  lb.data,
                         <double*>  ub.data,<double*>  sol.data, &val0)
    
    if not solved:
        val=None
    else:
        val=val0
    
    return sol,val


def lp_solve(c,A=None,b=None,lb=None,ub=None):
    """ solve stanard linear program
    lp_solve(c,A=None,b=None,lb=None,ub=None)
    
    solve the following optimization problem:
        
        min_x  x'c
        
        s. t.  lb <= x <= up
                 Ax <= b
                 
    returns sol,val that are the solution of the optimization problem and the value (val is None if an error occured)
    
    """
    
    n=c.shape[0]
    
    if A is None or b is None:
        A=np.zeros((0,n))
        b=np.zeros(0)
        
    if lb is None:
        lb=-np.ones(n)*np.inf

    if ub is None:
        ub=np.ones(n)*np.inf
        
    if not A.flags.c_contiguous:
        A=A.copy(order='C')
        
    sol,val=lp_solve_0(c,A,b,lb,ub)
    
    
    return sol, val
        



def qp_solve(Q,c=None,A=None,b=None,lb=None,ub=None):
    """ solve standard quadratic  program
    qp_solve(Q,c=None,A=None,b=None,lb=None,ub=None)
    
    solve the following optimization problem:
        
        min_x  x'Qx + x'c
        
        s. t.  lb <= x <= up
                 Ax <= b
                 
    returns sol,val that are the solution of the optimization problem and the value (val is None if an error occured)
    
    """
    
    n=Q.shape[0]
    
    if c is None:
        c=np.zeros((n))
    
    if A is None or b is None:
        A=np.zeros((0,n))
        b=np.zeros(0)
        
    if lb is None:
        lb=-np.ones(n)*np.inf

    if ub is None:
        ub=np.ones(n)*np.inf
        
    if not A.flags.c_contiguous:
        A=A.copy(order='C')

    if not Q.flags.c_contiguous:
        Q=Q.copy(order='C')
        
    sol,val=qp_solve_0(Q,c,A,b,lb,ub)
    

    return sol,val
