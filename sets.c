/* File:     sets.c
 * Purpose:  Implement a set of nonnegative ints with union, intersection, 
 *           and set difference.  The sets are implemented with sorted, singly
 *           linked lists.
 *
 * Compile:  gcc -g -Wall -o sets sets.c
 * Run:      ./sets
 *
 * Input:    A sequence of one character commands:
 *              'q' or 'Q' to quit
 *              'u' or 'U' to take the union of two sets
 *              'i' or 'I' to take the intersection of two sets
 *              'd' or 'D' to take the difference of two sets
 *           Union, intersection, and difference request input
 *           of two sets A and B.  The sets should be lists of
 *           nonnegative ints sorted into increasing order.  A
 *           negative value indicates the end of an input list.
 *
 * Output:   For union, intersection, and difference, the result
 *           of the operation.
 *
 * Notes:
 * 1.  Only the one character commands are checked for correctness.
 *     It is assumed that each input set is a list of distinct ints 
 *     sorted into increasing order.
 */   
#include <stdio.h>
#include <stdlib.h>

typedef struct listnode_s {
   int val;
   struct listnode_s* next_p;
} set_t;

char Get_command(void);
set_t* New_node(int val);
set_t* Append_node(set_t** head_pp, set_t* tail_p, int val);
set_t* Read_set(char name[]);
void Print_set(set_t* C_p, char name[]);
set_t* Union(set_t* A_p, set_t* B_p);
set_t* Intersection(set_t* A_p, set_t* B_p);
set_t* Difference(set_t* A_p, set_t* B_p);
set_t* Free_set(set_t* X_p);
void Free_all_sets(set_t** A_pp, set_t** B_pp, set_t** C_p);

int main(void) {
   set_t  *A_p = NULL, *B_p = NULL, *C_p = NULL;
   char   command;

   command = Get_command();
   while (command != 'q' && command != 'Q') {
      A_p = Read_set("A");
      B_p = Read_set("B");
      switch (command) {
         case 'u':
         case 'U':
            C_p = Union(A_p, B_p);
            Print_set(C_p, "C");
            Free_all_sets(&A_p, &B_p, &C_p);
            break;
         case 'i':
         case 'I':
            C_p = Intersection(A_p, B_p);
            Print_set(C_p, "C");
            Free_all_sets(&A_p, &B_p, &C_p);
            break;
         case 'd':
         case 'D':
            C_p = Difference(A_p, B_p);
            Print_set(C_p, "C");
            Free_all_sets(&A_p, &B_p, &C_p);
            break;
         default:
            printf("There is no %c command\n", command);
            printf("Please try again\n");
      } /* switch */
      command = Get_command();
   }  /* while */

   return 0;
}  /* main */


/*--------------------------------------------------------------------*/
/* Function:  Get_command
 * Purpose:   Read in a single char
 * Args:      None
 * Ret val:   The character read in
 */
char Get_command(void) {
   char c;

   printf("Please enter a command (u, i, d, q):  ");
   scanf(" %c", &c);
   return c;
}  /* Get_command */ 


/*--------------------------------------------------------------------*/
/* Function:  New_node
 * Purpose:   Create a new node for the linked list
 * In args:   val:  the int stored in the new node
 * Ret val:   pointer to the new node
 * Note:      The next_p field is set to NULL
 */
set_t* New_node(int val) {
   set_t *temp_p = malloc(sizeof(set_t));
   temp_p->val = val;
   temp_p->next_p = NULL;
   return temp_p;
}  /* New_node */


/*--------------------------------------------------------------------*/
/* Function: Append_node
 * Purpose:  Append a new node with val member val to the list
 *           with head *head_pp and tail tail_p
 * In arg:   val:  the value in the node to be appended
 *           tail_p:  pointer to the last node in the current list
 * In/Out arg:  head_pp:  If the list is empty, *head_pp will
 *           be updated to refer to the new node.  Otherwise,
 *           it will be unchanged.
 * Ret val:  pointer to the new last node in the updated list.
 *
 * Note:     This is ordinarily called with syntax 
 *
 *              tail_p = Append_node(&head_p, tail_p, val);
 */
set_t* Append_node(set_t** head_pp, set_t* tail_p, int val) {
   set_t* temp_p;

   temp_p = New_node(val);
   if (*head_pp == NULL) { /* Adding first node in list */
      *head_pp = temp_p;
      tail_p = temp_p;
   } else { /* There's already a node in the list */
      tail_p->next_p = temp_p;
      tail_p = temp_p;
   }
   return tail_p;
}  /* Append_node */


/*--------------------------------------------------------------------*/
/* Function:  Read_set
 * Purpose:   Read in a sorted list of non-negative ints, terminated
 *            by a negative int.
 * In Args:   name:  the name of the set being entered
 * Ret val:   Pointer to the linked list representing the set that
 *            was entered.
 * Note:      There is no error-checking of the input.
 */
set_t* Read_set(char name[]) {
   /* s_p refers to the head of the list */
   set_t *s_p = NULL; 
   /* tail_p refers to the tail of the list */
   set_t *tail_p = NULL;
   int val;

   printf("Enter the elements of the set %s in increasing order.\n", name);
   printf("There should be no repetitions.\n");
   printf("End input by entering a negative value.\n");

   scanf("%d", &val);
   while (val >= 0) {
      tail_p = Append_node(&s_p, tail_p, val);
      scanf("%d", &val);
   }  /* while */
   
   return s_p;
}  /* Read_set */


/*--------------------------------------------------------------------*/
/* Function:  Print_set
 * Purpose:   Print the elements of a set
 * In Args:   C_p:  pointer to the set 
 *            name: the name of the set being printed
 */
