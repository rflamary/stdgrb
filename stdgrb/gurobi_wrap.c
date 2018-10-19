

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

  //error = GRBaddconstrs(model, rows, 0, NULL, NULL, NULL, sense, rhs, NULL);
  if (error)
    goto QUIT;

  int *cind = malloc(sizeof(int) * cols);
  double *vals = malloc(sizeof(double) * cols);
  //for (i = 0; i < cols; i++) cind[i]=i;
  int i0 = 0;
  /* Populate A matrix */

  for (i = 0; i < rows; i++)
  {
    i0 = 0;
    for (j = 0; j < cols; j++)
    {
      if (A[i * cols + j] != 0)
      {
        cind[i0] = j;
        vals[i0] = A[i * cols + j];
        i0 += 1;
      }
    }
    error = GRBaddconstr(model, i0, cind, vals, sense[i], rhs[i], NULL);
    if (error)
      goto QUIT;
  }

  free(cind);
  free(vals);

  /* Populate Q matrix */

  if (Q)
  {
    for (i = 0; i < cols; i++)
    {
      for (j = 0; j < cols; j++)
      {
        if (Q[i * cols + j] != 0)
        {
          error = GRBaddqpterms(model, 1, &i, &j, &Q[i * cols + j]);
          if (error)
            goto QUIT;
        }
      }
    }
  }

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
    int rows,
    int cols,
    double *c,  /* linear portion of objective function */
    double *Q,  /* quadratic portion of objective function */
    double *A,  /* constraint matrix */
    double *b,  /* RHS vector */
    double *lb, /* variable lower bounds */
    double *ub, /* variable upper bounds */
    double *sol,
    double *objval,
    int nbeq,
    int method,
    int logtoconsole,
    int crossover)
{

  // contrainte
  char *sense = malloc(sizeof(char) * rows);
  for (int i = 0; i < rows; i++)
  {
    if (i < nbeq)
      sense[i] = '=';
    else
      sense[i] = '<';
  } //

  GRBenv *env = NULL;
  int error = 0;
  int solved = 0;

  /* Create environment */

  error = GRBloadenv(&env, "dense.log");
  if (error)
    goto QUIT;

  /* Solve the model */

  solved = dense_optimize(env, rows, cols, c, Q, A, sense, b, lb,
                          ub, NULL, sol, objval, method, logtoconsole,
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
