/* file repladd2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /*************************************************************************/
  /*  void report_lex_add2(i,pc1,perr)                                     */
  /*  Reports the addition of lexeme i to pcodeset *pc1 (into a file).     */
  /*  On exit, *perr == 0 means a.o.k.                                     */
  /*                    11 means ADD file couldn't be opened.              */
  /*************************************************************************/
  void report_lex_add2(int i, pcodeset *pc1, int *perr)
{char *proc = "report_lex_add2";
    static FILE *fp = NULL;
    static char fnx[LONG_LINE], *fname = &fnx[0];
    time_t t_sec;
/* code */
 db_enter_msg(proc,1);
    *perr = 0;
    if (fp == NULL) /* if file not open, try to open it */
      {fname = make_full_fname(fname,pc1->directory,pc1->file_name);
       fname = strcat(fname,".ADD");
       fname = expenv(fname,LONG_LINE);
       fp = fopen(fname,"a");
       if (fp == NULL) /* open fails, crap out */
         {fprintf(stderr,"*ERR:'%s' cannot open file '%s'\n",proc,fname);
          *perr = 11;
	 }
       else            /* open succeeds, add time stamp */
         {t_sec = time(&t_sec);
          fprintf(fp,"%s items added  %s:\n",
            pc1->comment_flag,del_eol(ctime(&t_sec)));
      }  }
   /* add item only to an open file */
    if (fp != NULL) fprintf(fp,"%s  %s\n",pc1->pc[i].ac,pc1->pc[i].lc_ext);
db_leave_msg(proc,1);
   return;
  }
/* end include repladd2.c */
