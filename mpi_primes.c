/* File:     mpi_primes.c
 * Purpose:  Find all primes less than or equal to an input value 
 *
 * Input:    n:  integer > = 2 
 * Output:   Sorted list of primes between 2 and n.
 *
 * Compile:  mpicc -g -Wall -o mpi_primes mpi_primes.c -lm
 * Usage:    mpiexec -n <p> ./mpi_primes <n>
 *           p:  number of MPI processes
 *           n:  max int to test for primality
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>

const int STRING_MAX = 100000;

void Usage(char prog[]);
int Get_n(int argc, char* argv[], int my_rank, int p, MPI_Comm comm);
int Is_prime(int i);
void Print_primes(int my_primes[], int my_prime_count, int my_rank, int p, MPI_Comm comm);
void Merge_lists(int my_contrib[], int my_count, int** list_p, int* size_p, int my_rank, int p, MPI_Comm comm);
void Merge(int** primes_p, int* prime_count_p, int received_primes[], int received_count, int** temp_p);
void Compute_list_sizes(int prime_counts[], int recv_counts[], int p);
void Print_list(char* title, int list[], int n, int my_rank);

int main(int argc, char* argv[]) {
   int n, i, p, inc, local_n, my_rank;
   MPI_Comm comm;
   int* my_primes, my_prime_count=0;

   MPI_Init(&argc, &argv);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &p);
   MPI_Comm_rank(comm, &my_rank);

   n = Get_n(argc, argv, my_rank, p, comm);
   local_n = n/(2*p)+2;
   my_primes = malloc(local_n*sizeof(int));

   if (my_rank == 0) {
      my_primes[my_prime_count++] = 2;
   }

   inc = 2*p;

   for (i = 2*my_rank + 3; i <= n; i += inc) {
      if (Is_prime(i)) {
         my_primes[my_prime_count++] = i;

#        ifdef DEBUG
         printf("Proc %d > %d\n", my_rank, i);
#        endif
      }
   }

   Print_list("After search primes are", my_primes, my_prime_count, my_rank);

   Print_primes(my_primes, my_prime_count, my_rank, p, comm);

   free(my_primes);
   MPI_Finalize();
   return 0;
}  /* main */

/*-------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a brief message explaining how the program is run.
 *            The quit.
 * In arg:    prog:  name of the executable
 */
void Usage(char prog[]) {
   fprintf(stderr, "usage: %s <n>\n", prog);
   fprintf(stderr, "   n = max integer to test for primality\n");
   exit(0);
}  /* Usage */

/*-------------------------------------------------------------------
 * Function:    Get_n
 * Purpose:     Get the input value n
 * Input args:  argc:  number of command line args
 *              argv:  array of command line args
 */
int Get_n(int argc, char* argv[], int my_rank, int p,  MPI_Comm comm) {
   int n;

   if (my_rank == 0) {
      if (argc != 2)
         n = -1;
      else {
         n = strtol(argv[1], NULL, 10);
      }
   }

   MPI_Bcast(&n, 1, MPI_INT, 0, comm);

   /* Check for bogus input */
   if (n <= 1) {
      if (my_rank == 0) {
            Usage(argv[0]);
      }
      MPI_Finalize();
      exit(0);
   }
   return n;
}  /* Get_n */

/*-------------------------------------------------------------------
 * Function:   Is_prime
 * Purpose:    Determine whether the argument is prime
 * Input arg:  i
 * Return val: true (nonzero) if arg is prime, false (zero) otherwise
 */
int Is_prime(int i) {
   int j;

   for (j = 2; j <= sqrt(i); j++)
      if (i % j == 0)
         return 0;  /* Not prime */
   return 1;        /* Prime */
}  /* Is_prime */


/*-------------------------------------------------------------------
 * Function:    Print_primes
 * Purpose:     Print the primes found by the processes
 * Input args:  my_primes: the primes found by the current process
 *              my_prime_count:  the number of primes found by the
 *                 current process
 *              my_rank, p, comm:  the usual MPI variables.
 */
void Print_primes(int my_primes[], int my_prime_count, int my_rank, int p, MPI_Comm comm) {
   int* primes;
   int primes_count, i;

   Merge_lists(my_primes, my_prime_count, &primes, &primes_count,
         my_rank, p, comm);

   if (my_rank == 0) {
      printf("The primes are:\n");
      for (i = 0; i < primes_count; i++)
         printf("%d ", primes[i]);
      printf("\n");
      free(primes);
   }
   
}  /* Print_primes */


/*-------------------------------------------------------------------
 * Function:  Merge_lists
 * Purpose:   Merge a collection of sorted lists
 * In args:   my_slist:  the sorted list
 *            my_count:  the number of elements in the sorted list
 *            my_rank, p, comm: the MPI variables
 * Out args:  list_p:  the merged list
 *            size_p: number of elements in the merged list
 *
 */
