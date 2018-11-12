

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


static int
dense_optimize(GRBenv *env,
               int rows,
               int cols,
               double *c,        /* linear portion of objective function */
               double *Q,        /* quadratic portion of objective function */
               double *A,        /* constraint matrix */
               char *sense,      /* constraint senses */
               double *rhs,      /* RHS vector */
               double *lb,       /* variable lower bounds */
               double *ub,       /* variable upper bounds */
               char *vtype,      /* variable types (continuous, binary, etc.) */
               double *solution, /* result of optimization */
               double *objvalP,  /* final objective value of optimization */
               int method,       /* solver used by gurobi */
               int logtoconsole, /* print stuff during and after optimization*/
               int crossover)
{
  GRBmodel *model = NULL;
  int i, j, optimstatus;
  int error = 0;
  int success = 0;

  /* Create an empty model */
  error = GRBnewmodel(env, &model, "dense", cols, c, lb, ub, vtype, NULL);
  if (error)
    goto QUIT;




  /* Populate A matrix */
  int *i_ind = malloc(sizeof(int) * cols);
  int *j_ind = malloc(sizeof(int) * cols);
  double *vals = malloc(sizeof(double) * cols);
  int i0 = 0;

  for (i = 0; i < rows; i++)
  {
    // seeks non zero values
    i0 = 0;
    for (j = 0; j < cols; j++)
    {
      if (A[i * cols + j] != 0)
      {
        j_ind[i0] = j;
        vals[i0] = A[i * cols + j];
        i0 += 1;
      }
    }
    // add constraint for line i
    error = GRBaddconstr(model, i0, j_ind, vals, sense[i], rhs[i], NULL);
    if (error)
      goto QUIT;
  }



  /* Populate Q matrix */

  if (Q)
  {
    for (i = 0; i < cols; i++)
    {
    // seeks non zero values
    i0 = 0;
    for (j = 0; j < cols; j++)
    {
      if (Q[i * cols + j] != 0)
      {
        j_ind[i0] = j;
        i_ind[i0] = i;
        vals[i0] = Q[i * cols + j];
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

  /* Write model to 'dense.lp' */

  //error = GRBwrite(model, "dense.lp");
  //if (error) goto QUIT;

  /* Capture solution information */

  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error)
    goto QUIT;

  if (optimstatus == GRB_OPTIMAL)
  {

    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, objvalP);
    if (error)
      goto QUIT;

    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, cols, solution);
    if (error)
      goto QUIT;

    success = 1;
  }

QUIT:

  /* Error reporting */

  if (error)
  {
    printf("ERROR: %s\n", GRBgeterrormsg(env));
    exit(1);
  }

  /* Free model */

  GRBfreemodel(model);

  return success;
}

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

  // define constraint type
  char *sense = malloc(sizeof(char) * rows);
  for (int i = 0; i < rows; i++)
  {
    if (i < nbeq)
      sense[i] = '=';
    else
      sense[i] = '<';
  } //

  // define variable type
  char *trad_type="CBISN";
  char *vtype = malloc(sizeof(char) * cols);
  if (!type)
  {
    for (int i = 0; i < cols; i++)
      vtype[i]='C';
  }
  else
  {
     for (int i = 0; i < cols; i++)
      vtype[i]=trad_type[type[i]];   
  };
  


  GRBenv *env = NULL;
  int error = 0;
  int solved = 0;

  /* Create environment */

  error = GRBloadenv(&env, "dense.log");
  if (error)
    goto QUIT;

  /* Solve the model */

  solved = dense_optimize(env, rows, cols, c, Q, A, sense, b, lb,
                          ub, vtype, sol, objval, method, logtoconsole,
                          crossover);

  //  if (solved)
  //    printf("Solved: x=%.4f, y=%.4f, z=%.4f\n", sol[0], sol[1], sol[2]);

QUIT:

  if (error)
  {
    printf("ERROR: %s\n", GRBgeterrormsg(env));
    exit(1);
  }

  free(sense);

  /* Free environment */

  GRBfreeenv(env);

  return solved;
}

static int solve_sparse_lp(
    int rows,    /* nb rows in A */
    int cols,    /* nb cols in A, also size of the problame */
    size_t nnz,
    double *val,        /* linear portion of objective function */
    size_t *beg,
    int *ind,
    double *c,
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

 // define constraint type
  char *sense = malloc(sizeof(char) * rows);
  for (int i = 0; i < rows; i++)
  {
    if (i < nbeq)
      sense[i] = '=';
    else
      sense[i] = '<';
  } //

  // define variable type
  char *trad_type="CBISN";
  char *vtype = malloc(sizeof(char) * cols);
  if (!type)
  {
    for (int i = 0; i < cols; i++)
      vtype[i]='C';
  }
  else
  {
     for (int i = 0; i < cols; i++)
      vtype[i]=trad_type[type[i]];   
  };
  
  

  GRBenv *env = NULL;
  int error = 0;
 
  
  
  /* Create environment */

  error = GRBloadenv(&env, "dense.log");
  if (error)
    goto QUIT;

  /* Solve the model */

  GRBmodel *model = NULL;
  int optimstatus;
  int success = 0;

  /* Create an empty model */
  error = GRBnewmodel(env, &model, "dense", cols, c, lb, ub, vtype, NULL);
  if (error)
    goto QUIT;
    
    // add all contrstraints
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
    
    
    
  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error)
    goto QUIT;
    
   QUIT:

  if (error)
  {
    printf("ERROR: %s\n", GRBgeterrormsg(env));
    exit(1);
  }
  
    if (optimstatus == GRB_OPTIMAL)
  {

    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, objval);
    if (error)
      goto QUIT;

    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, cols, sol);
    if (error)
      goto QUIT;

    success = 1;
  }
  
  free(sense);
  
  GRBfreeenv(env);

  return success;

}
