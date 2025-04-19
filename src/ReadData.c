#include "def.h"

extern int N, M;
extern double** c, * f, * d, * b;
extern double MAX_DOUBLE;
extern int* open_facility;
extern int* customer_assign;


// Function used to read the input parameters of the instance being solved

void read_instance(const char* name)
{
    int i, j;
    FILE* in;

    in = open_file(name, "r");

    if (fscanf(in, "%d %d", &N, &M) != 2)
    {
        fprintf(stderr, "ERROR: Cannot read instance size \n");
        exit(1);
    }
    printf("Instance size: N:%d M:%d \n", N, M);

    Initialize_memory();

    for (i = 0; i < N; i++)
    {
        if (fscanf(in, "%lf %lf", &b[i], &f[i]) != 2)
        {
            fprintf(stderr, "ERROR: Cannot read capacities and set-up cots \n");
            exit(1);
        }
        f[i] = f[i] * 10; // Used for capc.txt
    }

    for (j = 0; j < M; j++)
    {
        if (fscanf(in, "%lf", &d[j]) != 1)
        {
            fprintf(stderr, "ERROR: Can't read demand \n");
            exit(1);
        }
        for (i = 0; i < N; i++)
        {
            if (fscanf(in, "%lf", &c[i][j]) != 1)
            {
                fprintf(stderr, "ERROR: Can't read distance \n");
                exit(1);
            }
            // c[i][j] /= 7000; // Used for cap72.txt
        }
    }

    fclose(in);
}

//Function used to allocate memory to arrays based on the size of the input parameter of the specific instance being solved

void Initialize_memory(void)
{
    c = create_double_matrix(N, M);
    f = create_double_vector(N);
    d = create_double_vector(M);
    b = create_double_vector(N);
    open_facility = create_int_vector(N);
    customer_assign = create_int_vector(M);
    
}

//Function used to release memory of the arrays based on the size of the input parameter of the specific instance being solved

void free_memory(void)
{
    int i;

    for (i = 0; i < N; i++)
        free(c[i]);

    free(c);
    free(b);
    free(f);
    free(d);
    free(open_facility);
    free(customer_assign);
   
}

// Function used to open data file

FILE* open_file(const char* name, const char* mode)
{
    FILE* file;

    if ((file = fopen(name, mode)) == NULL)
    {
        printf("\nError: Failed to open data file \n");
        exit(8);
    }
    return file;
}

// Functions to allocate memory to one and two dimensional arrays

int** create_int_matrix(int rows, int Columns)
{
    int i;
    int** ptr;

    if ((ptr = (int**)calloc(rows, sizeof(int*))) == NULL)
    {
        printf("\nError: Insuficient memory \n");
        exit(8);
    }
    for (i = 0; i < rows; i++)
        ptr[i] = create_int_vector(Columns);
    return ptr;
}

double** create_double_matrix(int rows, int Columns)
{
    int i;
    double** ptr;

    if ((ptr = (double**)calloc(rows, sizeof(double*))) == NULL)
    {
        printf("\nError: Insuficient memory \n");
        exit(8);
    }
    for (i = 0; i < rows; i++)
    {
        ptr[i] = create_double_vector(Columns);
    }
    return ptr;
}

int* create_int_vector(int dim)
{
    int* ptr;

    if ((ptr = (int*)calloc(dim, sizeof(int))) == NULL)
    {
        printf("\nError: Insuficient memory \n");
        exit(8);
    }
    return ptr;
}

double* create_double_vector(int dim)
{
    double* ptr;

    if ((ptr = (double*)calloc(dim, sizeof(double))) == NULL)
    {
        printf("\nError: Insuficient memory \n");
        exit(8);
    }
    return ptr;
}

// CPLEX functions to allocate memeory to arrays

void i_vector(int** vector, int n, char* s)
{
    if ((*vector = (int*)calloc(n, sizeof(int))) == NULL)
        //error(s);
        printf("Error: Insuficient memory \n");
    return;
}

void d_vector(double** vector, int n, char* s)
{
    if ((*vector = (double*)calloc(n, sizeof(double))) == NULL)
        // error(s);
        printf("Error: Insuficient memory \n");
    return;
}

void c_vector(char** vector, int n, char* s)
{
    if ((*vector = (char*)calloc(n, sizeof(char))) == NULL)
        //error(s);
        printf("Error: Insuficient memory \n");
    return;
}
