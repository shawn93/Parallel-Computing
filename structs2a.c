/* File:     structs2a.c
 *
 * Purpose:  Determine whether structs are passed by value or reference
 * Input:    None
 * Output:   struct member values
 *
 * Compile:  gcc -g -Wall -o structs2a structs2a.c
 * Usage:    ./structs2a
 */
#include <stdio.h>

struct element_s {
   double value;
   int    row;
   int    col;
};  /* Don't forget this semi-colon! */

void pass_struct(struct element_s x);

int main(void) {
   struct element_s element;

   element.value = 5.5;
   element.row = 6;
   element.col = 10;

   printf("element.value = %f\n", element.value);
   printf("element.row   = %d\n", element.row);
   printf("element.col   = %d\n", element.col);

   pass_struct(element);

   printf("After pass_struct\n");
   printf("element.value = %f\n", element.value);
   printf("element.row   = %d\n", element.row);
   printf("element.col   = %d\n", element.col);

   return 0;
}  /* main */

void pass_struct(struct element_s x) {

   printf("\nIn pass_struct\n\n");
   x.value = 3.14159;
   x.row = 2;
   x.col = 7;
}  /* pass_struct */
