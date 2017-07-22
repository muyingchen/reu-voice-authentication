/* file wrppf2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/**********************************************/
/* function write_ppf2(pctab,ipc,fname,perr)  */
/* and its special functions and structures.  */
/* Write pcode symbol-to-symbol function file */
/* Outputs a table based on pcodeset # <ipc>  */
/* of the pcode table <pctab>, into           */
/* file <fname>.                              */
/*                                            */
/* *perr  meaning                             */
/*   0    a.o.k.                              */
/*  12    ncodesets < 1                       */
/*  13    ipc < 1                             */
/*  14    ipc > ncodesets                     */
/*  15    too many table entries to write out */
/*  16    error on open  of output file       */
/*  17    error on close of output file       */
/*                                            */
/* Refers to external constants:              */
/*   MAX_PHONES                               */
/*   MAX_FHIST_SAVED_VALUES                   */
/**********************************************/


/* special structures: */

 typedef struct
   {int value;
    unsigned int frequency;
   } FELEMENT_PPF;
 typedef struct
   {int nsaved;
    FELEMENT_PPF entry[MAX_FHIST_SAVED_VALUES+1];
   } FREQ_HISTO_PPF;


/* local functions: */

 static void tally(int d, FREQ_HISTO_PPF *table)
{int i; boolean tallied;
  tallied = F;
/* if we already have the d value, tally 1 into its pot */
  for (i = 0; ((!tallied)&&(i < table->nsaved)); i++)
    {if (d == table->entry[i].value)
       {table->entry[i].frequency += 1;
        tallied = T;
    }  }
/* otherwise, put it in with a tally of 1 */
  if (!tallied)
    {if (table->nsaved < MAX_FHIST_SAVED_VALUES)
       {table->nsaved += 1;
        table->entry[table->nsaved-1].value = d;
        table->entry[table->nsaved-1].frequency = 1;
       }
     else
       fprintf(stdout,"*ERR: PPF table ovf.\n");
 }  }


