/* file smstr2.h */

#if !defined(SMSTR2_HEADER)
#define SMSTR2_HEADER

/*************************************************************************/
/* structure for tallying splits and merges:                             */
/* for each entry,                                                       */
/*   if s_or_m == 's', it's the split ival => jval kval                  */
/*   if s_or_m == 'm', it's the merge ival jval => kval                  */
/* nsaved is the number of entries saved.                                */
/* nover is the number of entries not saved (list too short).            */
/* entry[k] is a list of s & m entries w/frequency.                      */
/* iloc is a key to where in an alignment the s/m occurs, typically the  */
/*   first astr index.                                                   */
/* Parameterized by MAX_SMTAB_ENTRIES (typically 200)                    */
/*************************************************************************/

 typedef struct
   {int ival;
    int jval;
    int kval;
    char s_or_m;
    char l_or_r;
    int pdist;
    int pdist_sm;
    double fom;
    long unsigned int frequency;
    int iloc;
   } SM_ELEMENT2;

 typedef struct
   {int nsaved;
    int nover;
    SM_ELEMENT2 entry[MAX_SMTAB_ENTRIES];
   } SM_TAB_STR2;

#endif
