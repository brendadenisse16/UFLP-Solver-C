#include "def.h"

extern int N, M;
extern double** c, * f, * d, * b;
extern double MAX_DOUBLE;
extern int* open_facility;
extern int* customer_assign;

void LagrangeanUflp(double upper_bound)
{
	int i, j, t, best_i;
	double acum_lambda;
	double SqrNorm, L_value, min_cost;
	double StepLength, UB;
	int max_iter = 1000;
	int interv_dec_lambda = 1000;
	int last = 1;
	double epsilon = 2;
	double best_lower_bound = 0;
	double* lambda;
	int** x;
	int* z ;
	int* s;

	lambda = create_double_vector(M);
	x = create_int_matrix(N, M);
	z = create_int_vector(N);
	s = create_int_vector(M);

	for (j = 0; j < M; j++) //Initialize Lagrange multipliers
	lambda[j] = 0;


	for (t = 0; t < max_iter; t++)
	{
		L_value = 0;
		for (i = 0; i < N; i++)
		{ // Reset value of decision variables
			for (j = 0; j < M; j++)
			{
				x[i][j] = 0;
				customer_assign[j] = 0;
				s[j] = 0;
			}
			z[i] = 0;
		}


		

		
		// turn breaks up into a subproblem for each location. 
		for (i = 0; i < N; i++)  //Solve z(lambda) for each location
		{ 
			min_cost = 0;
			for (j = 0; j < M; j++)
			{
				if ((lambda[j]+d[j] * c[i][j]) < 0) //  If Z[i]=1, all clients that are profitable are served, nameley those (d[j]*c[i][j] + lambda[j]) < 0)
				{
					
					min_cost += (lambda[j]+d[j]*c[i][j]); //
					customer_assign[j] = 1;
				}
			}
			if (f[i] + min_cost < 0) //  if the set_up cost f[i] + min_cost<0 ,  it is optimal to set z[i]  
			{
				for (j = 0; j < M; j++)
					x[i][j] = customer_assign[j]; //Update x[i][j]==1 (customer assign )
				
				z[i] = 1; //open facility
				L_value += (min_cost+ f[i]); // Update optimal solution
								
					
			}
			else {
				for (j = 0; j < M; j++) //on the other hand this variable takes value 0
					x[i][j] = 0; //Update x[i][j]==0 
			}
		}
		
		for (j = 0; j < M; j++)
			//sum of the lambdas (the price or duals variables )
			L_value -= lambda[j];  // udpate Optimal solution 
		 

			
			

		if (L_value > best_lower_bound)
		{ //Update best lower bound
			best_lower_bound = L_value;
			for (i = 0; i < N; i++)
				open_facility[i] = z[i];

			UB = Reassign_customers(); //Lagrangean heuristic to find UB
			if(UB != -1 && UB < upper_bound)
			  upper_bound = UB; 
		}
		
		printf(" %d  %3.2f  %0.2f  %0.2f  %0.2f \n",
			t, L_value, best_lower_bound, upper_bound, (upper_bound - best_lower_bound) / upper_bound * 100);
		
		//Compute subgradient vector (direction of movement)
		SqrNorm = 0;
		for (j = 0; j < M; j++)
		{
			for (i = 0; i < N; i++)
			{
				s[j] += x[i][j] ;
				
			}

			s[j] -= 1;
			SqrNorm += pow((double)s[j], 2);

		}
		//Check stoping criteria
		if ((upper_bound - best_lower_bound) / upper_bound * 100 < 0.01)
			break;
		//Compute steplength
		if (t - last > interv_dec_lambda)
		{
			last = t;
			epsilon = epsilon / 2;
		}
		if (epsilon < 0.1)
			epsilon = 2;
		StepLength = epsilon * (upper_bound - L_value) / SqrNorm;
		
		 //Update Lagrange multipliers
			for (j = 0; j < M; j++)
			{
				lambda[j] = lambda[j] + StepLength * s[j]; //the  corresponding  Lagrange  multipliers lambda are unrestricted in sign.
				
			}
		
	}
}