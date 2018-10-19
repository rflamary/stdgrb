# stdgrb: Python Gurobi solvers for standard dense problems 

This is a fast cython wrapper for strandard dense optimization that calls 
gurobi through its C interface. It is more efficient for dense problems 
than the gurobi python 
interface that cannot accept directly numpy arrays for constraints.

## Install

The only strong dependencies are

* numpy
* cython (for the compilation)

In order to build install ```stdgrb``` you need to define first environement 
variables ```GUROBI_HOME``` and ```GUROBI_VERSION``` so that the compiler can 
find the include files and link the good library.

this can be done with 

```bash
export GUROBI_HOME=/path/to/gurobi/linux64
export GUROBI_VERSION=gurobi75 # for gurobi 7.5
``` 

you can find the gurobi version in the ```GUROBI_HOME/lib``` folder by seeking 
the ```libgurobi[XX].so``` and setting ```GUROBI_VERSION=gurobi[XX]``` 
(be carefull to remove the 'lib').

When the environement variables are set, you can install the module with

```bash
python setup.py install # --user if local install
```

## Use the module


Supposing that the matrices A, b, lb and ub are already defined, you can solve
 the linear program with
 
 
 ```python
 import stdgrb
 sol,val=stdgrb.lp_solve(c,A,b,lb,ub)
 ```
 
 if A, b, ul, ub and not given or None, the corresponding constraints are not 
 used.
 
 
 
