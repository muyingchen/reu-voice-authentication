/* file wphtal3.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/****************************************************************/
/*  fcn write_phon_tally3(table,pcode,iselect,jselect,ndigits)  */
/*  writes out the phone part of the tally table                */
/*  This version writes column headings vertically.             */
/****************************************************************/

void write_phon_tally3(TALLY_STR *table, pcodeset *pcode, int *iselect, int *jselect, int ndigits, int style)
{int i,j,k,ilet,max_row_name_size,max_col_name_size,left_margin;
  int nmax_pr, lmarg, rmarg;
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
/* get normalization factor */
  max_phfreq = 0;
  for (i=0; i <= pcode->ncodes; i++)
   {if (iselect[i])
     {for (j=0; j <= pcode->ncodes; j++)
       {if ((jselect[j])&&(i!=j))
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
  fprintf(stdout," (True values multiplied by sf=%f)\n",sf);
  fprintf(stdout," (Printing of diagonal cells suppressed.)\n\n");
/* column heading line */
  fprintf(stdout,"\n");
  rmarg = (ndigits - 1) / 2;
  lmarg = ndigits - 1 - rmarg;
  for (k=1; k <= max_col_name_size; k++)
    {
     fprintf(stdout,"%*.*s",left_margin,left_margin,"                       ");
     for (j=0; j <= pcode->ncodes; j++)
       {if (jselect[j])
          {fprintf(stdout," ");
           ilet = strlen(pcode->pc[j].ac) - (max_col_name_size - k);
           if (lmarg > 0) fprintf(stdout,"%*.*s",lmarg,lmarg,"        ");
           if (ilet > 0)
             fprintf(stdout,"%c",*(pcode->pc[j].ac + (ilet-1)));
           else fprintf(stdout," ");
           if (rmarg > 0) fprintf(stdout,"%*.*s",rmarg,rmarg,"        ");
       }  }
     printf("\n");
    }
/* rows and columns */
  for (i=0; i <= pcode->ncodes; i++)
    {if (iselect[i])
       {fprintf(stdout,"\n %*.*s:",max_row_name_size,max_row_name_size,
          pcode->pc[i].ac);
        for (j=0; j <= pcode->ncodes; j++)
          {if (jselect[j])
             {if (i==j)
                fprintf(stdout," %*u",ndigits,0);
              else
                fprintf(stdout," %*u",ndigits,
                (unsigned) ((sf * (float)table->phfreq[i][j]) + 0.5));
    }  }  }  }
EXIT:
  fprintf(stdout,"\n\n");
 }
/* end function write_phon_tally3() */
/* end file wphtal3.c */
