#include "def.h"

extern int        N, M;
extern double     **c, *f, *d, *b;
extern double     MAX_DOUBLE;
extern int        *open_facility;
extern int        *customer_assign;


double solve_MIP(void)
{
	int i,j;
	int index,index1;  // auxiliar indices to fill in the constraint matrix
	double best_upper_bound, best_lower_bound;
	int nodecount;
	//Variables to call cplex
	CPXLPptr  lp;      // data strucutre to store a problem in cplex ...................
	CPXENVptr env;     // cplex environment.............................................
	int       numcols; // number of variables ..........................................
	int       numrows; // number of constraints.........................................
	int       numnz;   // number of non-zero elements in the matrix ....................
	int       objsen;  // optimization sense (min:1, max:-1 ) ..........................
	double    *obj;    // objective function coefficients ..............................
	double    *rhs;    // right and side of constraints ................................
	char      *sense;  // constraints sense (<=: 'L', =:'E', >=:'G') ...................
	int       *matbeg; // index of first non-zero element in each row...................
	int       *matind; // associated column of each non-zelo element ...................
	double    *matval; // coefficient values fo the non-zero elements of constraints....
	double    *lb;     // lower bounds of variables.....................................
	double    *ub;     // upper bounds of variables.....................................
	int       status;  // optimization status......................... .................
	double    *x;      // solution vector (double, even if the problem is integer) .....
	char probname[16]; // problem name for cplex .......................................
	char      *ctype;  // variable type ('C', 'I', 'B') only if integer.................
	double    value;   // objevtive value of solution ..................................
	int       num_z_var, num_x_var, whichmodel;
	int       *pos_z;
	int       **pos_x;


	pos_z = create_int_vector(N);
	pos_x = create_int_matrix(N,M);

    objsen = 1; //min

	
	//Initialize CPLEX environment
	env = CPXopenCPLEX (&status);
	if ( env == NULL ) {
		char  errmsg[1024];
		printf ("Could not open CPLEX. \n");
		CPXgeterrorstring (env, status, errmsg);
		printf ("%s", errmsg);
	}

    // Create the problem in CPLEX 
	strcpy(probname,"UFLP");
	lp = CPXcreateprob (env, &status, probname);
	if ( env == NULL ) {
		char  errmsg[1024];
		printf ("Could not create LP. \n");
		CPXgeterrorstring (env, status, errmsg);
		printf ("%s", errmsg);
	}


	                                        //Define z_i variables
    index1 = 0;  // index of columns
	numcols = N;
	d_vector(&obj,numcols,"open_cplex:1");
	d_vector(&lb,numcols,"open_cplex:8");
	d_vector(&ub,numcols,"open_cplex:9");
	c_vector(&ctype,numcols,"open_cplex:01");

    for(i=0;i<N;i++){
	   pos_z[i] = index1;
       obj[index1] = f[i];
       ctype[index1] = 'B';
       lb[index1] = 0;
       ub[index1] = 1;
       index1++;
	}
	status = CPXnewcols (env, lp, index1, obj, lb, ub, ctype, NULL);
	if( status ) 
      fprintf (stderr,"CPXnewcols failed.\n");
	free(obj);
	free(lb);
	free(ub);
	free(ctype);
	num_z_var = index1;
	
	                                        //Define x_ij variables
    index1 = 0;  // index of columns
	numcols = N*M;
	d_vector(&obj,numcols,"open_cplex:1");
	d_vector(&lb,numcols,"open_cplex:8");
	d_vector(&ub,numcols,"open_cplex:9");
	c_vector(&ctype,numcols,"open_cplex:01");
 
    for(i=0;i<N;i++){
      for(j=0;j<M;j++){
		pos_x[i][j] = num_z_var + index1;
        obj[index1] = d[j]*c[i][j];
        ctype[index1] = 'C';
        lb[index1] = 0;
        ub[index1] = 1;
        index1++;
	  }
	}
    status = CPXnewcols (env, lp, index1, obj, lb, ub, ctype, NULL);
	if( status ) 
      fprintf (stderr,"CPXnewcols failed.\n");
	free(obj);
	free(lb);
	free(ub);
	free(ctype);
	num_x_var = index1;


	                                             //Add assignment constraints  \sum_{i \in N} x_ij = 1
	numrows = M;
	numnz = M*N;
	d_vector(&rhs,numrows,"open_cplex:2");
	c_vector(&sense,numrows,"open_cplex:3");
	i_vector(&matbeg,numrows,"open_cplex:4");
	i_vector(&matind,numnz,"open_cplex:6");
	d_vector(&matval,numnz,"open_cplex:7");

    index = 0;
    index1 = 0;
    for(j=0;j<M;j++){
      sense[index1]='E';
      rhs[index1]= 1;
	  matbeg[index1++] = index;
	  for(i=0;i<N;i++){
		 matind[index] = pos_x[i][j];
         matval[index++] = 1;
	  }
    }
	status = CPXaddrows (env, lp, 0, index1, index, rhs, sense, matbeg, matind, matval, NULL, NULL);
    if( status ) 
      fprintf (stderr,"CPXaddrows failed.\n");
	free(matbeg);
	free(matind);
	free(matval);
	free(sense);
	free(rhs);

	printf("Enter 1 to solve agregated formulation or 2 to solve disagregated formulation \n");
	scanf("%d",&whichmodel);


	if(whichmodel == 1){
	                                             //Add linking constraints  x_ij <= z_i
	numrows = M*N;
	numnz = 2*M*N;
	d_vector(&rhs,numrows,"open_cplex:2");
	c_vector(&sense,numrows,"open_cplex:3");
	i_vector(&matbeg,numrows,"open_cplex:4");
	i_vector(&matind,numnz,"open_cplex:6");
	d_vector(&matval,numnz,"open_cplex:7");

    index = 0;
    index1 = 0;
	for(i=0;i<N;i++){
      for(j=0;j<M;j++){
        sense[index1]='L';
        rhs[index1]= 0;
		matbeg[index1++] = index;
		matind[index] = pos_x[i][j];
        matval[index++] = 1;
		matind[index] = pos_z[i];
        matval[index++] = -1;
	  }
	}
	status = CPXaddrows (env, lp, 0, index1, index, rhs, sense, matbeg, matind, matval, NULL, NULL);
    if( status ) 
      fprintf (stderr,"CPXaddrows failed.\n");
	free(matbeg);
	free(matind);
	free(matval);
	free(sense);
	free(rhs);

	}
	else{
	                                             //Add linking constraints  \sum (j in M) x_ij <= |M| z_i
	numrows = N;
	numnz = N*(M+1);
	d_vector(&rhs,numrows,"open_cplex:2");
	c_vector(&sense,numrows,"open_cplex:3");
	i_vector(&matbeg,numrows,"open_cplex:4");
	i_vector(&matind,numnz,"open_cplex:6");
	d_vector(&matval,numnz,"open_cplex:7");

    index = 0;
    index1 = 0;
	for(i=0;i<N;i++){
        sense[index1]='L';
        rhs[index1]= 0;
	    matbeg[index1++] = index;
		for(j=0;j<M;j++){
		  matind[index] = pos_x[i][j];
          matval[index++] = 1;
	   }
	matind[index] = pos_z[i];
    matval[index++] = -M;
	}
	status = CPXaddrows (env, lp, 0, index1, index, rhs, sense, matbeg, matind, matval, NULL, NULL);
    if( status ) 
      fprintf (stderr,"CPXaddrows failed.\n");
	free(matbeg);
	free(matind);
	free(matval);
	free(sense);
	free(rhs);

	}  

    //CPXwriteprob(env,lp,"model2.lp",NULL);                          //write the model in .lp format if needed (to debug)

    CPXsetintparam(env,CPX_PARAM_SCRIND,CPX_ON); //output display
    //CPXsetintparam(env,CPX_PARAM_INTSOLLIM,1);    //stops after finding first integer sol.
    CPXsetintparam(env,CPX_PARAM_MIPDISPLAY,3); //different levels of output display
    //CPXsetintparam(env,CPX_PARAM_MIPEMPHASIS,1);//0:balanced; 1:feasibility; 2:optimality,3:bestbound, 4:hiddenfeas
    CPXsetdblparam(env,CPX_PARAM_TILIM,86400); // time limit
    //CPXsetdblparam(env,CPX_PARAM_TRELIM, 14000); // B&B memory limit
    //CPXsetdblparam(env,CPX_PARAM_EPGAP, 0.0000000001); // e-optimal solution (%gap)
    //CPXsetdblparam(env,CPX_PARAM_EPAGAP, 0.0000000001); // e-optimal solution (absolute value)
    //CPXsetdblparam(env,CPX_PARAM_EPINT, 0.0000000001); // integer precision
	//CPXsetintparam(env,CPX_PARAM_THREADS, 1); // Number of threads to use
	//CPXsetdblparam(env,CPX_PARAM_EPRHS, 0.0000001);
	//CPXsetintparam(env,CPX_PARAM_REDUCE, 0);  // only needed when adding lazy constraints
    //CPXsetintparam(env,CPX_PARAM_HEURFREQ, -1); //heuristic frequency and intensisty 
	CPXsetdblparam(env,CPX_PARAM_CUTSFACTOR, 1.0);  //limit the number of cuts added by cplex 1.0002
	//CPXsetdblparam(env,CPX_PARAM_CUTUP,UpperBound+.01); // provide an initial upper bound
	//CPXsetintparam(env,CPX_PARAM_MIPEMPHASIS,CPX_MIPEMPHASIS_OPTIMALITY);  // MIP emphasis: optimality, feasibility, moving best bound
	//CPXsetintparam(env,CPX_PARAM_PARALLELMODE, 1); 
	//CPXsetintparam(env,CPX_PARAM_PREIND,0);
	//CPXsetintparam(env,CPX_PARAM_MIPORDIND,CPX_ON); // Turn on or off the use of priorities on bracnhing variables
	//CPXsetintparam(env,CPX_PARAM_MIPEMPHASIS,CPX_MIPEMPHASIS_BESTBOUND);  // MIP emphasis: optimality, feasibility, moving best bound

	CPXmipopt(env,lp);  //solve the integer program

    i=CPXgetstat(env,lp);
	if(i==101)
		printf("Optimal solution found\n");
	else if(i==102)
		printf("e-optimal solution found\n");
	else if(i==103)
		printf(" infeasible solution\n");
	else if(i==107)
		printf("Time limit reached\n");
	else
		printf("Unknown stopping criterion (%d)\n",i);
  
	// retrive solution values
	CPXgetmipobjval(env,lp,&value);
	printf("Upper bound: %.2f   ",value);
	best_upper_bound=value;
    // If CPLEX was able to find the optimal solution, the previous function provides the optimal solution value
	//if not, it provides the best upper bound
	CPXgetbestobjval(env,lp,&value);  //best lower bound in case the problem was not solved to optimality
	best_lower_bound=value;
	printf("Lower bound: %.2f  \n",value);
        
	nodecount = CPXgetnodecnt (env, lp);
	printf("Number of BB nodes : %ld  \n",nodecount);

	numcols = CPXgetnumcols (env, lp);
    d_vector(&x,numcols,"open_cplex:0");
	CPXgetmipx(env,lp,x,0, numcols-1);  // obtain the values of the decision variables

	for(i=0;i<N;i++){
      if( x[pos_z[i]] > 0.5)
	    open_facility[i] = 1;
	  else
	    open_facility[i] = 0;
	}

    /*printf("Optimal Allocation: ");
    for(i=0;i<N;i++){
      for(j=0;j<M;j++){
        if( x[pos_x[i][j]] > 0.5)
	      printf("(%d,%d) ", i+1,j+1);
	  }
	}*/

   // TERMINATE:

    if ( lp != NULL ) {
      status = CPXfreeprob (env, &lp);
      if ( status ) {
        fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
      }
    }
    if ( env != NULL ) {
      status = CPXcloseCPLEX (&env);
      if ( status ) {
        char  errmsg[1024];
        fprintf (stderr, "Could not close CPLEX environment.\n");
        CPXgeterrorstring (env, status, errmsg);
        fprintf (stderr, "%s", errmsg);
      }
    }

    for(i=0;i<N;i++)
      free(pos_x[i]);
	free(pos_z);
	free(pos_x);
	free(x);

	return best_upper_bound;
}


/* This simple routine frees up the pointer *ptr, and sets *ptr
   to NULL */

static void free_and_null (char **ptr)
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */ 








