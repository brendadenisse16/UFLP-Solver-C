#include "def.h"

int        N, M;
double     **c, *f, *d, *b;
double     MAX_DOUBLE = 10000000000;
int        *open_facility;
int        *customer_assign;


int main (int argc, char *argv[])
{
 clock_t  start,end;
 double obj_value_ch, obj_value_ls, opt_value;
 double cputime;

 start = clock();
 read_instance("capc.txt");

 printf("Solving UFLP with CPLEX: \n");
 opt_value = solve_MIP();
 end = clock();
 cputime = (double)(end - start) / (double)CLOCKS_PER_SEC;   //Compute CPU time
 printf("Optimal solution value: %.2f    Time: %.5f seconds \n", opt_value, cputime);  
 Print_solution();

 //printf("Solving UFLP with Heuristic Procedure \n");
 //printf("\n Constructive phase: \n");
 //start = clock();
 //obj_value_ch = Constructive_Heuristic();
 //end = clock();
 //cputime = (double)(end - start) / (double)CLOCKS_PER_SEC;   //Compute CPU time
 //printf("Best objective value: %.2f  Deviation: %.2f    Time: %.5f seconds \n", obj_value_ch, (obj_value_ch - opt_value)/opt_value*100, cputime);  
 //Print_solution();
 //
 //printf("\n Local improvement phase: \n");
 //obj_value_ls = Local_Search_Heuristic(obj_value_ch);
 //end = clock();
 //cputime = (double)(end - start) / (double)CLOCKS_PER_SEC;   //Compute CPU time

 //printf("Best objective value: %.2f  Deviation: %.2f   Time: %.5f seconds \n", obj_value_ls, (obj_value_ls - opt_value)/opt_value*100, cputime);  
 //Print_solution();

LagrangeanRelaxation(opt_value);
//LagrangeanUflp(opt_value);


 free_memory();

 return 0;
}


void Print_solution(void)
{
 int i;
 printf("Set of open facilities: ");
 for(i=0;i<N;i++){
   if(open_facility[i] == 1)
	 printf("%d ", i+1);
 }
 printf("\n");
 /*printf("Assignment of customers to facilities: \n");
 for(j=0;j<M;j++)
   printf("Customer %d to facility %d \n", j+1, best_assign[j]+1);*/
}


double Local_Search_Heuristic(double current_obj)
{
 int flag = 1;
 
 while(flag) {
   flag = Close_Facility(&current_obj);
   if(flag == 0){
     flag = Open_Facility(&current_obj);
	 if(flag == 0)
	   flag = Open_Close_Facility(&current_obj);
   }
 }

 return current_obj;
}


int Close_Facility(double *current_obj)
{
 int i;
 double cost;
 int best_facility = -1;

 for(i=0;i<N;i++){
   if(open_facility[i] == 1){
	 open_facility[i] = 0;
	 cost = Reassign_customers();
	 if(cost < *current_obj){
	   *current_obj = cost;
	   best_facility = i;
	 }
	 open_facility[i] = 1;
   }
 }

 if(best_facility != -1){
   open_facility[best_facility] = 0;
   printf("Improved solution found in Close_Facility Neighbourhood \n Objectie value: %.2f \n",current_obj);
   Print_solution();
   return 1;
 }else
   return 0;
}


int Open_Facility(double *current_obj)
{
 int i;
 double cost;
 int best_facility = -1;

 for(i=0;i<N;i++){
   if(open_facility[i] == 0){
	 open_facility[i] = 1;
	 cost = Reassign_customers();
	 if(cost < *current_obj){
	   *current_obj = cost;
	   best_facility = i;
	 }
	 open_facility[i] = 0;
   }
 }

 if(best_facility != -1){
   open_facility[best_facility] = 1;
   printf("Improved solution found in Open_Facility Neighbourhood \n Objectie value: %.2f \n",current_obj);
   Print_solution();
   return 1;
 }else
   return 0;
}



