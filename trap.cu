/* File:     trap.cu
 * Purpose:  Implement the trapezoidal on a gpu using cuda.  This version
 *           uses a binary tree reduction in which we attempt to reduce
 *           thread divergence.  It also uses shared memory to store 
 *           intermediate results.  Assumes both threads_per_block and 
 *           blocks_per_grid are powers of 2.
 *
 * Compile:  nvcc  -arch=sm_21 -o trap trap.cu 
 * Run:      ./trap <n> <a> <b> <blocks> <threads_per_block>
 *              n is the number of trapezoids
 *              a is the left endpoint
 *              b is the right endpoint
 *
 * Input:    None
 * Output:   Result of trapezoidal applied to f(x).
 *
 * Notes:
 * 1.  The function f(x) = x^2 + 1 is hardwired
 */
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define MAX_BLOCK_SZ 512

/*-------------------------------------------------------------------
 * Function:    Dev_f
 * Purpose:     The function we're integrating
 * In arg:      x
 */
__device__ float Dev_f(float x) {
   return x*x + 1;
}  /* Dev_f */

/*-------------------------------------------------------------------
 * Function:    f
 * Purpose:     The function we're integrating
 * In arg:      x
 */
float f(float x) {
   return x*x + 1;
}  /* f */



/*-------------------------------------------------------------------
 * Function:    Dev_trap  (kernel)
 * Purpose:     Implement the trapezoidal rule
 * In args:     a, b, h, n
 * Out arg:     z
 *
 */
__global__ void Dev_trap(float a, float b, float h, int n, float z[]) {
   /* Use tmp to store each thread's trapezoid area */
   /* Can't use variable dimension here             */
   __shared__ float tmp[MAX_BLOCK_SZ];
   int t = blockDim.x * blockIdx.x + threadIdx.x;
   int loc_t = threadIdx.x;
   float my_a = a + t*h;
   
   if (t < n) tmp[loc_t] = 0.5*h*(Dev_f(my_a) + Dev_f(my_a+h));
   __syncthreads();

   /* This uses a tree structure to do the additions */
   for (int stride = blockDim.x/2; stride >  0; stride /= 2) {
      if (loc_t < stride)
         tmp[loc_t] += tmp[loc_t + stride];
      __syncthreads();
   }

   /* Store the result from this cache block in z[blockIdx.x] */
   if (threadIdx.x == 0) z[blockIdx.x] = tmp[0];
}  /* Dev_trap */    


/*-------------------------------------------------------------------
 * Host code 
 */
void Get_args(int argc, char* argv[], int* n_p, float* a_p, float* b_p,
      int* threads_per_block_p, int* blocks_p);
float Serial_trap(float a, float b, int n);
float Trap_wrapper(float a, float b, int n, float z_d[],
      int blocks, int threads);


/*-------------------------------------------------------------------
 * main
 */
int main(int argc, char* argv[]) {
   int n, threads_per_block, blocks;
   float a, b, *z_d, trap;
   double start, finish;  /* Only used on host */

   Get_args(argc, argv, &n, &a, &b, &threads_per_block, &blocks);
   cudaMalloc(&z_d, blocks*sizeof(float));

   GET_TIME(start);
   trap = Trap_wrapper(a, b, n, z_d, blocks, threads_per_block);
   GET_TIME(finish);

   printf("The area as computed by cuda is: %e\n", trap);
   printf("Elapsed time for cuda = %e seconds\n", finish-start);

   GET_TIME(start)
   trap = Serial_trap(a, b, n);
   GET_TIME(finish);
   printf("The area as computed by cpu is: %e\n", trap);
   printf("Elapsed time for cpu = %e seconds\n", finish-start);

   cudaFree(z_d);

   return 0;
}  /* main */


/*-------------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get and check command line args.  If there's an error
 *            quit.
 */
void Get_args(int argc, char* argv[], int* n_p, float* a_p, float* b_p,
      int* threads_per_block_p, int* blocks_p) {

   if (argc != 6) {
      fprintf(stderr, "usage: %s <n> <a> <b> <blocks> <threads per block>\n", 
            argv[0]);
      exit(0);
   }
   *n_p = strtol(argv[1], NULL, 10);
   *a_p = strtod(argv[2], NULL);
   *b_p = strtod(argv[3], NULL);
   *blocks_p = strtol(argv[4], NULL, 10);
   *threads_per_block_p = strtol(argv[5], NULL, 10);
}  /* Get_args */


/*-------------------------------------------------------------------
 * Function:  Trap_wrapper
 * Purpose:   CPU wrapper function for GPU trapezoidal rule
 * Note:      Assumes z_d has been allocated.
 */
float Trap_wrapper(float a, float b, int n, float z_d[], 
      int blocks, int threads) {
   int i;
   float trap = 0.0, h;
   float z_h[blocks];

   /* Invoke kernel */
   h = (b-a)/n;
   Dev_trap<<<blocks, threads>>>(a, b, h, n, z_d);
   cudaThreadSynchronize();

   cudaMemcpy(&z_h, z_d, blocks*sizeof(float), cudaMemcpyDeviceToHost);

   for (i = 0; i < blocks; i++)
      trap += z_h[i];
   return trap;
}  /* Trap_wrapper */


/*-------------------------------------------------------------------
 * Function:  Serial_dot
 * Purpose:   Compute a dot product on the cpu
 */
float Serial_trap(float a, float b, int n) {
   int i;
   float x, h, trap = 0;

   h = (b-a)/n;

   trap = (f(a) + f(b))/2.0;
   for (i = 1; i <= n-1; i++) {
       x = a + i*h;
       trap = trap + f(x);
   }
   trap = trap*h;
   
   return trap;
}  /* Serial_trap */
