 /* File gnalc2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void get_next_aln_chr2(fp_ref,fp_hyp,palc1,p_at_end)             */
 /*                                                                    */
 /* Gets the next alignment <*palc1> (in character string form) from   */
 /* files <*fp_ref> and <*fp_hyp>.  Returns *p_at_end == T iff one of  */
 /* the files is at end-of-file.                                       */
 /* Assumes that strings read in are already aligned.                  */
 /*                                                                    */
 /**********************************************************************/

 void get_next_aln_chr2(FILE *fp_ref, FILE *fp_hyp, ALIGNMENT_CHR *palc1, int *p_at_end)
{
/* local data: */
  char *proc = "get_next_aln_chr2";
/* code: */
 db_enter_msg(proc,3); /* debug only */
/* initialize */
  init_al_chr(palc1);
/* go */
  if (! *p_at_end)
    {palc1->pastr = fgets(palc1->pastr,MAX_STR_NCHARS,fp_ref);
     palc1->pbstr = fgets(palc1->pbstr,MAX_STR_NCHARS,fp_hyp);
     if ( (palc1->pastr == NULL) || (palc1->pbstr == NULL) )
       *p_at_end = T;
     else
       {palc1->pastr = del_eol(palc1->pastr);
        palc1->pbstr = del_eol(palc1->pbstr);
      } /* end "while (( palc1->pastr = fgets ... " */
    if (palc1->pastr == NULL) *p_at_end = T;
    }
 db_leave_msg(proc,3); /* debug only */
  } /* end of function "get_next_aln_chr2" */
/* end of file "gnalc2.c" */
