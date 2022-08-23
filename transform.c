/**************************************************************
 *
 *                     transform.c
 *
 *     Assignment: locality
 *     Authors:  Eli Intriligator (eintri01), Katie Yang (zyang11)
 *     Date:     Oct 15, 2021
 *
 *     Summary
 *     Implementation of the transform interface. This program implements
 *     image rotations including 0, 90, 180, and 270 degrees clockwise,
 *     flip horizontally and vertically, and transpose.
 *
 **************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
#include "transform.h"

/* ArrayData stores the transformed array of pixels and the methods
 * suite. It is passed through mapping functions as the closure
 * so they have access to the output array and its functions
 */
struct ArrayData {
    A2Methods_UArray2 output_array;
    A2Methods_T methods;
};

/* transform
 * Purpose: Process the image transformation
 * Parameters: a Pnm_ppm for the input image, an int for the rotation degree,
 *             a C string for the flip type, an int for whether or not
 *             transpose should be performed, an A2Methods_T for the methods
 *             suite, and an A2Methods_mapfun ptr for the mapping function
 *             chosen by the user
 * Returns: the processed image as a Pnm_ppm
 *
 * Expected input: a valid ppm image and methods suite, as well as a non-null
 *                 map function pointer and degrees of 0, 90, 180, or 270
 * Success output: a processed Pnm_ppm that is transformed as desired
 * Failure output: if the input ppm, methods suite, or map are null, the
 *                 program will throw a Hanson-style CRE
 */
Pnm_ppm transform(Pnm_ppm input_ppm, int degrees, char *flip, int do_transpose,
                                    A2Methods_T methods, A2Methods_mapfun *map)
{
    assert(input_ppm && methods);
    assert (map != NULL && *map != NULL);

    /* Rotation */
    if (degrees != 0) {
        return rotate(input_ppm, degrees, methods, map);
    }
    /* Flip */
    else if (flip != NULL)
        if (strcmp(flip, "horizontal") == 0) {
            return flip_horizontal(input_ppm, methods, map);
        } else {
            return flip_vertical(input_ppm, methods, map);
        }
    /* Transpose */
    else if (do_transpose == 1) {
        return transpose(input_ppm, methods, map);
    /* 0 degree rotation – returns original image */
    } else {
        return input_ppm;
    }
}

/* rotate
 * Purpose: Rotate a given ppm by a given number of degrees (either 90,
 *          180, or 270)
 * Parameters: a Pnm_ppm for the input image, an int for degrees of rotation,
 *             an A2Methods_T for the methods suite, and an A2Methods_mapfun
 *             ptr for the mapping function chosen by the user
 * Returns: the rotate ppm image
 *
 * Expected input: a valid ppm image and methods suite, as well as a non-null
 *                 map function pointer
 * Success output: a processed Pnm_ppm that is rotated the desired amount
 * Failure output: if there is a rotation error, a CRE will be thrown from
 *                 one of the helper functions
 */
Pnm_ppm rotate(Pnm_ppm input_ppm, int degrees, A2Methods_T methods,
                                             A2Methods_mapfun *map)
{
    if (degrees == 90) {
        return rotate_90(input_ppm, methods, map);
    } else if (degrees == 180) {
        return rotate_180(input_ppm, methods, map);
    } else {
        return rotate_270(input_ppm, methods, map);
    }
}

/* rotate_90
 * Purpose: Rotate the image 90 degrees clockwise
 * Parameters: a Pnm_ppm for the input image, an A2Methods_T for the methods
 *             suite, and an A2Methods_mapfun for the mapping function choosen
 * Returns: the processed image
 *
 * Expected input: valid Pnm_ppm, A2Methods_T containing the method suite,
 *                 and a valid A2Methods_mapfun containing the choosen mapping
 *                 function
 * Success output: a processed Pnm_ppm
 * Failure output: CRE if array_data isn't valid
 */
Pnm_ppm rotate_90(Pnm_ppm input_ppm, A2Methods_T methods, A2Methods_mapfun *map)
{
    A2Methods_UArray2 input_array = input_ppm->pixels;
    A2Methods_UArray2 output_array;
    
    ArrayData array_data = malloc(sizeof(A2Methods_UArray2)
                                    + sizeof(A2Methods_T));
    assert(array_data);
    array_data->methods = methods;

    int width = input_ppm->width;
    int height = input_ppm->height;
    int size = methods->size(input_array);
    output_array = methods->new(height, width, size);
    array_data->output_array = output_array;
    
    map(input_array, apply90, &array_data);
    
    input_ppm->width = methods->width(output_array);
    input_ppm->height = methods->height(output_array);
    input_ppm->pixels = output_array;

    methods->free(&input_array);
    free(array_data);
    
    return input_ppm;
}

