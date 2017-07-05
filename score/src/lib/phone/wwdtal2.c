/* file wwdtal2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/**************************************************/
/*  fcn write_word_tally2(table,pcode)            */
/*  writes out the word part of the tally table   */
/**************************************************/

void write_word_tally2(TALLY_STR *table, pcodeset *pcode)
{int i,j,k,lmaxi,lmaxj,lmaxf,lfreq;
  unsigned long fmax, xpow;
  fprintf(stdout,"\n Word-to-word Substitution Frequencies\n");
  fprintf(stdout,  " No. of different word-word substitutions saved = %ld\n",
     table->nsaved);
  fprintf(stdout,  " No. of word-word substitutions not saved = %ld\n",
     table->nover);
  if (table->nsaved < 1) goto RETURN;
/* calc widths of columns */
  lmaxi = 0; lmaxj = 0; fmax = 0;
  for (k = 0; k < table->nsaved; k++)
    {i = table->entry[k].ival;
     if (strlen(pcode->pc[i].ac) > lmaxi) lmaxi = strlen(pcode->pc[i].ac);
     j = table->entry[k].jval;
     if (strlen(pcode->pc[j].ac) > lmaxj) lmaxj = strlen(pcode->pc[j].ac);
     if (table->entry[k].frequency > fmax) fmax = table->entry[k].frequency;
    }
  xpow = 10;
  for (lfreq = 1; (xpow < fmax); lfreq++) {xpow = xpow * 10;}
  if (lmaxi < 3) lmaxi = 3;
  if (lmaxj < 3) lmaxj = 3;
  if (lfreq < 3) lmaxf = lfreq + 3; else lmaxf = lfreq + 2;
/* write column headings */
  fprintf(stdout,"\n");
  fprintf(stdout," %*s    %-*s%*s\n",lmaxi,"REF",lmaxj,"HYP",lmaxf,"FREQ");
/* write table entries */
  for (k = 0; k < table->nsaved; k++)
    {i = table->entry[k].ival;
     j = table->entry[k].jval;
     fprintf(stdout," %*s => %-*s  %*lu\n",lmaxi,pcode->pc[i].ac,lmaxj,
       pcode->pc[j].ac,lfreq,table->entry[k].frequency);
    }
  fprintf(stdout,"\n");
RETURN:
  return;
 }
/* end function write_word_tally2() */
/* end file wwdtal2.c */
