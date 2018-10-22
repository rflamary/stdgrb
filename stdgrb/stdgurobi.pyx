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
               long * typevar,
               double *solution,
               double *objvalP,
               int nbeq,
               int method,
               int logtoconsole,
               int crossover) nogil


def lp_solve_0(np.ndarray[double, ndim=1] c,
             np.ndarray[double, ndim=2, mode="c"] A, 
             np.ndarray[double, ndim=1] b, 
             np.ndarray[double, ndim=1] lb,np.ndarray[double, ndim=1] ub, 
             int nbeq, np.ndarray[long, ndim=1] typevar,
             int method=-1,int logtoconsole=1, int crossover=-1):
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
                         <double*>  ub.data,<long*> typevar.data, <double*>  sol.data, &val0, 
                         nbeq, method, logtoconsole,crossover)
    
    if not solved:
        val=None
    else:
        val=val0
    
    return sol,val

def qp_solve_0(np.ndarray[double, ndim=2, mode="c"] Q,np.ndarray[double, ndim=1] c,
             np.ndarray[double, ndim=2, mode="c"] A, 
             np.ndarray[double, ndim=1] b, 
             np.ndarray[double, ndim=1] lb,np.ndarray[double, ndim=1] ub, 
             int nbeq, np.ndarray[long, ndim=1] typevar,
             int method=-1,int logtoconsole=1, int crossover=-1):
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
                         <double*>  ub.data, <long*> typevar.data, <double*>  sol.data, &val0,
                         nbeq, method, logtoconsole,crossover)
    
    if not solved:
        val=None
    else:
        val=val0
    
    return sol,val


def lp_solve(c,A=None,b=None,lb=None,ub=None,nbeq=0, typevar=None, method=-1,logtoconsole=1, crossover=-1):
    """ Solves a standard linear program
    
    Solve the following optimization problem:
        
    .. math::
        \min_x x^Tc
        
        s.t. 
        
        lb <= x <= ub
        
        Ax <= b
        
    You can also set equalitu constraint with parameter nbeq that define the 
    first nnbeq lines of A and b as esuality constraints.
    
    Uses the gurobi solver.
    
    Parameters
    ----------
    c : (d,) ndarray, float64
        Linear cost vector
    A : (n,d) ndarray, float64, optional
        Linear constraint matrix 
    b : (n,) ndarray, float64, optional
        Linear constraint vector
    lb : (d) ndarray, float64, optional
        Lower bound constraint        
    ub : (d) ndarray, float64, optional
        Upper bound constraint     
    nbeq: int, optional
        Treat the nbeq first lines of A as equality constraints.
    typevar: int, (d,) ndarray, int, optional
        integer or array of integer defining the type of variables (default 
        continuous).
        Select typevar from:
        * 0 : Continuous
        * 1 : Binary
        * 2 : Integer
        * 3 : Semi-continuous (0.0 or value in const)
        * 4 : Semi-integer (0 or value in const)
    method : int, optional
        Selected solver from  
        * -1=automatic (default), 
        * 0=primal simplex, 
        * 1=dual simplex, 
        * 2=barrier, 
        * 3=concurrent, 
        * 4=deterministic concurrent, 
        * 5=deterministic concurrent simplex
    logtoconsole : int, optional
        If 1 the print log in console, 
    crossover : int, optional
        Select crossover strategy for interior point (see gurobi documentation)  
                
    Returns
    -------
    x: (d,) ndarray
        Optimal solution x
    val: float
        optimal value of the objective (None if optimization error)
    
    
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

    if typevar is None:
        typevar=np.zeros(n,dtype=np.int64)
    elif type(typevar)==int:
        typevar=typevar*np.ones(n,dtype=np.int64)
    else:
        typevar=typevar.astype(np.int64)


        
    sol,val=lp_solve_0(c,A,b,lb,ub,nbeq,typevar, method,logtoconsole,crossover)
    
    
    return sol, val
        



def qp_solve(Q,c=None,A=None,b=None,lb=None,ub=None,nbeq=0,typevar=None,  method=-1,logtoconsole=1,crossover=-1):
    """ Solves a standard quadratic program
    
    Solve the following optimization problem:
        
    .. math::
        \min_x  x^TQx+x^Tc
        
        s.t. 
        
        lb <= x <= ub
        
        Ax <= b

    You can also set equalitu constraint with parameter nbeq that define the 
    first nbeq lines of A and b as equality constraints.        
    

    Parameters
    ----------
    Q : (d,d) ndarray, float64, optional
        Quadratic cost matrix matrix     
    c : (d,) ndarray, float64
        Linear cost vector
    A : (n,d) ndarray, float64, optional
        Linear constraint matrix 
    b : (n,) ndarray, float64, optional
        Linear constraint vector
    lb : (d) ndarray, float64, optional
        Lower bound constraint        
    ub : (d) ndarray, float64, optional
        Upper bound constraint    
    nbeq: int, optional
        Treat the nbeq first lines of A as equality constraints.        
    typevar: int, (d,) ndarray, int, optional
        integer or array of integer defining the type of variables (default 
        continuous).
        Select typevar from:
        * 0 : Continuous
        * 1 : Binary
        * 2 : Integer
        * 3 : Semi-continuous (0.0 or value in const)
        * 4 : Semi-integer (0 or value in const)        
    method : int, optional
        Selected solver from  
        * -1=automatic (default), 
        * 0=primal simplex, 
        * 1=dual simplex, 
        * 2=barrier, 
        * 3=concurrent, 
        * 4=deterministic concurrent, 
        * 5=deterministic concurrent simplex
    logtoconsole : int, optional
        If 1 the print log in console, 
    crossover : int, optional
        Select crossover strategy for interior point (see gurobi documentation)  
                 
    Returns
    -------
    x: (d,) ndarray
        Optimal solution x
    val: float
        optimal value of the objective (None if optimization error)
    
    
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

    if typevar is None:
        typevar=np.zeros(n,dtype=np.int64)
    elif type(typevar)==int:
        typevar=typevar*np.ones(n,dtype=np.int64)
    else:
        typevar=typevar.astype(np.int64)    
        
    sol,val=qp_solve_0(Q,c,A,b,lb,ub,nbeq, typevar, method,logtoconsole,crossover)
    

    return sol,val