/* apply90
 *    Purpose: apply function for rotating the image 90 degrees clockwise
 *             - visits each cell and maps it to the appropriate cell
 *             in the output image
 * Parameters: an int for the column number, and int for the row number,
 *             an A2Methods_UArray2 for the original image, an A2Methods_Object
 *             for the current element being processed, and a void pointer
 *             closure which points to an ArrayData containing the output array
 *             and the methods suite
 *    Returns: void
 *
 * Expected input: two ints for column and row that are in bound for the
 *                 original image, a valid A2Methods_UArray2, a valid 
 *                 A2Methods_Object for the current element
 * Success output: none
 * Failure output: none
 */
void apply90(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
                                                                  void *cl)
{
    ArrayData array_data = *(ArrayData *)cl;
    A2Methods_UArray2 output_array = array_data->output_array;
    A2Methods_T methods = array_data->methods;

    int height = methods->height(array2);
    int new_i = height - j - 1;
    int new_j = i;

    *(Pnm_rgb)methods->at(output_array, new_i, new_j) = *(Pnm_rgb)ptr;
}

/* rotate_180
 *    Purpose: Rotate the image 180 degrees clockwise
 * Parameters: a Pnm_ppm for the input image, an A2Methods_T for the methods
 *             suite, and an A2Methods_mapfun for the mapping function choosen
 *    Returns: the processed image
 *
 * Expected input: valid Pnm_ppm, A2Methods_T containing the method suite,
 *                 and a valid A2Methods_mapfun containing the choosen mapping
 *                 function
 * Success output: a processed Pnm_ppm
 * Failure output: CRE if array_data isn't valid
 */
Pnm_ppm rotate_180(Pnm_ppm input_ppm, A2Methods_T methods,
                                    A2Methods_mapfun *map)
{
    A2Methods_UArray2 input_array = input_ppm->pixels;
    A2Methods_UArray2 output_array;
    
    ArrayData array_data = malloc(sizeof(A2Methods_UArray2)
                                     + sizeof(A2Methods_T));
    assert(array_data);
    array_data->methods = methods;

    int width = input_ppm->width;
    int height = input_ppm->height;
    int size = methods->size(input_array);
    output_array = methods->new(width, height, size);
    array_data->output_array = output_array;
    
    map(input_array, apply180, &array_data);
    
    input_ppm->pixels = output_array;

    methods->free(&input_array);
    free(array_data);
    
    return input_ppm;
}

/* apply180
 *    Purpose: apply function for rotating the image 180 degrees clockwise
 *             - visits each cell and maps it to the appropriate cell
 *             in the output image
 * Parameters: an int for the column number, and int for the row number,
 *             an A2Methods_UArray2 for the original image, an A2Methods_Object
 *             for the current element being processed, and a void pointer
 *             closure which points to an ArrayData containing the output array
 *             and the methods suite
 *    Returns: void
 *
 * Expected input: two ints for column and row that are in bound for the
 *                 original image, a valid A2Methods_UArray2, a valid 
 *                 A2Methods_Object for the current element
 * Success output: none
 * Failure output: none
 *
 */
void apply180(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
                                                                   void *cl)
{
    ArrayData array_data = *(ArrayData *)cl;
    A2Methods_UArray2 output_array = array_data->output_array;
    A2Methods_T methods = array_data->methods;

    int height = methods->height(array2);
    int width = methods->width(array2);
    int new_i = width - i - 1;
    int new_j = height - j - 1;

    *(Pnm_rgb)methods->at(output_array, new_i, new_j) = *(Pnm_rgb)ptr;
}

/* rotate_270
 *    Purpose: Rotate the image 270 degrees clockwise
 * Parameters: a Pnm_ppm for the input image, an A2Methods_T for the methods
 *             suite, and an A2Methods_mapfun for the mapping function choosen
 *    Returns: the processed image
 *
 * Expected input: valid Pnm_ppm, A2Methods_T containing the method suite,
 *                 and a valid A2Methods_mapfun containing the choosen mapping
 *                 function
 * Success output: a processed Pnm_ppm
 * Failure output: CRE if array_data isn't valid
 */
