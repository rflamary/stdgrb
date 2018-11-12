

#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"

/*
  Solve an LP/QP/MILP/MIQP represented using dense matrices.  This
  routine assumes that A and Q are both stored in row-major order.
  It returns 1 if the optimization succeeds.  When successful,
  it returns the optimal objective value in 'objvalP', and the
  optimal solution vector in 'solution'.
*/


// define equality/inequality constraints
char *init_const(int rows, int nbeq)
{

  // define constraint type
  char * sense = malloc(sizeof(char) * rows);
  for (int i = 0; i < rows; i++)
  {
    if (i < nbeq)
      sense[i] = '=';
    else
      sense[i] = '<';
  } //
  
  return sense;


}

char * init_vtype(int cols, long *type)
{

  // define variable type
  char * trad_type="CBISN";
  char * vtype = malloc(sizeof(char) * cols);
  if (!type)
  {
    for (int i = 0; i < cols; i++)
      vtype[i]='C';
  }
  else
  {
     for (int i = 0; i < cols; i++)
     {
         if (type[i]<5)
             vtype[i]=trad_type[type[i]];
         else
             vtype[i]='C';
     }
        
  };
  return vtype;
}



// solve dense optimization problem
static int solve_problem(
    int rows,    /* nb rows in A */
    int cols,    /* nb cols in A, also size of the problame */
    double *c,  /* linear portion of objective function */
    double *Q,  /* quadratic portion of objective function */
    double *A,  /* constraint matrix */
    double *b,  /* RHS vector */
    double *lb, /* variable lower bounds */
    double *ub, /* variable upper bounds */
    long *type, /* type of variable upper bounds */
    double *sol,
    double *objval,
    int nbeq,
    int method,
    int logtoconsole,
    int crossover)
{

  GRBenv *env = NULL;
  GRBmodel *model = NULL;
  int error = 0;
  int optimstatus;
  int success = 0; 

  // initialise constraintes (eq/ineq + variable type) 
  char * sense = init_const(rows,nbeq);
  char * vtype = init_vtype(cols,type);  
  
  /* Create environment */

  error = GRBloadenv(&env, "dense.log");
  if (error)
    goto QUIT;

  /* Create an empty model */
  error = GRBnewmodel(env, &model, "dense", cols, c, lb, ub, vtype, NULL);
  if (error)
    goto QUIT;
    

  /* Populate A matrix */
  int *i_ind = malloc(sizeof(int) * cols);
  int *j_ind = malloc(sizeof(int) * cols);
  double *vals = malloc(sizeof(double) * cols);
  int i,j, i0 = 0;
  
  for (i = 0; i < rows; i++) // for every line in A
  {
    // seeks non zero values
    i0 = 0;
    for (j = 0; j < cols; j++)
    {
      if (A[i * cols + j] != 0)
      {
        j_ind[i0] = j; // store index
        vals[i0] = A[i * cols + j]; // store value
        i0 += 1;
      }
    }
    // add constraint for line i
    error = GRBaddconstr(model, i0, j_ind, vals, sense[i], b[i], NULL);
    if (error)
      goto QUIT;
  }

  /* Populate Q matrix */
  if (Q)
  {
    for (i = 0; i < cols; i++) // for every mine of Q
    {
    // seeks non zero values
    i0 = 0;
    for (j = 0; j < cols; j++)
    {
      if (Q[i * cols + j] != 0)
      {
        j_ind[i0] = j; // store index
        i_ind[i0] = i; // store index
        vals[i0] = Q[i * cols + j]; // store value
        i0 += 1;
      }
    }
    error = GRBaddqpterms(model, i0, i_ind, j_ind, vals);
    if (error)
      goto QUIT;
    }
  }

  free(i_ind);
  free(j_ind);
  free(vals);

 /* set parameters */
  error = GRBsetintparam(GRBgetenv(model), "Method", method);
  if (error)
    goto QUIT;
  error = GRBsetintparam(GRBgetenv(model), "LogToConsole", logtoconsole);
  if (error)
    goto QUIT;
  error = GRBsetintparam(GRBgetenv(model), "Crossover", crossover);
  if (error)
    goto QUIT;
    

    
  /* Optimize model */
  error = GRBoptimize(model);
  if (error)
    goto QUIT;
    
    
  /* get optimization status model */
  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error)
    goto QUIT;
    
  /* if converged */
  if (optimstatus == GRB_OPTIMAL)
  {
    // get objective value
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, objval);
    if (error)
      goto QUIT;

    // get solution
    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, cols, sol);
    if (error)
      goto QUIT;

    success = 1;
  }
  
  QUIT:

  if (error)
  {
    printf("ERROR: %s\n", GRBgeterrormsg(env));
    exit(1);
  }
  
  
  free(sense);
  free(vtype);
  
  if (model) 
      GRBfreemodel(model);
  if (env) 
      GRBfreeenv(env);
  

  return success;
}

static int solve_sparse_lp(
    int rows,    /* nb rows in A */
    int cols,    /* nb cols in A, also size of the problame */
    size_t nnz,  /* number of non-zero coefficients */
    double *val, /* value of non-zero coefficients is CSR format */       
    size_t *beg, /* begin/end value of raows in csr format is CSR format */ 
    int *ind,    /* value of variable is CSR format */ 
    double *c,   /* linear portion of objective function */
    double *b,   /* RHS vector */
    double *lb,  /* variable lower bounds */
    double *ub,  /* variable upper bounds */
    long *type,  /* type of variable upper bounds */
    double *sol,
    double *objval,
    int nbeq,
    int method,
    int logtoconsole,
    int crossover)
{


  GRBenv *env = NULL;
  GRBmodel *model = NULL;
  int error = 0;
  int optimstatus;
  int success = 0; 

  // initialise constraintes (eq/ineq + variable type) 
  char * sense = init_const(rows,nbeq);
  char * vtype = init_vtype(cols,type);  
  
  /* Create environment */

  error = GRBloadenv(&env, "dense.log");
  if (error)
    goto QUIT;

  /* Create an empty model */
  error = GRBnewmodel(env, &model, "dense", cols, c, lb, ub, vtype, NULL);
  if (error)
    goto QUIT;
    
    // add all contrstraints in one line!
   error = GRBXaddconstrs (model,rows,nnz,beg,ind,val,sense,b,NULL);


 /* set parameters */
  error = GRBsetintparam(GRBgetenv(model), "Method", method);
  if (error)
    goto QUIT;
  error = GRBsetintparam(GRBgetenv(model), "LogToConsole", logtoconsole);
  if (error)
    goto QUIT;
  error = GRBsetintparam(GRBgetenv(model), "Crossover", crossover);
  if (error)
    goto QUIT;
    

    
  /* Optimize model */
  error = GRBoptimize(model);
  if (error)
    goto QUIT;
    
    
  /* get optimization status model */
  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error)
    goto QUIT;
    
  /* if converged */
  if (optimstatus == GRB_OPTIMAL)
  {
    // get objective value
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, objval);
    if (error)
      goto QUIT;

    // get solution
    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, cols, sol);
    if (error)
      goto QUIT;

    success = 1;
  }
  
  QUIT:

  if (error)
  {
    printf("ERROR: %s\n", GRBgeterrormsg(env));
    exit(1);
  }
  
  
  free(sense);
  free(vtype);
  
  if (model) 
      GRBfreemodel(model);
  if (env) 
      GRBfreeenv(env);
  

  return success;

}
