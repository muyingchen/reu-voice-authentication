/* file wphtal2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/****************************************************************/
/*  fcn write_phon_tally2(table,pcode,iselect,jselect,ndigits)  */
/*  Writes out the phone part of the tally table                */
/****************************************************************/

void write_phon_tally2(TALLY_STR *table, pcodeset *pcode, int *iselect, int *jselect, int ndigits, int style)
{int i,j,jcol,max_row_name_size,max_col_name_size,lcolx,left_margin;
  int lcol[MAX_PHONES], nmax_pr, lmarg, rmarg, nletts;
  float sf; long unsigned int max_phfreq;
  fprintf(stdout,"\n Symbol-to-Symbol Substitution Frequencies\n");
  fprintf(stdout," Symbol code set is defined in %s.\n\n",pcode->name);
  if (nselected(iselect,jselect,pcode) < 1)
    {fprintf(stdout,"(No rows or columns were selected.)\n");
     goto EXIT;
    }
/* calc max row and column header sizes */
  max_row_name_size = 0; max_col_name_size = 0;
  for (i=0; i<=pcode->ncodes; i++)
    {if (iselect[i])
       if (strlen(pcode->pc[i].ac) > max_row_name_size)
         max_row_name_size = strlen(pcode->pc[i].ac);
     j = i;
     if (jselect[j])
       if (strlen(pcode->pc[j].ac) > max_col_name_size)
         max_col_name_size = strlen(pcode->pc[j].ac);
    }
if (db_level > 0) printf("*DB: max_row_name_size = %d\n",max_row_name_size);
if (db_level > 0) printf("*DB: max_col_name_size = %d\n",max_col_name_size);

  left_margin = max_row_name_size + 2;
/* compute column sizes */
  if (style == 1)  /* constant column width style */
    {if (ndigits > max_col_name_size) lcolx = ndigits;
     else lcolx = max_col_name_size;
    }
  jcol = 0;
  for (j=0; j < pcode->ncodes; j++)
    {if (jselect[j])
       {jcol += 1;
        if (style == 1) lcol[jcol] = lcolx;
        else
          if (ndigits > strlen(pcode->pc[j].ac)) lcol[jcol] = ndigits;
          else lcol[jcol] = strlen(pcode->pc[j].ac);
    }  }
/* get normalization factor */
  max_phfreq = 0;
  for (i=0; i <= pcode->ncodes; i++)
   {if (iselect[i])
     {for (j=0; j <= pcode->ncodes; j++)
       {if (jselect[j])
         {if (table->phfreq[i][j] > max_phfreq)
            max_phfreq = table->phfreq[i][j];
   } } } }
if (db_level > 0) printf("*DB: max_phfreq=%u\n",max_phfreq);
  nmax_pr = 1; for (i=1; i <= ndigits; i++) {nmax_pr = nmax_pr * 10;} 
  nmax_pr -= 1;
  if (max_phfreq > 0)  sf = (float)nmax_pr / (float)max_phfreq;
  else                 sf = 1.0;
if (db_level > 0) printf("*DB: sf=%f\n",sf);
/* print confusion matrix */
  fprintf(stdout," Confusion matrix, range 0-%-*u:\n",ndigits,nmax_pr);
  fprintf(stdout," (True values multiplied by sf=%f)\n\n",sf);
/* column heading line */
  fprintf(stdout,"%*.*s",left_margin,left_margin,"                        ");
  jcol = 0;
  for (j=0; j <= pcode->ncodes; j++)
    {if (jselect[j])
      {jcol += 1;
       fprintf(stdout," "); /* always 1 space before a column */
       nletts = strlen(pcode->pc[j].ac);
       rmarg = (lcol[jcol] - nletts) / 2;
       lmarg = lcol[jcol] - nletts - rmarg;
       if (lmarg > 0) fprintf(stdout,"%*.*s",lmarg,lmarg,"        ");
       fprintf(stdout,"%*.*s",nletts,nletts,pcode->pc[j].ac);
       if (rmarg > 0) fprintf(stdout,"%*.*s",rmarg,rmarg,"        ");
    } }
/* rows and columns */
  for (i=0; i <= pcode->ncodes; i++)
    {if (iselect[i])
       {fprintf(stdout,"\n %*.*s:",max_row_name_size,max_row_name_size,
         pcode->pc[i].ac);
        jcol = 0;
        for (j=0; j <= pcode->ncodes; j++)
          {if (jselect[j])
             {jcol += 1;
              fprintf(stdout," ");
              rmarg = (lcol[jcol] - ndigits) / 2;
              lmarg = lcol[jcol] - ndigits - rmarg;
              if (lmarg > 0) fprintf(stdout,"%*.*s",lmarg,lmarg,"        ");
              fprintf(stdout,"%*u",ndigits,
                (unsigned) ((sf * (float)table->phfreq[i][j]) + 0.5));
              if (rmarg > 0) fprintf(stdout,"%*.*s",rmarg,rmarg,"        ");
    }  }  }  }
EXIT:
  fprintf(stdout,"\n\n");
 }
/* end function write_phon_tally2() */
/* end file wphtal2.c */