int Open_Close_Facility(double *current_obj)
{
 int i1,i2;
 double cost;
 int best_open = -1;
 int best_closed = -1;

 for(i1=0;i1<N;i1++){
   if(open_facility[i1] == 0){
     for(i2=0;i2<N;i2++){
       if(open_facility[i2] == 1){
	     open_facility[i1] = 1;
		 open_facility[i2] = 0;
	     cost = Reassign_customers();
	     if(cost < *current_obj){
	       *current_obj = cost;
	       best_open = i1;
		   best_closed = i2;
	     }
	     open_facility[i1] = 0;
		 open_facility[i2] = 1;
	   }
	 }
   }
 }

 if(best_open != -1 && best_closed != -1){
   open_facility[best_open] = 1;
   open_facility[best_closed] = 0;
    printf("Improved solution found in Open_Close_Facility Neighbourhood \n Objectie value: %.2f \n",current_obj);
   Print_solution();
   return 1;
 }else
   return 0;
}


double Reassign_customers(void)
{
 int i,j;
 double cost, assign_cost;
 int flag = 0;

 for(j=0;j<M;j++){          //Assign each customer to its closest open facility
   customer_assign[j] = -1;
   assign_cost = MAX_DOUBLE;
   for(i=0;i<N;i++){
	 if(open_facility[i] == 1 && c[i][j] < assign_cost){
	   assign_cost = c[i][j];
	   customer_assign[j] = i;
	 }
   }
 }
 
 cost = 0;                  // Compute opbjective value of current solution 
 for(i=0;i<N;i++){
   if(open_facility[i] == 1){
     cost += f[i];
	 flag = 1;
   }
 }
 if(flag == 1){
   for(j=0;j<M;j++)
     cost += d[j]*c[customer_assign[j]][j];
   return cost;
 }
 else 
   return -1;
}








double Constructive_Heuristic(void)
{
 int i,j;
 int stoping_criteria, best_facility;
 double cost, best_cost, imp_cost;
 int *best_assign;
 int *imp_assign;
 int count=1;

 best_assign = create_int_vector(M);
 imp_assign = create_int_vector(M);

 best_cost = MAX_DOUBLE;
 for(i=0;i<N;i++)        //Initialize solution (empty solution = no factilities open)
   open_facility[i] = 0;
 for(j=0;j<M;j++)
	 best_assign[j] = -1;

 do{
   printf("Iteration %d \n", count++);
   printf("Current set of open facilities S= { ");
   for(i=0;i<N;i++){
     if(open_facility[i] == 1)
	 printf("%d ",i+1);
   }
   printf("}\n");
   if(count > 1)
	   printf("Current solution value: %.2f \n", best_cost);
   best_facility = -1;
   imp_cost = best_cost;

   for(i=0;i<N;i++){
     if(open_facility[i] == 0){   //Evaluate the benefit of opening a temporarily new facility areassigning customers
	   open_facility[i] = 1;
	   cost = Reassign_customers();	                        
	   printf("New objective value when adding facility %d to current solution: \t %10.2f \n",i+1,cost);
	   if(cost < imp_cost){       //Compare cost to best found solution so far
		 best_facility = i;
		 imp_cost = cost;
		 for(j=0;j<M;j++)
		  imp_assign[j] = customer_assign[j];
	   }

	   open_facility[i] = 0;      //Close facility that was temporarily open
	 }
   }
                               //If a new best solution is found, update current solution and continue
   if(best_facility != -1){
	   best_cost = imp_cost;
	   open_facility[best_facility] = 1;
	   for(j=0;j<M;j++)
		 best_assign[j] = imp_assign[j];
	   stoping_criteria = 0;
   }else
	   stoping_criteria = 1;

 }while(stoping_criteria != 1);


 free(best_assign);
 free(imp_assign);
 
 return best_cost;
}