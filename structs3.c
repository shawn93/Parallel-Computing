/* File:     structs3.c
 *
 * Purpose:  Illustrate second solution to problem of passing struct
 *           args -- using typedef
 * Input:    None
 * Output:   struct member values
 *
 * Compile:  gcc -g -Wall -o structs3 structs3.c
 * Usage:    ./structs3
 */
#include <stdio.h>

typedef struct {
   double value;
   int    row;
   int    col;
} element_t;  

void pass_struct(element_t x);

int main(void) {
   element_t element;

   element.value = 5.5;
   element.row = 6;
   element.col = 10;

   printf("element.value = %f\n", element.value);
   printf("element.row   = %d\n", element.row);
   printf("element.col   = %d\n", element.col);

   pass_struct(element);

   return 0;
}  /* main */

void pass_struct(element_t x) {
   printf("\nIn pass_struct\n");
   printf("x.value = %f\n", x.value);
   printf("x.row   = %d\n", x.row);
   printf("x.col   = %d\n", x.col);
}  /* pass_struct */
