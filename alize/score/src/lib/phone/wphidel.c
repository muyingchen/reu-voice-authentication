/* file wphidel.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/****************************************************************/
/*  fcn write_phon_ins_del(table,pcode)                         */
/*  writes out insertions and deletions in the phone part       */
/*  of the tally table                                          */
/****************************************************************/

void write_phon_ins_del(TALLY_STR *table, pcodeset *pcode)
{int i,j,max_name_size,s_col_size;
  int nrocc[MAX_PHONES], nhocc[MAX_PHONES];
/* calc number of REF and HYP occurrences */
  for (i=0; i<=pcode->ncodes; i++)
    {nrocc[i] = 0; nhocc[i] = 0;
     for (j=0; j<=pcode->ncodes; j++)
       {nrocc[i] += table->phfreq[i][j];
        nhocc[i] += table->phfreq[j][i];
    }  }
/* calc max symbol name size */
  max_name_size = 0;
  for (i=1; i<=pcode->ncodes; i++)
    {if (strlen(pcode->pc[i].ac) > max_name_size)
         max_name_size = strlen(pcode->pc[i].ac);
    }
if (db_level > 0) printf("*DB: max_name_size = %d\n",max_name_size);
/* (error check) */
  if (max_name_size < 1) 
    {fprintf(stderr,"*ERR: longest symbol name length=%d\n",max_name_size);
     fprintf(stderr," in pcode set '%s'\n",pcode->name);
     fprintf(stderr," Function 'write_phon_ins_del' failed.\n");
    }
  else
/* print 'em out */
    {if (max_name_size < 6) s_col_size = 6;
     else                   s_col_size = max_name_size;
     fprintf(stdout,"\n");
     fprintf(stdout," %*.*s  # REF         # HYP\n",
        s_col_size,s_col_size,"                            ");
     fprintf(stdout," %*.*s  occs.  # Del  occs.  # Ins\n",
        s_col_size,s_col_size,"Symbol");
     for (i=1; i<=pcode->ncodes; i++)
       {fprintf(stdout," %*.*s%7d%7d%7d%7d\n",
          s_col_size,s_col_size,pcode->pc[i].ac,
          nrocc[i],table->phfreq[i][0],
          nhocc[i],table->phfreq[0][i]);
       }
     fprintf(stdout,"\n\n");
    }
 }
/* end function write_phon_ins_del() */
/* end file wphidel.c */
