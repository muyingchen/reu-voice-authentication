/*************************************************************************/
/* file talstr1.h                                                        */
/* structure for tallying alignment results:                             */
/* phtot is the total number of phone-phone tallies.                     */
/* phfreq[i][j] is a table for phone-phone frequencies.                  */
/* nsaved is the number of word-word correspondences saved in entry[].   */
/* nover is the number of word-word correspondences not saved.           */
/* entry[k] is a list of word-word correspondences w/frequency.          */
/*************************************************************************/

#ifndef TALSTR1_HEADER
#define TALSTR1_HEADER

 typedef struct xz
   {int ival;
    int jval;
    long unsigned int frequency;
   } FELEMENT;

 typedef struct xy
   {long unsigned int phtot;
    long unsigned int phfreq[MAX_PHONES][MAX_PHONES];
    long unsigned int nsaved;
    long unsigned int nover;
    FELEMENT entry[MAX_FHIST_SAVED_VALUES];
   } TALLY_STR;

#endif
/* end of file talstr1.h */
