/**************************************************************
 *
 *                     transform.h
 *
 *     Assignment: locality
 *     Authors:  Eli Intriligator (eintri01), Katie Yang (zyang11)
 *     Date:     Oct 15, 2021
 *
 *     Summary
 *     The transform interface.
 *
 **************************************************************/

#ifndef __TRANSFORM__
#define __TRANSFORM__

#include <stdio.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

typedef struct ArrayData *ArrayData;

Pnm_ppm transform(Pnm_ppm input_ppm, int degrees, char *flip, int transpose,
                                A2Methods_T methods, A2Methods_mapfun *map);

Pnm_ppm rotate(Pnm_ppm input_ppm, int degrees, A2Methods_T methods,
                                            A2Methods_mapfun *map);

Pnm_ppm rotate_90(Pnm_ppm input_ppm, A2Methods_T methods,
                                  A2Methods_mapfun *map);
void apply90(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
                                                                 void *cl);

Pnm_ppm rotate_180(Pnm_ppm input_ppm, A2Methods_T methods,
                                    A2Methods_mapfun *map);
void apply180(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
                                                                  void *cl);

Pnm_ppm rotate_270(Pnm_ppm input_ppm, A2Methods_T methods,
                                   A2Methods_mapfun *map);
void apply270(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
                                                                  void *cl);

Pnm_ppm flip_vertical(Pnm_ppm input_ppm, A2Methods_T methods,
                                      A2Methods_mapfun *map);
void apply_vertical(int i, int j, A2Methods_UArray2 array2,
                          A2Methods_Object *ptr, void *cl);

Pnm_ppm flip_horizontal(Pnm_ppm input_ppm, A2Methods_T methods,
                                        A2Methods_mapfun *map);
void apply_horizontal(int i, int j, A2Methods_UArray2 array2,
                            A2Methods_Object *ptr, void *cl);

Pnm_ppm transpose(Pnm_ppm input_ppm, A2Methods_T methods,
                                  A2Methods_mapfun *map);
void apply_transpose(int i, int j, A2Methods_UArray2 array2,
                           A2Methods_Object *ptr, void *cl);

#endif /* __TRANSFORM */