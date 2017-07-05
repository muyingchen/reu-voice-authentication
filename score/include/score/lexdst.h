/**********************************************************************/
/*                                                                    */
/*             FILENAME: mkwrdtbl.c                                   */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This program will hold structure definitions to   */
/*                  use a lexicon distance matrix                     */
/*                                                                    */
/**********************************************************************/
#define FROM_FILE	TRUE
#define ALLOC_MEM	FALSE
#define MAX_DIST_VAL	511


typedef struct lexicon_word_dist_srtuct{
    char *file_name;                 /* filename of the distance matrix */
    int is_in_memory;                /* Bool flag to signify if it's in */
                                     /* memory other wise use random access */
    short num_words;                 /* number of words in the lexicon +1 */
                                     /* for the insertion and deletion cells*/
    short scale_factor;              /* 1000 the original scale factor */
    FILE *fp;                        /* FILE pointer to the file for access */
    int fp_is_open;                  /* TRUE if the file has been opened */
    int hdr_size;                    /* num of bytes to skip in data file */
    int cur_fp_pos;                  /* current fp byte position */
    unsigned char *dist_matr;        /* Row Major ordering of 2dim arr */

    char (*dist_funct)(struct lexicon_word_dist_srtuct *,
		       int, int);    /* function to return the distance */
    char (*stor_dist)(struct lexicon_word_dist_srtuct *,
		      int, int, int);/* store the new value in the cell */
                                     /* value distance */
} LEX_DIST;


#define cell(sz,r,c)   ((r*sz) + c)



