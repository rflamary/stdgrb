""" stdgrb: Python Gurobi solvers for standard dense problems (LP,QP)"""


__version__="0.1.1"

from .stdgurobi import lp_solve,qp_solve,lp_solve_sparse



__all__ = ['lp_solve','qp_solve','lp_solve_sparse']

