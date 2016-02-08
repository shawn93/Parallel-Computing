/* File:     structs2.c
 *
 * Purpose:  Illustrate one solution to problem of passing struct args
 * Input:    None
 * Output:   struct member values
 *
 * Compile:  gcc -g -Wall -o structs2 structs2.c
 * Usage:    ./structs2
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

   return 0;
}  /* main */

void pass_struct(struct element_s x) {

   printf("\nIn pass_struct\n");
   printf("value = %f\n", x.value);
   printf("row   = %d\n", x.row);
   printf("col   = %d\n", x.col);

}  /* pass_struct */