Pnm_ppm rotate_270(Pnm_ppm input_ppm, A2Methods_T methods,
                                    A2Methods_mapfun *map)
{
    A2Methods_UArray2 input_array = input_ppm->pixels;
    A2Methods_UArray2 output_array;
    
    ArrayData array_data = malloc(sizeof(A2Methods_UArray2)
                                     + sizeof(A2Methods_T));
    assert(array_data);
    array_data->methods = methods;

    int width = input_ppm->width;
    int height = input_ppm->height;
    int size = methods->size(input_array);
    output_array = methods->new(height, width, size);
    array_data->output_array = output_array;
    
    map(input_array, apply270, &array_data);
    
    input_ppm->width = methods->width(output_array);
    input_ppm->height = methods->height(output_array);
    input_ppm->pixels = output_array;

    methods->free(&input_array);
    free(array_data);

    return input_ppm;
}

/* apply270
 *    Purpose: apply function for rotating the image 270 degrees clockwise
 *             - visits each cell and maps it to the appropriate cell
 *             in the output image
 * Parameters: an int for the column number, and int for the row number,
 *             an A2Methods_UArray2 for the original image, an A2Methods_Object
 *             for the current element being processed, and a void pointer
 *             closure which points to an ArrayData containing the output array
 *             and the methods suite
 *    Returns: void
 *
 * Expected input: valid Pnm_ppm, A2Methods_T containing the method suite,
 *                 and a valid A2Methods_mapfun containing the choosen mapping
 *                 function
 * Success output: a processed Pnm_ppm
 * Failure output: CRE if array_data isn't valid
 */
void apply270(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
                                                                   void *cl)
{
    ArrayData array_data = *(ArrayData *)cl;
    A2Methods_UArray2 output_array = array_data->output_array;
    A2Methods_T methods = array_data->methods;

    int width = methods->width(array2);
    int new_i = j;
    int new_j = width - i - 1;

    *(Pnm_rgb)methods->at(output_array, new_i, new_j) = *(Pnm_rgb)ptr;
}

/* flip_vertical
 *    Purpose: Flip the image vertically
 * Parameters: a Pnm_ppm for the input image, an A2Methods_T for the methods
 *             suite, and an A2Methods_mapfun for the mapping function choosen
 *    Returns: the processed image
 *
 * Expected input: valid Pnm_ppm, A2Methods_T containing the method suite,
 *                 and a valid A2Methods_mapfun containing the choosen mapping
 *                 function
 * Success output: a processed Pnm_ppm
 * Failure output: CRE if array_data isn't valid
 */
Pnm_ppm flip_vertical(Pnm_ppm input_ppm, A2Methods_T methods, 
                                       A2Methods_mapfun *map)
{
    A2Methods_UArray2 input_array = input_ppm->pixels;
    A2Methods_UArray2 output_array;
    
    ArrayData array_data = malloc(sizeof(A2Methods_UArray2)
                                     + sizeof(A2Methods_T));
    assert(array_data);
    array_data->methods = methods;

    int width = input_ppm->width;
    int height = input_ppm->height;
    int size = methods->size(input_array);
    output_array = methods->new(width, height, size);
    array_data->output_array = output_array;
    
    map(input_array, apply_vertical, &array_data);
    
    input_ppm->width = methods->width(output_array);
    input_ppm->height = methods->height(output_array);
    input_ppm->pixels = output_array;

    methods->free(&input_array);
    free(array_data);

    return input_ppm;
}

/* apply_vertical
 *    Purpose: apply function for flipping the image vertically – visits each
 *             cell and maps it to the appropriate cell in the output image
 * Parameters: an int for the column number, and int for the row number,
 *             an A2Methods_UArray2 for the original image, an A2Methods_Object
 *             for the current element being processed, and a void pointer
 *             closure which points to an ArrayData containing the output array
 *             and the methods suite
 *    Returns: void
 *
 * Expected input: two ints for column and row that are in bound for the
 *                 original image, a valid A2Methods_UArray2, a valid 
 *                 A2Methods_Object for the current element
 * Success output: none
 * Failure output: none
 *
 */
void apply_vertical(int i, int j, A2Methods_UArray2 array2,
                           A2Methods_Object *ptr, void *cl)
{
    ArrayData array_data = *(ArrayData *)cl;
    A2Methods_UArray2 output_array = array_data->output_array;
    A2Methods_T methods = array_data->methods;

    int height = methods->height(array2);
    int new_i = i;
    int new_j = height - j - 1;

    *(Pnm_rgb)methods->at(output_array, new_i, new_j) = *(Pnm_rgb)ptr;
}

/* flip_horizontal
 *    Purpose: Flip the image horizontally
 * Parameters: a Pnm_ppm for the input image, an A2Methods_T for the methods
 *             suite, and an A2Methods_mapfun for the mapping function choosen
 *    Returns: the processed image
 *
 * Expected input: valid Pnm_ppm, A2Methods_T containing the method suite,
 *                 and a valid A2Methods_mapfun containing the choosen mapping
 *                 function
 * Success output: a processed Pnm_ppm
 * Failure output: CRE if array_data isn't valid
 */
