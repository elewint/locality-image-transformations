/*
 *                      useuarray2bb.c
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <uarray2b.h>

typedef long number;

const int DIM1 = 1000;
const int DIM2 = 1200;
const int ELEMENT_SIZE = sizeof(number);
const int MARKER = 99;

void 
check_and_print(int i, int j, UArray2b_T a, void *p1, void *p2) 
{
        number *entry_p = p1;
        // printf("bool is %d\n", *((bool *)p2));
        // printf("trying [%d,%d]\n", i, j);
        *((bool *)p2) &= UArray2b_at(a, i, j) == entry_p;

        if ( (i == (DIM1 - 1) ) && (j == (DIM2 - 1) ) ) {
                /* we got the corner */
                *((bool *)p2) &= (*entry_p == MARKER);
        }
        // printf("  bool is %d\n", *((bool *)p2));
        
        // printf("ar[%d,%d] done\n", i, j);
        // printf("\n");
}

int
main(int argc, char *argv[])
{
        UArray2b_T test_array;
        bool OK = true;

        test_array = UArray2b_new(DIM1, DIM2, ELEMENT_SIZE, 12);

        OK = (UArray2b_width(test_array) == DIM1) &&
	     (UArray2b_height(test_array) == DIM2) &&
             (UArray2b_size(test_array) == ELEMENT_SIZE);

        /* Note: we are only setting a value on the corner of the array */
        *((number *)UArray2b_at(test_array, DIM1 - 1, DIM2 - 1)) = MARKER;

        printf("Trying block major\n");
        UArray2b_map(test_array, check_and_print, &OK);

        UArray2b_free(&test_array);

        printf("The array is %sOK!\n", (OK ? "" : "NOT "));

        (void)argc;
        (void)argv;
}
