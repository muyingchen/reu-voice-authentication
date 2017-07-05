/* file chunk1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /************************************************************/
  /*                                                          */
  /*  void chunk_out(fp,s,line_width,continuation_chr)        */
  /*  Writes *s out into file *fp in chunks no longer than    */
  /*  <line_width>, breaking lines with *continuation_chr.    */
  /*                                                          */
  /************************************************************/
  void chunk_out(FILE *fp, char *s, int line_width, char continuation_chr)
{char csave,*p,*q;
    int n;
    if (s != NULL)
      {p = s;
       while ((n=strlen(p)) > 0)
         {if (n > line_width)
            {for (q=p+line_width - 2;((q > p)&&(!isspace(*q))); q--);
             if (q <= p) q = p + line_width - 3; else q++;
             csave = *q;
             *q = '\0';
             fprintf(fp,"%s%c\n",p,continuation_chr);
             *q = csave;
             p = q;
            }
          else
            {fprintf(fp,"%s\n",p);
             p = p+n;
      }  }  }
    return;
   } /* end chunk_out() */