void write_ppf2(TABLE_OF_CODESETS *pctab, int ipc, char *fname, int *perr)
{char *proc = "write_ppf2";
  FILE *fp_out;
  pcodeset *pcode;
  ppfcn symbol_symbol_fcn, *ppf = &symbol_symbol_fcn;
  int i,imax,j,d,dmin,dmax;
  long n_out;
  FREQ_HISTO_PPF dval_freq_hist_table, *dtable = &dval_freq_hist_table;
  unsigned int freq_max;
  int idb,jdb;
  FELEMENT_PPF db_entry_temp;
  time_t t_sec;
/* forward declaration */
/*  void tally();*/
/* code: */

 db_enter_msg(proc,0); /* debug only */
 if (db_level > 0) printf("*DB: ipc=%d\n",ipc);

  *perr = 0;
  t_sec = time(&t_sec);      /* ANSI, Turbo C */
/* error checks */
  if (pctab->ncodesets < 0)
    {fprintf(stderr,"*ERR: No pcodesets were in the pcodeset file.\n");
     *perr = 12; goto EXIT;
    }
  if (ipc < 0)
    {fprintf(stderr,"*ERR: You have asked me to write a table based on\n");
     fprintf(stderr," pcodeset number %d.\n",ipc);
     *perr = 13; goto EXIT; 
    }
  if (ipc > pctab->ncodesets)
    {fprintf(stderr,"*ERR: You have asked me to write a table based on\n");
     fprintf(stderr," pcodeset number %d, but there are only %d pcodesets\n",
         ipc,pctab->ncodesets);
     fprintf(stderr," in the pcodeset table.\n");
     *perr = 14; goto EXIT; 
    }

  pcode = pctab->cd[ipc].pc;

/********************************************************************
  fprintf(stdout,"\n The codeset defining the symbols is %s.\n",
       pcode->name);
  fprintf(stdout," Type is '%s', max %d entries,  %d actual entries.\n",
       pctab->cd[ipc].ptype, pctab->cd[ipc].nmax,pcode->ncodes);
  fprintf(stdout,"\n");
*******************************************************************/

  if (!pcode->compositional)
    {fprintf(stderr,"*ERR: You have asked me to write a table based on\n");
     fprintf(stderr," pcodeset number %d, but that one is NOT compositional.\n",
         ipc);
     *perr = 15; goto EXIT; 
    }
  if (pcode->ppf != NULL)
    {fprintf(stderr,"*ERR: You have asked me to write a table based on\n");
     fprintf(stderr," pcodeset number %d, but that one already has a\n",
         ipc);
     fprintf(stderr," ppf table.\n");
     *perr = 16; goto EXIT; 
    }

/* go */
  if (pcode->ncodes > MAX_PHONES)
    {fprintf(stderr,"*The number of symbols in the codeset that you\n");
     fprintf(stderr," wanted an ASCII table built from, %d,\n",pcode->ncodes);
     fprintf(stderr," is greater than the maximum currently allowed.\n");
     fprintf(stderr," To increase this maximum, change the constant\n");
     fprintf(stderr," MAX_PHONES and recompile.\n");
     *perr = 15; goto EXIT;
    }
  if (db_level > 0) printf("*DB: starting to calc wod distance table.\n");
  dmin =  9e4;
  dmax = -9e4;
  dtable->nsaved = 0;
  for (i = 0; i <= pcode->ncodes; i++)
    {for (j = 0; j <= pcode->ncodes; j++)
      {ppf->fval[i][j] = d = wod2(i,j,pcode,perr);
       if (d < dmin) dmin = d;
       if (d > dmax) dmax = d;
       tally(d,dtable);
    } }
  ppf->min_fval = dmin;
  ppf->max_fval = dmax;
  if (db_level > 0) printf("*DB: wod distance table built.\n");

  if (db_level > 0)
    {printf("*DB: dtable->nsaved = %d\n",dtable->nsaved);
     if (dtable->nsaved < 21)
       {printf("*DB:  freq   dval\n");
        for (idb=0; idb<dtable->nsaved; idb++)
          {for (jdb=idb+1; jdb<dtable->nsaved; jdb++)
             {if (dtable->entry[jdb].frequency > dtable->entry[idb].frequency)
               {db_entry_temp = dtable->entry[idb];
                dtable->entry[idb] = dtable->entry[jdb];
                dtable->entry[jdb] = db_entry_temp;
             } }
           printf("*DB: %2d.%6d %u\n",idb+1,
             dtable->entry[idb].frequency,dtable->entry[idb].value);
    }  }  }
 /* check for symmetricality */
  if (db_level > 0) printf("*DB: now checking for symmetry\n");
  ppf->symmetric = T;
  for (i = 0; ((ppf->symmetric)&&(i <= (pcode->ncodes - 1) )); i++)
    {for (j = i + 1; ((ppf->symmetric)&&(j <= pcode->ncodes)); j++)
       {if (ppf->fval[i][j] != ppf->fval[j][i]) ppf->symmetric = F;
        else j += 1;
    }  }
  if (db_level > 0) printf("*DB: symmetric = %d\n",ppf->symmetric);
  if (db_level > 0) printf("*DB: now picking most frequent dval\n");

 /* check for diagonal zeros */
  if (db_level > 0) printf("*DB: now checking for diagonal zeros\n");
  ppf->diagonal_zeros = T;
  for (i = 0; ((ppf->diagonal_zeros)&&(i <= pcode->ncodes )); i++)
    {if (ppf->fval[i][i] != 0) ppf->diagonal_zeros = F;
    }
  if (db_level > 0)
     printf("*DB: diagonal_zeros = %d\n",ppf->diagonal_zeros);
  if (db_level > 0) printf("*DB: now picking most frequent dval\n");

/*  find most frequent value - make it the default value */
/*   first, if diagonal_zeros is on, reduce the freq of the  */
/*   0-val item by the number of items on the diagonal       */
  if (ppf->diagonal_zeros)
    {for (i = 0; i < dtable->nsaved; i++)
       if (dtable->entry[i].value == 0)
          dtable->entry[i].frequency -= pcode->ncodes;
    }
  freq_max = 0;
  imax = -1;
  for (i = 0; i < dtable->nsaved; i++)
    {if (dtable->entry[i].frequency > freq_max)
       {freq_max = dtable->entry[i].frequency; imax = i;}
    }
  ppf->default_value = dtable->entry[imax].value;
/* open output ppf ASCII file */
  if ( (fp_out = fopen(fname,"w")) == NULL)
    {fprintf(stderr,"*ERR: can't open file '%s'\n",fname);
     *perr = 16; goto EXIT;
    };
/* print out data in form to be read back in */
  fprintf(fp_out,"; Pcode Symbol-to-symbol Distance Function Table.\n");
  fprintf(fp_out,"* Symbols defined in pcodeset: '%s'\n",pcode->name);
  fprintf(fp_out,"; Symbol-symbol distances generated by WOD2\n");
  fprintf(fp_out,"; using lower pcodeset: '%s'\n",pcode->lower_code_fname);
  if (ipc == pctab->ncodesets) 
    fprintf(fp_out,"; (none)\n");
  else
    {for (i=ipc+1; i <= pctab->ncodesets; i++)
       {fprintf(fp_out,";   %s\n",pctab->cd[i].pc->name);
    }  }
  fprintf(fp_out,"* Written in external format: 'NIST1'\n");
  fprintf(fp_out,"; Time/date of run: %s",ctime(&t_sec));
  if (ppf->symmetric)
     fprintf(fp_out,"*   function is symmetric\n");
  else
     fprintf(fp_out,"*   function is not symmetric\n");
  if (ppf->diagonal_zeros)
     fprintf(fp_out,"*   diagonal zeros\n");
  else
     fprintf(fp_out,"*   diagonal not zeros\n");
  fprintf(fp_out,"*   default value = %d\n",ppf->default_value);
  fprintf(fp_out,";    (occurs %d times)\n",
   dtable->entry[imax].frequency);
  fprintf(fp_out,";    (no. of different ppf values = %d)\n",
   dtable->nsaved);
  fprintf(fp_out,";\n");
  fprintf(fp_out,"; i sym   j sym  fval[i][j]\n");
/* write out table values */
  n_out = 0;
  for (i = 0; i <= (pcode->ncodes - 1); i++)
    {for (j = i + 1; j <= pcode->ncodes; j++)
       {if (ppf->fval[i][j] != ppf->default_value)
          {fprintf(fp_out,"%7s %7s %7d\n",pcode->pc[i].ac,
             pcode->pc[j].ac,ppf->fval[i][j]);
           n_out += 1;
          }
        if (!ppf->symmetric)
          {if (ppf->fval[j][i] != ppf->default_value)
             {fprintf(fp_out,"%7s  %7s %7d\n",pcode->pc[i].ac,
                pcode->pc[j].ac,ppf->fval[i][j]);
              n_out += 1;
    }  }  }  }
  fprintf(fp_out,"; Total no. of ppf values output = %d\n",n_out);
  if (fclose(fp_out) != 0)
    {fprintf(stdout,"*ERR: error return from fclose(%s)\n",fname);
     *perr = 17; goto EXIT;
    }
EXIT:
 db_leave_msg(proc,0); /* debug only */
 return;
 }
/* end wrppf2.c file */