void Print_set(set_t* C_p, char name[]) {
   set_t* curr_p = C_p;

   printf("Set %s = {", name);
   while (curr_p != NULL) {
      printf("%d", curr_p->val);
      curr_p = curr_p->next_p;
      if (curr_p != NULL)
         printf(", ");
   }

   printf("}\n");
}  /* Print_set */


/*--------------------------------------------------------------------*/
/* Function:  Union
 * Purpose:   Form the union of the input sets
 * In Args:   A_p: pointer to the first set 
 *            B_p: pointer to the second set
 * Ret val:   Pointer to the union.
 */
set_t* Union(set_t* A_p, set_t* B_p) {
   /* new_p points to the first node in the union */
   set_t* new_p = NULL;
   /* tail_p points to the last node in the union */
   set_t* tail_p = NULL;
   /* a_p, b_p point to the current nodes in A_p, B_p, respectively */
   set_t *a_p = A_p, *b_p = B_p;

   while (a_p != NULL && b_p != NULL)
      if (a_p->val < b_p->val) {
         tail_p = Append_node(&new_p, tail_p, a_p->val);
         /* tail_p is pointing at the last node in the new list */
         a_p = a_p->next_p;
      } else if (b_p->val < a_p->val) {
         tail_p = Append_node(&new_p, tail_p, b_p->val);
         /* tail_p is pointing at the last node in the new list */
         b_p = b_p->next_p;
      } else { // a_p->val == b_p->val
         tail_p = Append_node(&new_p, tail_p, a_p->val);
         /* tail_p is pointing at the last node in the new list */
         a_p = a_p->next_p;
         b_p = b_p->next_p;
      }
   
   /* At most one of a_p, b_p is not NULL */
   while (a_p != NULL) {
      tail_p = Append_node(&new_p, tail_p, a_p->val);
      /* tail_p is pointing at the last node in the new list */
      a_p = a_p->next_p;
   }
   while (b_p != NULL) {
      tail_p = Append_node(&new_p, tail_p, b_p->val);
      /* tail_p is pointing at the last node in the new list */
      b_p = b_p->next_p;
   }
      
   return new_p;
}  /* Union */


/*--------------------------------------------------------------------*/
/* Function:   Intersection
 * Purpose:    Form the intersection of the two sets referred to by
 *             A_p and B_p
 * In args:    A_p, B_p:  the sets whose intersection is being found
 * Ret val:    Pointer to the intersection
 */
set_t* Intersection(set_t* A_p, set_t* B_p) {
   /* new_p points to the first node in the intersection */
   set_t* new_p = NULL;
   /* tail_p points to the last node in the intersection */
   set_t* tail_p = NULL;
   /* a_p, b_p point to the current nodes in A_p, B_p, respectively */
   set_t *a_p = A_p, *b_p = B_p;

   while (a_p != NULL && b_p != NULL)
      if (a_p->val < b_p->val) {
         a_p = a_p->next_p;
      } else if (b_p->val < a_p->val) {
         b_p = b_p->next_p;
      } else { // a_p->val == b_p->val
         tail_p = Append_node(&new_p, tail_p, a_p->val);
         /* tail_p is pointing at the last node in the new list */
         a_p = a_p->next_p;
         b_p = b_p->next_p;
      }
   
   return new_p;
}  /* Intersection */


/*--------------------------------------------------------------------*/
set_t* Difference(set_t* A_p, set_t* B_p) {
   /* new_p points to the first node in the intersection */
   set_t* new_p = NULL;
   /* tail_p points to the last node in the intersection */
   set_t* tail_p = NULL;
   /* a_p, b_p point to the current nodes in A_p, B_p, respectively */
   set_t *a_p = A_p, *b_p = B_p;

   while (a_p != NULL && b_p != NULL)
      if (a_p->val < b_p->val) {
         /* Add a_p->val to A-B */
         tail_p = Append_node(&new_p, tail_p, a_p->val);
         a_p = a_p->next_p;
      } else if (b_p->val < a_p->val) {
         /* Just go forward */
         b_p = b_p->next_p;
      } else { // a_p->val == b_p->val
         /* a_p-> val is in B, discard */
         a_p = a_p->next_p;
         b_p = b_p->next_p;
      }
   
   while (a_p != NULL) {
      tail_p = Append_node(&new_p, tail_p, a_p->val);
      /* tail_p is pointing at the last node in the new list */
      a_p = a_p->next_p;
   }

   return new_p;
}  /* Difference */


/*--------------------------------------------------------------------*/
/* Function:   Free_set
 * Purpose:    Free the memory referenced by X_p
 * In arg:     X_p:  head of the list
 * Ret val:    NULL
 * Note:       This function is ordinarily called with the following
 *             syntax:
 *
 *                   X_p = Free_set(X_p);
 */
set_t* Free_set(set_t* X_p) {
   set_t* curr_p = X_p;
   set_t* temp_p;

   curr_p = X_p;
   while (curr_p != NULL) {
      temp_p = curr_p;
      curr_p = curr_p->next_p;
      free(temp_p);     
   }

   X_p = NULL;
   return X_p;
} /* Free_set */


/*--------------------------------------------------------------------*/
/* Function:     Free_all_sets
 * Purpose:      Free the memory used by *A_pp, *B_pp, and *C_pp
 * In/out args:  A_pp, B_pp, C_pp:  the sets
 */
void Free_all_sets(set_t** A_pp, set_t** B_pp, set_t** C_pp) {
   *A_pp = Free_set(*A_pp);
   *B_pp = Free_set(*B_pp);
   *C_pp = Free_set(*C_pp);
}  /* Free_all_sets */
