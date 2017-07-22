 /* File gnalc1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void get_next_alignment_chr(cfp,palc1,p_at_end)                  */
 /*                                                                    */
 /* Gets the next alignment <*palc1> (in character string form) from   */
 /* file <*cfp>.  Returns *p_at_end == T iff file is at end-of-file    */
 /* Assumes that strings read in are already aligned.                  */
 /*                                                                    */
 /**********************************************************************/

 void get_next_alignment_chr(FILE *cfp, ALIGNMENT_CHR *palc1, int *p_at_end)
{
/* local data: */
  char *proc = "get_next_alignment";
  char sxx1[MAX_STR_NCHARS], *rec = &sxx1[0];
  SUBSTRING ss1x, *ss1 = &ss1x;
/* code: */
 db_enter_msg(proc,3); /* debug only */
/* initialize */
  init_al_chr(palc1);
/* go */
  if (! *p_at_end)
    {while ((rec = fgets(rec,MAX_STR_NCHARS,cfp)) != NULL)
       {if (*(rec + strlen(rec) - 1) != '\n')
          {fprintf(stderr,"*WARNING:%s: line w/o newline char,",proc);
           fprintf(stderr," probably truncated.\n  Line is:'%s'\n",rec);
          }
    /* see if line is an i.d. line */
        if (strncmp(rec,"id: ",4) == 0)
 	  {ss1->start = strchr(rec,'(')  + 1;
           ss1->end   = strrchr(rec,')') - 1;
           palc1->id = strcpy(palc1->id,substr_to_str(ss1,rec,LINE_LENGTH));
           continue;
	  }
    /* otherwise look for REF and HYP lines */
        if (strncmp(rec,"REF: ",5) == 0)
	  {palc1->pastr = strcpy(palc1->pastr,del_eol(strcutl(rec,5)));
           rec = fgets(rec,MAX_STR_NCHARS,cfp);
           if (*(rec + strlen(rec) - 1) != '\n')
             {fprintf(stderr,"*WARNING:%s: line w/o newline char,",proc);
              fprintf(stderr," probably truncated.\n  Line is:'%s'\n",rec);
             }
       	   if (strncmp(rec,"HYP: ",5) == 0)
	     {palc1->pbstr = strcpy(palc1->pbstr,del_eol(strcutl(rec,5)));
	      goto RETURN;
       }  }  }
    if (rec == NULL) *p_at_end = T;
    }
RETURN:
 db_leave_msg(proc,3); /* debug only */
  } /* end of function "get_next_alignment" */
