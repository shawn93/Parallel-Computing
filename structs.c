/* File:     structs.c
 *
 * Purpose:  Illustrate direct definition of struct variable
 * Input:    None
 * Output:   struct member values
 *
 * Compile:  gcc -g -Wall -o structs structs.c
 * Usage:    ./structs
 */
#include <stdio.h>

int main(void) {
   /* Store value, row and column of an element */
   /* in a two-dimensional array of doubles     */
   struct {
      double value;
      int    row;
      int    col;
   } element;

   element.value = 5.5;
   element.row = 6;
   element.col = 10;

   printf("element.value = %f\n", element.value);
   printf("element.row   = %d\n", element.row);
   printf("element.col   = %d\n", element.col);

   return 0;
}  /* main */
