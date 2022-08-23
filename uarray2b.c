/**************************************************************
 *
 *                     uarray2b.c
 *
 *     Assignment: locality
 *     Authors:  Eli Intriligator (eintri01), Katie Yang (zyang11)
 *     Date:     Oct 14, 2021
 *
 *     summary
 *     The blocked array. It groups elements in blocks.
 *
 *     Note
 *     The blocksize parameter counts the number of cells on 
 *     one side of a block. Some memory is wasted at the right 
 *     and bottom edges: not all the cells in those blocks are used.
 *     
 *
 **************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <uarray.h>
#include <uarray2.h>
#include <uarray2b.h>
#define T UArray2b_T

struct T {
    int width;
    int height;
    int size;
    int blocksize;
    int num_vert_blocks;
    int num_hort_blocks;
    
    UArray2_T block_array;
};

/*
* new blocked 2d array
* blocksize = square root of # of cells in block.
* blocksize < 1 is a checked runtime error
*/
extern T UArray2b_new (int width, int height, int size, int blocksize)
{
    assert(blocksize >= 1 && width >= 1 && height >= 1 && size > 0);
    
    int num_vert_blocks = ceil((float) height / (float) blocksize);
    int num_hort_blocks = ceil((float) width / (float) blocksize);

    UArray2_T block_array = UArray2_new(num_hort_blocks, num_vert_blocks,
                                                       sizeof(UArray_T));

    for (int i = 0; i < num_vert_blocks; i++) {
        for (int j = 0; j < num_hort_blocks; j++) {
            UArray_T new_array = UArray_new(blocksize * blocksize, size);
            *(UArray_T *)(UArray2_at(block_array, j, i)) = new_array;
        }
    }

    T array = malloc(sizeof(struct T));
    assert(array != NULL); 
    
    array->width = width;
    array->height = height;
    array->size = size;
    array->blocksize = blocksize;
    array->block_array = block_array;
    array->num_vert_blocks = ceil((float) height / (float) blocksize);
    array->num_hort_blocks = ceil((float) width / (float) blocksize);

    return array;
}

/* new blocked 2d array: blocksize as large as possible provided
*  block occupies at most 64KB (if possible)
*/
extern T UArray2b_new_64K_block(int width, int height, int size){
    int blocksize = sqrt(64 * 1024 / size);
    return UArray2b_new(width, height, size, blocksize);
}

extern void UArray2b_free (T *array2b)
{
    assert(array2b != NULL && *array2b != NULL);

    UArray2_T block_array = (*array2b)->block_array;

    int num_vert_blocks = (*array2b)->num_vert_blocks;
    int num_hort_blocks = (*array2b)->num_hort_blocks;

    for (int i = 0; i < num_vert_blocks; i++) {
        for (int j = 0; j < num_hort_blocks; j++) {
            UArray_T *target_uarray = (UArray_T *)UArray2_at(block_array, j, i);
            UArray_free(target_uarray);
        }
    }

    UArray2_free(&block_array);
    free(*array2b);
}

extern int UArray2b_width (T array2b)
{
    assert(array2b);
    return array2b->width;
}

extern int UArray2b_height (T array2b)
{
    assert(array2b);
    return array2b->height;
}

extern int UArray2b_size (T array2b)
{
    assert(array2b);
    return array2b->size;
}

extern int UArray2b_blocksize(T array2b)
{
    assert(array2b);
    return array2b->blocksize;
}

/* return a pointer to the cell in the given column and row.
* index out of range is a checked run-time error
*/
extern void *UArray2b_at(T array2b, int column, int row)
{
    assert (array2b);
    assert (column < array2b->width && column >= 0);
    assert (row < array2b->height && row >= 0);

    UArray2_T block_array = array2b->block_array;
    int blocksize = array2b->blocksize;
    
    int block_col = column / blocksize;
    int block_row = row / blocksize;

    /* Convert global column and row coords to coords within block */
    column %= blocksize;
    row %= blocksize;

    UArray_T target_uarray = *(UArray_T *)UArray2_at(block_array, block_col,
                                                                 block_row);
    
    return UArray_at(target_uarray, blocksize * row + column);
}

/* visits every cell in one block before moving to another block */
extern void UArray2b_map(T array2b,
                void apply(int col, int row, T array2b, void *elem, void *cl),
                                                                    void *cl)
{ 
    assert (array2b != NULL && apply != NULL);
    
    int num_vert_blocks = array2b->num_vert_blocks;
    int num_hort_blocks = array2b->num_hort_blocks;
    int width = array2b->width;
    int height = array2b->height;
    int blocksize = array2b->blocksize;
    UArray2_T block_array = array2b->block_array;
    
    for (int b_row = 0; b_row < num_vert_blocks; b_row++) {
        for (int b_col = 0; b_col < num_hort_blocks; b_col++) {
            
            UArray_T *target_uarray = (UArray_T *)UArray2_at(block_array,
                                                           b_col, b_row);
            int length = UArray_length(*target_uarray);

            for (int i = 0; i < length; i++) {
                int col = i % blocksize + b_col * blocksize;
                int row = i / blocksize + b_row * blocksize;

                if (col < width && row < height) {
                    void *curr_elem_p = UArray_at(*target_uarray, i);
                    apply(col, row, array2b, curr_elem_p, cl);
                }
            }
        }
    }
}

/*
* it is a checked run-time error to pass a NULL T
* to any function in this interface
*/

#undef T