void Merge_lists(int my_slist[], int my_count, int** list_p, int* size_p, int my_rank, int p, MPI_Comm comm) {

   int my_size, my_recv_size, done = 0, partner, i, curr_size;
   int* my_list = NULL, *recv_list = NULL, *temp = NULL;
   unsigned bitmask = 1;
   int *counts = malloc(p*sizeof(int));
   int *recv_counts = malloc(p*sizeof(int));
   MPI_Status status;  
   int recv_count=0;     
   MPI_Allgather(&my_count, 1, MPI_INT, counts, 1, MPI_INT, comm);
   Compute_list_sizes(counts, recv_counts, p);
   my_size = counts[my_rank];

   if (my_size > 0) 
      my_list = malloc(my_size*sizeof(int));
   if (my_size > 0) 
      temp = malloc(my_size*sizeof(int));

   my_recv_size = recv_counts[my_rank];

   if (my_recv_size > 0) 
      recv_list = malloc(my_recv_size*sizeof(int));

   for (i = 0; i < my_count; i++) {
      my_list[i] = my_slist[i];
   }

   curr_size = my_count;

   while (!done && bitmask < p) {
      partner = my_rank ^ bitmask;
      if (my_rank < partner && counts[partner] > 0) {
         if (partner < p) {
            MPI_Recv(recv_list, counts[partner], MPI_INT,
               partner, 0, comm, &status);
            recv_count = counts[partner];
            Merge(&my_list, &curr_size, recv_list, recv_count,
                  &temp);
            Print_list("After merge", my_list, curr_size, my_rank);
         }
         bitmask <<= 1;
      } 
      else {
         MPI_Send(my_list, my_size, MPI_INT, partner, 0, comm);
         done = 1;
      }
   }

   if (my_rank == 0) {
      *list_p = my_list;
      *size_p = my_size;
   } 
   else if (my_list != NULL)
      free(my_list);

   if (recv_list != NULL) 
      free(recv_list);
   if (temp != NULL) 
      free(temp);

} /* Merge_lists */

/*-------------------------------------------------------------------
 * Function:    Merge
 * Purpose:     Merge two sorted lists
 * In arg:      primes_p:  one of the sorted list
 *              prime_count_p:  the number of elements in the list
 *              received_primes: the other input sorted list 
 *              received_count:  the number of elements in the list
 *              temp_p:  temporary storage for the merged list
 */
void Merge(int** primes_p, int* prime_count_p, int received_primes[], int received_count, int** temp_p) {
   int *pp = *primes_p;
   int *tp = *temp_p;
   int i, j, k; 

   i = j = k = 0;
   while (i < *prime_count_p && j < received_count) 
      if (pp[i] <= received_primes[j]) {
         tp[k++] = pp[i++];
      }
      else {
         tp[k++] = received_primes[j++];
      }
   while (i < *prime_count_p) {
      tp[k++] = pp[i++];
   }
   while (j < received_count) {
      tp[k++] = received_primes[j++];
   }

   *primes_p = tp;
   *temp_p = pp;
   *prime_count_p = k;
}  /* Merge */

/*-------------------------------------------------------------------
 * Function:    Compute_list_sizes
 * Purpose:     Find the sizes of the lists of primes each process
 *              will store and compute the maximum size list each 
 *              process will receive.
 *
 * Input args:  prime_counts: the number of primes found by process
 *              recv_counts: the size each process received
 *              p:  the number of processes
 */
void Compute_list_sizes(int prime_counts[], int recv_counts[], int p) {
   unsigned bitmask;
   int partner, rank, inc;

   for (rank = 0; rank < p; rank++) {
      recv_counts[rank] = 0;
   }

   for (bitmask = 1, inc = 2; bitmask < p; bitmask <<= 1, inc *= 2) {
      for (rank = 0; rank < p; rank += inc) {
         partner = rank ^ bitmask;
         if (partner < p) {
            prime_counts[rank] += prime_counts[partner];
            if (recv_counts[rank] < prime_counts[partner]) {
               recv_counts[rank] = prime_counts[partner];
            }
         }
      }
   }
            
}  /* Compute_list_size */   

/*-------------------------------------------------------------------
 * Function:  Print_list
 * Purpose:   Convert a list of ints to a single string before
 *            printing.  This should make it less likely that the
 *            output is interrupted by another process.  This is
 *            mainly intended for debugging purposes.
 * In args:   title:  list's title
 *            list:  the ints to be printed
 *            n:  the number of ints
 *            my_rank:  the usual MPI variable
 */
void Print_list(char* title, int list[], int n, int my_rank) {
   char string[STRING_MAX];
   char* s_p;
   int i;

   sprintf(string, "Proc %d %s > ", my_rank, title);
   s_p = string + strlen(string);
   for (i = 0; i < n; i++) {
      sprintf(s_p, "%d ", list[i]);
      s_p = string + strlen(string);
   }

   printf("%s\n", string);
   fflush(stdout);
}  /* Print_list */