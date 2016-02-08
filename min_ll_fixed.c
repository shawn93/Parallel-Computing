/* File:     min_ll_fixed.c
 * Purpose:  Create and print a linked list of ints.
 *
 * Compile:  gcc -g -Wall -o min_ll_fixed min_ll_fixed.c
 * Run:      ./min_ll_fixed
 *
 * Input:    List of positive ints terminated with an int <= 0
 * Output:   The positive ints in the list
 *
 * Notes:
 *   1. The program is supposed to add new nodes to the head of
 *      the list.  For example, if the user enters 1 2 3 0,
 *      the list should be
 *
 *       head_p -> 3 -> 2 -> 1  
 *       
 *   2. The original version was broken because it was using 
 *      the stack for the storage for the new nodes.  This
 *      memory is returned to the stack when the function
 *      returns, and it is probably overwritten by any 
 *      subsequent function calls.  So when the Print function
 *      is called, it's likely that the contents of the head_p
 *      pointer is garbage.
 */
#include <stdio.h>
#include <stdlib.h>

struct list_node_s {
   int data;
   struct list_node_s* next_p;
};

struct list_node_s* Create_node(int val, struct list_node_s* node_p);
void Print(struct list_node_s* head_p);

int main(void) {
   struct list_node_s* head_p = NULL;
   int val;

   /* Add new nodes to the head of the list */
   printf("Enter an int, <= 0 to stop\n");
   scanf("%d", &val);
   while (val > 0) {
      head_p = Create_node(val, head_p);
      scanf("%d", &val);
   }    

   Print(head_p);

   return 0;
}  /* main */


/*-----------------------------------------------------------------
 * Function:    Create_node
 * Purpose:     Return a pointer to a new struct list_node_s
 * Input args:  val:  value to put in data field
 *              node_p:  address to put in next_p field
 */
struct list_node_s* Create_node(int val, struct list_node_s* node_p) {
   struct list_node_s* temp_p = 
      malloc(sizeof(struct list_node_s));

   temp_p->data = val;
   temp_p->next_p = node_p;
   return temp_p;
}  /* Create_node */

/*-----------------------------------------------------------------
 * Function:   Print
 * Purpose:    Print list on a single line of stdout
 * Input arg:  head_p
 */
void Print(struct list_node_s* head_p) {
   struct list_node_s* curr_p = head_p;

   printf("list = ");
   while (curr_p != NULL) {
      printf("%d ", curr_p->data);
      curr_p = curr_p->next_p;
   }  
   printf("\n");
}  /* Print */
