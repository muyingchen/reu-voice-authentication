/* file chunk2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /************************************************************/
  /*                                                          */
  /*  void chunk_out2(fp,s,line_width,cc1,continuation_chr)   */
  /*  Writes *s out into file *fp in chunks no longer than    */
  /*  <line_width>, breaking lines with *continuation_chr.    */
  /*  If cc1 != NULL, puts *cc1 at the far left margin of     */
  /*  each line.                                              */
  /*                                                          */
  /************************************************************/
  void chunk_out2(FILE *fp, char *s, int line_width, char *cc1, char continuation_chr)
{char csave,*p,*q;
    int n;
  /* code */
if (db_level > 1) printf("%sin chunk_out2, s='%s'\n",pdb,s);
    if (s != NULL)
      {p = s;
       line_width -= strlen(cc1);
       while ((n=strlen(p)) > 0)
         {if (n > line_width)
            {for (q=p+line_width - 2;((q > p)&&(!isspace(*q))); q--);
             if (q <= p) q = p + line_width - 3; else q++;
             csave = *q;
             *q = '\0';
if (db_level > 1) printf("%snext chunk='%s'\n",pdb,p);
             if (cc1 == NULL) fprintf(fp,"%s%c\n",p,continuation_chr);
             else             fprintf(fp,"%s%s%c\n",cc1,p,continuation_chr);
             *q = csave;
             p = q;
            }
          else
            {if (cc1 == NULL) fprintf(fp,"%s\n",p);
             else             fprintf(fp,"%s%s\n",cc1,p);
             p = p+n;
      }  }  }
    return;
   } /* end chunk_out2() */
