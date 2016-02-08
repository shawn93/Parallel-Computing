/* File:     
 *     pth_mat_vect_rand_cyc.c 
 *
 * Purpose:  
 *     Computes a parallel matrix-vector product.  Matrix and the
 *     vector y are logically distributed using a cyclic distribution.  
 *     This version uses a random number generator to generate A and x.
 *
 * Input:
 *     none
 *
 * Output:
 *     y: the product vector
 *     Elapsed time for the computation
 *
 * Compile:  
 *    gcc -g -Wall -o pth_mat_vect_rand pth_mat_vect_rand.c -lpthread
 * Usage:
 *    ./pth_mat_vect_rand <thread_count> <m> <n>
 *
 * Notes:  
 *     1.  Local storage for A, x, y is dynamically allocated.
 *     2.  We use a 1-dimensional array for A and compute subscripts
 *         using the formula A[i][j] = A[i*n + j]
 *     3.  Distribution of A, x, and y is logical:  all three are 
 *         globally shared.
 *     4.  Compile with -DDEBUG for information on generated data
 *         and product.
 *
 * Performance:  We obtained the following run-times for this program
 * and the program pth_mat_vect_rand.c, which uses a block distribution.
 * The programs were run on a node of the penguin cluster with -O3
 * optimization.  Times are in milliseconds:
 *
 *          1 thread      2 threads     4 threads
 * Matrix   Block Cyclic  Block Cyclic  Block Cyclic
 * 8M x 8   370   370     210   400     150   620
 * 8K x 8K  340   340     180   180     120   120
 * 8 x 8M   440   430     290   350     370   380
 *
 * The times for the block distribution are very close to the times
 * we obtained in class.  (See the file pth_mat_vect_times.pdf.)
 * When there's only one thread, the code for the cyclic distribution 
 * and the block distribution is, effectively, identical, and the
 * two versions have the same run times.  However, when there is
 * more than one thread, the block and the cyclic distributions
 * differ considerably for the 8,000,000 x 8 and the 8 x 8,000,000
 * matrices.  The run-times for the cyclic distribution are much
 * worse.  This is to be expected:  In general, the cache performance of 
 * a vector with a cyclic distribution should be worse than the
 * performance with a block distribution.  If the block size is
 * >= 8 (on x86), then loading a typical line will result in
 * 1 miss and 7 hits, regardless of the number of threads.  However,
 * with a cyclic distribution and 2 threads, loading a typical
 * line will result in 1 miss and 3 hits;  with 4 threads, it
 * will result in 1 miss and 1 hit:
 * 
 *    2 threads 0 1 2 3 4 5 6 7
 *      Thread  0 1 0 1 0 1 0 1
 *              m   h   h   h
 *
 *    4 threads 0 1 2 3 4 5 6 7
 *      Thread  0 1 2 3 0 1 2 3
 *              m       h    
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

/* Global variables */
int     thread_count;
int     m, n;
double* A;
double* x;
double* y;

/* Serial functions */
void Usage(char* prog_name);
void Gen_matrix(double A[], int m, int n);
void Read_matrix(char* prompt, double A[], int m, int n);
void Gen_vector(double x[], int n);
void Read_vector(char* prompt, double x[], int n);
void Print_matrix(char* title, double A[], int m, int n);
void Print_vector(char* title, double y[], double m);

/* Parallel function */
void *Pth_mat_vect(void* rank);

/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles;
   double start, finish;

   if (argc != 4) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   m = strtol(argv[2], NULL, 10);
   n = strtol(argv[3], NULL, 10);

#  ifdef DEBUG
   printf("thread_count =  %d, m = %d, n = %d\n", thread_count, m, n);
#  endif

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   A = malloc(m*n*sizeof(double));
   x = malloc(n*sizeof(double));
   y = malloc(m*sizeof(double));
   
   srandom(1);
   Gen_matrix(A, m, n);
#  ifdef DEBUG
   Print_matrix("We generated", A, m, n); 
#  endif

   Gen_vector(x, n);
#  ifdef DEBUG
   Print_vector("We generated", x, n); 
#  endif

   GET_TIME(start);
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
         Pth_mat_vect, (void*) thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);
   GET_TIME(finish);

#  ifdef DEBUG
   Print_vector("The product is", y, m); 
#  endif
   printf("Elapsed time = %e seconds\n", finish - start);

   free(A);
   free(x);
   free(y);
   free(thread_handles);

   return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count> <m> <n>\n", prog_name);
   exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function:    Read_matrix
 * Purpose:     Read in the matrix
 * In args:     prompt, m, n
 * Out arg:     A
 */
void Read_matrix(char* prompt, double A[], int m, int n) {
   int             i, j;

   printf("%s\n", prompt);
   for (i = 0; i < m; i++) 
      for (j = 0; j < n; j++)
         scanf("%lf", &A[i*n+j]);
}  /* Read_matrix */

/*------------------------------------------------------------------
 * Function: Gen_matrix
 * Purpose:  Use the random number generator random to generate
 *    the entries in A
 * In args:  m, n
 * Out arg:  A
 */
void Gen_matrix(double A[], int m, int n) {
   int i, j;

   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         A[i*n+j] = random()/((double) RAND_MAX);
}  /* Gen_matrix */

/*------------------------------------------------------------------
 * Function: Gen_vector
 * Purpose:  Use the random number generator random to generate
 *    the entries in x
 * In arg:   n
 * Out arg:  A
 */
void Gen_vector(double x[], int n) {
   int i;
   for (i = 0; i < n; i++)
      x[i] = random()/((double) RAND_MAX);
}  /* Gen_vector */

/*------------------------------------------------------------------
 * Function:        Read_vector
 * Purpose:         Read in the vector x
 * In arg:          prompt, n
 * Out arg:         x
 */
void Read_vector(char* prompt, double x[], int n) {
   int   i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++) 
      scanf("%lf", &x[i]);
}  /* Read_vector */


/*------------------------------------------------------------------
 * Function:       Pth_mat_vect
 * Purpose:        Multiply an mxn matrix by an nx1 column vector
 * In arg:         rank
 * Global in vars: A, x, m, n, thread_count
 * Global out var: y
 */
void *Pth_mat_vect(void* rank) {
   long my_rank = (long) rank;
   int i;
   int j; 

   for (i = my_rank; i < m; i += thread_count) {
      y[i] = 0.0;
      for (j = 0; j < n; j++)
          y[i] += A[i*n+j]*x[j];
   }

   return NULL;
}  /* Pth_mat_vect */


/*------------------------------------------------------------------
 * Function:    Print_matrix
 * Purpose:     Print the matrix
 * In args:     title, A, m, n
 */
void Print_matrix( char* title, double A[], int m, int n) {
   int   i, j;

   printf("%s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++)
         printf("%6.3f ", A[i*n + j]);
      printf("\n");
   }
}  /* Print_matrix */


/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void Print_vector(char* title, double y[], double m) {
   int   i;

   printf("%s\n", title);
   for (i = 0; i < m; i++)
      printf("%6.3f ", y[i]);
   printf("\n");
}  /* Print_vector */
