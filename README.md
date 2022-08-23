# COMP 40 hw03: Locality & Image Transformations
Eli Intriligator and Katie Yang

Date: Oct 15 2021

TA help acknowledgements:
    Tina, Ann-Marie, Liana, Alex, Miles, Devon, Gustavo

## Purpose
The main task of this program is to analyze the effects of
locality on CPU time. To measure these locality effects,
we perform different transformations on a Netpbm PPM image,
comparing the CPU time of different transformations, as well
as the difference between doing the transformations with
different mapping functions.

## Architecture
This program uses a methods suite A2Methods.h to support
polymorphic 2D unboxed arrays. The methods suite works with
two different representations of 2D arrays: UArray2, which
supports row-major and column-major mapping, and UArray2b,
which supports block-major mapping. The methods suite has a
type A2Methods_T, which is a pointer to a list of function
pointers that provide the core functionality for 2D arrays.
    
## Features implemented
ppmtrans
- Supports image transformations including 0, 90, 180, and 
   270 degrees clockwise, flip horizontally and vertically,
   and transpose
- The user is allowed to choose whether or not the rotation
   should be done in row, column, or block major

a2plain
- Store the image file data in a row or column
   major uarray2. Subclass for the method superclass

transform
- Supporting polymorphic manipulation of 2D arrays

## Known problems/limitations
We believe we have implemented all features correctly.

## Measured performance

| Mapping Functions | Total Time Taken (seconds) | Time Per Input Pixel (nanoseconds) |
|-------------------|----------------------------|------------------------------------|
| Column-major      |                            |                                    |
| 90                | 5.721                      | 114.562                            |
| 180               | 8.704                      | 174.310                            |
| 270               | 5.556                      | 111.257                            |
| Row-major         |                            |                                    |
| 90                | 5.031                      | 100.760                            |
| 180               | 2.472                      | 49.531                             |
| 270               | 5.052                      | 101.174                            |
| Block-major       |                            |                                    |
| 90                | 3.886                      | 77.815                             |
| 180               | 3.729                      | 74.677                             |
| 270               | 3.500                      | 70.092                             |

Info about these measurements:
- Image used: mobo.ppm
- Image size: 49,939,200 pixels (width 8160 and height 6120)
- Computer: Halligan computer (accessed via SSH)
- CPU: Intel(R) Xeon(R) Silver 4214Y CPU @ 2.20GHz
- Clock speed: 2194.844 MHz

## Explanations of locality differences 

The fastest result is a 180 degree rotation done with a row-major
mapping function. This is because in a 180 degree rotation, rows map to
rows and columns map to columns, so the locality benefits of the source
image are shared with the destination image (which is not true for
column-major and block-major mappings). In this case, row-major mapping
is fastest since the underlying plain UArray2 data structure is stored
in row-major order, so neighboring elements in the array will be
adjacent to each other in memory, meaning they can be accessed more
effectively by the cache.

Block major is the fastest for both 90 and 270 rotation because these
rotations are less efficient for column and row-major mappings. When
doing a 90 or 270 degree rotation with these non-blocked mapping
methods, reading a row in the source image results in writing a column
in the destination image and vice versa, so the locality benefits of a
row-major mapping are not shared between the source and destination
images. In contrast, a block-major mapping is more efficient since its
locality benefits are shared between source and destination, i.e. the
source image and destination image are both processed in block-major
order. This grants block-major mapping a greater cache hit ratio than
the other mapping methods.

Time spent on project:
    36 hrs as of submission
