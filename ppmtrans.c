/**************************************************************
 *
 *                     ppmtrans.c
 *
 *     Assignment: locality
 *     Authors:  Eli Intriligator (eintri01), Katie Yang (zyang11)
 *     Date:     Oct 15, 2021
 *
 *     Summary
 *     This program transforms an image with rotations including 
 *     0, 90, 180, and 270 degrees clockwise, flip horizontally 
 *     and vertically, and transpose.
 *     
 *     Note
 *     If no rotation angle is provided, 0 will be the default
 *     Example commands:
 *     ./ppmtrans -rotate 270 -row-major -time time.txt in.ppm
 *     ./ppmtrans -transpose -block-major -time time.txt in.ppm
 *     
 **************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "transform.h"
#include "cputiming.h"

FILE * open_file(char *filename);
void write_timefile(FILE *output_fp, char *filename, Pnm_ppm image,
                                                 double time_used);

/* SET_METHODS
 * Purpose: Set the method and mapping function for the transformation
 * Parameters: a A2Methods_T for the method suite containing the
 *             correct function pointers, a function pointer to the
 *             mapping method that will be used, and a C string
 *             representation of the chosen mapping method
 * Expected input: valid mapping method
 * Success output: none
 * Failure output: message to standard error if mapping choosen is not valid
 */
#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
                                                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

/* usage
 * Purpose: Write to standard error if there is issues with command line
 * Parameters: a char pointer for the parameter that is causing the problem
 * Returns: void exit(1)
 *
 * Expected input: a valid char pointer for the parameter that is causing the
 *                 problem
 * Success output: A message written to standard error
 * Failure output: none
 */
static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        char *filename       = NULL;
        int   rotation       = 0;
        char *flip           = NULL;
        int   transpose      = 0;
        int   i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        for (i = 1; i < argc; i++) {
            /* change mapping function to reflect user input */
            if (strcmp(argv[i], "-row-major") == 0) {
                SET_METHODS(uarray2_methods_plain, map_row_major, 
                                                    "row-major");
            } else if (strcmp(argv[i], "-col-major") == 0) {
                SET_METHODS(uarray2_methods_plain, map_col_major, 
                                                 "column-major");
            } else if (strcmp(argv[i], "-block-major") == 0) {
                SET_METHODS(uarray2_methods_blocked, map_block_major,
                                                      "block-major");
            /* check for rotation value */
            } else if (strcmp(argv[i], "-rotate") == 0) {
                if (!(i + 1 < argc)) {      /* no rotate value */
                    usage(argv[0]);
                }
                char *endptr;
                rotation = strtol(argv[++i], &endptr, 10);
                if (!(rotation == 0 || rotation == 90 ||
                    rotation == 180 || rotation == 270)) {
                    fprintf(stderr, 
                        "Rotation must be 0, 90 180 or 270\n");
                    usage(argv[0]);
                }
                if (!(*endptr == '\0')) {    /* Not a number */
                    usage(argv[0]);
                }
            /* check for flips */
            } else if (strcmp(argv[i], "-flip") == 0) {
                if (!(i + 1 < argc)) {      /* no rotate value */
                    usage(argv[0]);
                }

                flip = argv[++i];
                    
                if (!(strcmp(flip, "horizontal") == 0 ||
                        strcmp(flip, "vertical") == 0)) {
                    fprintf(stderr, 
                    "Flip must be horizontal or vertical\n");
                    usage(argv[0]);
                }
            /* check for transpose */
            } else if (strcmp(argv[i], "-transpose") == 0) {
                    transpose = 1;
            /* check if going to use -time */
            } else if (strcmp(argv[i], "-time") == 0) {
                time_file_name = argv[++i];      
            /* exceptions handling */
            } else if (*argv[i] == '-') {
                fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                                            argv[i]);
                usage(argv[0]);
            } else if (argc - i > 1) {
                printf("argc is %d\n", argc);
                printf("at %d\n", i);
                fprintf(stderr, "Too many arguments\n");
                usage(argv[0]);
            } else if (i == argc - 1) {
                filename = argv[argc - 1];
            } else {
                break;
            }
        }

        FILE *input_fp = open_file(filename);
        FILE *output_fp = NULL;

        CPUTime_T timer = CPUTime_New();

        Pnm_ppm image = Pnm_ppmread(input_fp, methods);

        if (time_file_name != NULL) {
            CPUTime_Start(timer);
            image = transform(image, rotation, flip, transpose, methods, map);
            CPUTime_Stop(timer);

            double time_used = CPUTime_Stop(timer);
            CPUTime_Free(&timer);

            output_fp = fopen(time_file_name, "a");
            write_timefile(output_fp, filename, image, time_used);
            fclose(output_fp);
        } else {
            image = transform(image, rotation, flip, transpose, methods, map);
        }

        Pnm_ppmwrite(stdout, image);
        
        fclose(input_fp);
        Pnm_ppmfree(&image);

        return(0);
}

/* open_file
 * Purpose: Determine where data should be read from. From file if a file
 *          name is specified, otherwise from standard input
 * Parameters: a char pointer for the name of input file
 * Returns: a file pointer to where the data should be read from
 *
 * Expected input: a char pointer containing a file name if a file is
 *          provided, or else the pointer should be NULL if reading from
 *          standard in
 * Success output: CPU speed information appended to the time file
 * Failure output: message written to stderr and exit_failure if a file name
 *                 is provided but the file cannot be oppened. Hanson C.R.E
 *                 would be raised if the file pointer is NULL.
 */
FILE *open_file(char *filename)
{
    /*
     * Set file pointer to stdin to handle command line input in the case
     * that no file is supplied
     */
    FILE *fp;
    fp = stdin;

    if (filename != NULL) {
        fp = fopen(filename, "r");
        if (fp == NULL) {
            fprintf(stderr,"file failed to open\n");
            exit(EXIT_FAILURE);
        }
    }

    assert(fp != NULL);
    
    return fp;
}

/* write_timefile
 * Purpose: Write the time file that contains original image information
 *          and time spent associated with the image transformation
 * Parameters: a file pointer for the output file, a char pointer to the
 *             file that contains original image file,  a Pnm_ppm for the 
 *             input image, and a double used for recording the time used
 * Returns: void
 *
 * Expected input: a valid file pointer, a valid file name that isn't NULL,
 *                 the pnm_ppm for the transformed image, and a double
 *                 containing the time spent on rotation
 * Success output: CPU speed information appended to the time file
 * Failure output: none
 */
void write_timefile(FILE *output_fp, char *filename, Pnm_ppm image,
                                                  double time_used)
{
    int width = image->width;
    int height = image->height;
    int total_size = width * height;

    fprintf(output_fp, "For file \"%s\":\n", filename);
    fprintf(output_fp, "    Image has width %d and height %d\n", width, height);
    fprintf(output_fp, "    Width * height = %d\n", total_size);
    fprintf(output_fp, "    Recorded time: %f nanoseconds\n", time_used);
    fprintf(output_fp, "    Time per input pixel: %f nanoseconds\n\n", 
                                       time_used / (float)total_size);
}