Pnm_ppm flip_horizontal(Pnm_ppm input_ppm, A2Methods_T methods,
                                         A2Methods_mapfun *map)
{
    A2Methods_UArray2 input_array = input_ppm->pixels;
    A2Methods_UArray2 output_array;
    
    ArrayData array_data = malloc(sizeof(A2Methods_UArray2)
                                     + sizeof(A2Methods_T));
    assert(array_data);
    array_data->methods = methods;

    int width = input_ppm->width;
    int height = input_ppm->height;

    int size = methods->size(input_array);
    output_array = methods->new(width, height, size);
    array_data->output_array = output_array;
    
    map(input_array, apply_horizontal, &array_data);
    
    input_ppm->width = methods->width(output_array);
    input_ppm->height = methods->height(output_array);
    input_ppm->pixels = output_array;

    methods->free(&input_array);
    free(array_data);

    return input_ppm;
}

/* apply_horizontal
 *    Purpose: apply function for flipping the image horizontally – visits each
 *             cell and maps it to the appropriate cell in the output image
 * Parameters: an int for the column number, and int for the row number,
 *             an A2Methods_UArray2 for the original image, an A2Methods_Object
 *             for the current element being processed, and a void pointer
 *             closure which points to an ArrayData containing the output array
 *             and the methods suite
 *    Returns: void
 *
 * Expected input: two ints for column and row that are in bound for the
 *                 original image, a valid A2Methods_UArray2, a valid 
 *                 A2Methods_Object for the current element
 * Success output: none
 * Failure output: none
 *
 */
void apply_horizontal(int i, int j, A2Methods_UArray2 array2,
                             A2Methods_Object *ptr, void *cl)
{
    ArrayData array_data = *(ArrayData *)cl;
    A2Methods_UArray2 output_array = array_data->output_array;
    A2Methods_T methods = array_data->methods;

    int width = methods->width(array2);
    int new_i = width - i - 1;
    int new_j = j;

    *(Pnm_rgb)methods->at(output_array, new_i, new_j) = *(Pnm_rgb)ptr;
}

/* transpose
 *    Purpose: transpose the image
 * Parameters: a Pnm_ppm for the input image, an A2Methods_T for the methods
 *             suite, and an A2Methods_mapfun for the mapping function choosen
 *    Returns: the processed image
 *
 * Expected input: valid Pnm_ppm, A2Methods_T containing the method suite,
 *                 and a valid A2Methods_mapfun containing the choosen mapping
 *                 function
 * Success output: a processed Pnm_ppm
 * Failure output: CRE if array_data isn't valid
 */
Pnm_ppm transpose(Pnm_ppm input_ppm, A2Methods_T methods,
                                   A2Methods_mapfun *map)
{
    A2Methods_UArray2 input_array = input_ppm->pixels;
    A2Methods_UArray2 output_array;
    
    ArrayData array_data = malloc(sizeof(A2Methods_UArray2)
                                    + sizeof(A2Methods_T));
    assert(array_data);
    array_data->methods = methods;
    
    int width = input_ppm->width;
    int height = input_ppm->height;
    int size = methods->size(input_array);
    output_array = methods->new(height, width, size);
    array_data->output_array = output_array;
    
    map(input_array, apply_transpose, &array_data);
    
    input_ppm->width = methods->width(output_array);
    input_ppm->height = methods->height(output_array);
    input_ppm->pixels = output_array;
    
    methods->free(&input_array);
    free(array_data);

    return input_ppm;
}

/* apply_transpose
 *    Purpose: apply function for transposing the image – visits each cell
 *             and maps it to the appropriate cell in the output image
 * Parameters: an int for the column number, and int for the row number,
 *             an A2Methods_UArray2 for the original image, an A2Methods_Object
 *             for the current element being processed, and a void pointer
 *             closure which points to an ArrayData containing the output array
 *             and the methods suite
 *    Returns: void
 *
 * Expected input: two ints for column and row that are in bound for the
 *                 original image, a valid A2Methods_UArray2, a valid 
 *                 A2Methods_Object for the current element
 * Success output: none
 * Failure output: none
 */
void apply_transpose(int i, int j, A2Methods_UArray2 array2,
                            A2Methods_Object *ptr, void *cl)
{
    (void)array2;
    ArrayData array_data = *(ArrayData *)cl;
    A2Methods_UArray2 output_array = array_data->output_array;
    A2Methods_T methods = array_data->methods;

    *(Pnm_rgb)methods->at(output_array, j, i) = *(Pnm_rgb)ptr;
}