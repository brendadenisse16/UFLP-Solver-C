#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include "cplex.h"

#define getrandom( min, max ) ((rand() % (int) (((max)+1)-(min)))+(min))
#define true 1
#define false 0
#define NOT_AVAIL -1
#define NONE -1


 void LagrangeanRelaxation(double upper_bound);
 void LagrangeanUflp(double upper_bound);
 double solve_MIP(void);
 void Print_solution(void);
 void read_instance(const char *);
 double Constructive_Heuristic(void);
 double Reassign_customers(void);
 double Local_Search_Heuristic(double);
 int Close_Facility(double *);
 int Open_Facility(double *);
 int Open_Close_Facility(double *);
 FILE *open_file(const char *, const char *);
 void free_memory(void);
 void Initialize_memory(void);
 void i_vector(int **vector,int n,char *s);
 void d_vector(double **vector,int n,char *s);
 void c_vector(char **vector,int n,char *s);
 int **create_int_matrix(int, int);
 double **create_double_matrix (int, int);
 int *create_int_vector(int);
 double *create_double_vector(int);

