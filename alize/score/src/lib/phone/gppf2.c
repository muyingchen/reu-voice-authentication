/* file gppf2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/****************************************************************/
/*                                                              */
/* File gppf2.c - software to get a phone-phone function       */
/* from a file into core.  Function get_ppfcn2 and supporting   */
/* functions: process_ppf_aux_line and process_ppf_data_line.   */
/*                                                              */
/* Ppf file format:                                             */
/* Consists of a header section followed by data section.       */
/* The lines in header section are auxiliary or comment lines.  */
/* Auxiliary lines are marked by "*" in cc1.                    */
/* Comment lines (ignored) are marked by ";" in cc1.            */
/* An auxiliary line may declare:                               */
/*   pcode "pcode name" -- will be checked for consistency.     */
/*   external format "XXXXX" -- currently may be either         */
/*     "NIST1" - data is in order iphone, jphone, fval; or      */
/*     "NIST2" - data is in order fval, iphone, jphone.         */
/*  default value = 999; value for phone-phone distances not    */
/*     specified in this file.  The default default value is 0. */
/*  (not) symmetric - if symmetric, make all                    */
/*     fval[i][j] == fval[j][i].                                */
/*  diagonal (not) zeros                                        */
/*     indicates whether or not fval[i][i]=0 for all i          */
/*     (overriding the default value declared, if any)          */
/* Data lines are free format.                                  */
/* A data line has three fields:  external (ASCII) codes for    */
/* a phone "i", a phone "j", and an integer phone-phone         */
/* function value for ppc[i][j].                                */
/*                                                              */
/****************************************************************/


  /************************************************************/
  /*   process_ppf_aux_line(ppf,pline,plno,perr)              */
  /*                                                          */
  /*   error codes:  1: warning error in aux line             */
  /*                13: fatal error in aux line               */
  /*                15: memory overflow.                      */
  /************************************************************/
  void process_ppf_aux_line(ppfcn *ppf, char *pline, int *plno, int *perr)
{
/* local data */
   char *proc = "process_ppf_aux_line";
   char *pix,*piy,*peq;
   char local_line[LINE_LENGTH], *pline_caps = &local_line[0];
/* coding */
   *perr = 0;
   pline_caps = make_upper(strcpy(pline_caps,pline)); /* normalize */
/* "external format" */
   if (strstr(pline_caps,"EXTERNAL FORMAT") != NULL)
     {pix = strtok(pline_caps,"\"'"); /* from start of line to "  */
      pix = strtok((char*)NULL,"\"'");  /* string in ""  or ''      */
      if ((pix == NULL) || (strlen(pix) < 1))
        {fprintf(stderr,
           "*WARNING:%s: bad EXTERNAL FORMAT line, line#%d\n",proc,*plno);
         fprintf(stderr," Using the default: '%s'\n",ppf->external_format);
         *perr=1; goto EXIT;
        }
      piy = strdup(pix);
      if (piy == NULL)
        {fprintf(stderr,"*ERR:%s: memory ovf\n",proc);
        *perr=15; goto EXIT;
        }
      if (ppf->external_format != NULL) free(ppf->external_format);
      ppf->external_format = piy;
      goto EXIT;
     }  /* if ((strstr(pline,"external format ... */
/* "default fval" */
    if (strstr(pline_caps,"DEFAULT VALUE")!= NULL)
      {if ((peq = (char*)strstr(pline,"=")) != NULL)
	 {ppf->default_value = atoi(peq+1);
          if (ppf->default_value > ppf->max_fval)
                ppf->max_fval = ppf->default_value;
          if (ppf->default_value < ppf->min_fval)
                ppf->min_fval = ppf->default_value;
	 }
       else
         {fprintf(stderr,
            "*WARNING:%s: bad default value line, line #%d\n",proc,*plno);
          *perr = 13;
	 }
       goto EXIT;
      }; /* if strstr(pline,"default fval ..." x;  */
/* "pcode" */
    if (strstr(pline_caps,"PCODE") != NULL)
      {pix = (char*)strtok(pline,"\"'"); /* get from start to " */
       if ((pix = (char*)strtok((char*)NULL,"\"'")) != NULL)  /* get "..." */
/* pix points to new pcode file name */
	 {if (!streq(ppf->pcode->name,pix))
/* give warning if pcode name is different */
            {fprintf(stderr,
              "*WARNING:%s: pcode name is different, line #%d\n",proc,*plno);
             fprintf(stderr," ppf file pcode name: '%s'\n",pix);
             fprintf(stderr," pcodeset name sent : '%s'\n",ppf->pcode->name);
             *perr = 1;
         }  }
       goto EXIT;
      }
/* "symmetric" */
    if (strstr(pline_caps,"SYMMETRIC") != NULL)
      {if (strstr(pline_caps,"NOT SYMMETRIC") != NULL)
	 ppf->symmetric = F;
       else
	 ppf->symmetric = T;
       goto EXIT;
      }
/* "diagonal_zeros" */
    if (strstr(pline_caps,"DIAGONAL") != NULL)
      {if (strstr(pline_caps,"NOT ZERO") != NULL)
	 ppf->diagonal_zeros = F;
       else
	 ppf->diagonal_zeros = T;
       goto EXIT;
      }
  EXIT:
    return;
  } /* end process_ppf_aux_line */


  /************************************************************/
  /*   process_ppf_data_line(ppf,pline,plno,perr)             */
  /*    error codes:  2: f(i,j) != f(j,i) for symmetric fcn.  */
  /*                 14: bungled data line                    */
  /************************************************************/
  void process_ppf_data_line(ppfcn *ppf, char *pline, int *plno, int *perr)
{
    int i,j,fval;
    char *proc = "process_ppf_data_line";
  /* coding */
    *perr = 0;
  /* check - blank line? */
    if (textlen(pline) <= 1) goto EXIT;
  /* adjust type case */
    adjust_type_case(pline,ppf->pcode);
  /* get next data triple */
    if (streq(ppf->external_format,"NIST1"))
      {i = pcindex2(strtok(pline," "),ppf->pcode);  /* phone i */
       j = pcindex2(strtok((char*)NULL," "),ppf->pcode);  /* phone j */
       fval =  atoi(strtok((char*)NULL," "));  /* phone-phone function value */
      }
    if (streq(ppf->external_format,"NIST2"))
      {fval =  atoi(strtok(pline," "));       /* phone-phone function value */
       i = pcindex2(strtok((char*)NULL," "),ppf->pcode);  /* phone i */
       j = pcindex2(strtok((char*)NULL," "),ppf->pcode);  /* phone j */
      }
/* check for errors */
    if ((i < 0) || (i > ppf->pcode->ncodes))
      {fprintf(stderr,"*ERR:%s: invalid phone i, line %d\n",proc,*plno);
       *perr = 14;
      }
    if ((j < 0) || (j > ppf->pcode->ncodes))
      {fprintf(stderr,"*ERR:%s: invalid phone j, line %d\n",proc,*plno);
       *perr = 14;
      }
    if (fval < 0)
      {fprintf(stderr,"*ERR:%s: invalid fval, line %d\n",proc,*plno);
       *perr = 14;
      }
    if (*perr > 10) goto EXIT;
 /* insert into table */
    ppf->fval[i][j] = fval;
    if (ppf->symmetric)
      {if ( (ppf->fval[j][i] != ppf->default_value) &&
            (ppf->fval[j][i] != ppf->fval[i][j]) )
         {fprintf(stderr,"*WARNING:%s:symmetricity violated.\n",proc);
          fprintf(stderr," Function was declared to be symmetric,\n");
          fprintf(stderr," but fval(i,j) NOT = fval(j,i) for\n");
          fprintf(stderr," i='%s', j='%s'\n",ppf->pcode->pc[i].ac,
            ppf->pcode->pc[j].ac);
          *perr = 2;
	 }
       ppf->fval[j][i] = fval;
      }
 /* check for maximum and minimum fval value */
    if (fval > ppf->max_fval) ppf->max_fval = fval;
    if (fval < ppf->min_fval) ppf->min_fval = fval;
EXIT:
    return;
  } /* end process_ppf_data_line */



  /******************************************************************/
  /*                                                                */
  /*  get_ppfcn2(ppf,fname,pcode,perr)                              */
  /*                                                                */
  /* gets the table of phone-phone distances from file fname        */
  /* and puts it into the area pointed to by ppf1;                  */
  /*                                                                */
  /*   Input: *fname contains the name of the file                  */
  /*          *pcode is the pcodeset                                */
  /*  Output: *ppf is a phone-phone distance table                  */
  /*          *perr indicates success:                              */
  /*           0 iff a.o.k.                                         */
  /*           1 warning: error in aux line.                        */
  /*           2 warning: declared symmetric, but f(i,j) != f(j,i). */
  /*           3 warning: declared not symmetric, but is symmetric. */
  /*          11 file can't be opened.                              */
  /*          12 empty file.                                        */
  /*          13 fatal error in an auxiliary line.                  */
  /*          14 invalid phone code or fval in a data line.         */
  /*          15 memory overflow.                                   */
  /*          16 sparse pcodeset pc table (won't work!)             */
  /*                                                                */
  /* (Uses global parameter LINE_LENGTH.)                           */
  /*                                                                */
  /******************************************************************/

  void get_ppfcn2(ppfcn *ppf, char *fname, pcodeset *pcode, int *perr)
{
/* data */
  char *proc = "get_ppfcn2";
  FILE *fopen(const char *, const char *),*fp;
  char line_saved[LINE_LENGTH], *pline_save = &line_saved[0];
  char line[LINE_LENGTH], *pline = &line[0];
  char fnxx[LONG_LINE], *full_fname = &fnxx[0];
  int lno, *plno = &lno, i, j, n_data_lines = 0;
  int worst_error, *perr_worst = &worst_error;
  boolean sym;
/* coding */
  db_enter_msg(proc,1); /* debug only */
 /* initialize data */
  *perr = 0;       /* error return                */
  *perr_worst = 0; /* worst error return          */
  *plno = 0;       /* line number in file         */
 /* error check */
  if (pcode->hashed)
    {*perr = 16; goto EXIT;
   }
 /* initialize phone-to-phone function structure */
  ppf->pcode           = pcode;
  ppf->external_format = strdup("NIST1");
  ppf->max_fval        = -1;
  ppf->min_fval        = 999999;
  ppf->symmetric       = T;
  ppf->diagonal_zeros  = T;
  ppf->default_value   = 0;
  for (i=0; i<=pcode->ncodes; i++)
    {for (j=i;j<=pcode->ncodes; j++)
       {ppf->fval[i][j] = 0; ppf->fval[j][i] = 0;
    }  }
/* expand environmental variables, if any */
    full_fname = expenv(strcpy(full_fname,fname),LONG_LINE);
 /* open pp table file */
  if ( (fp = fopen(full_fname,"r")) == NULL)
    {fprintf(stderr,"*ERR:%s: can't open %s\n",proc,full_fname);
     *perr = 11; goto EXIT;
    };
 /* process file contents */
  while (fgets(pline_save,LINE_LENGTH,fp) != NULL) /* get next line */
   {*plno += 1;
    pline_save = prtrim2(del_eol(pline_save));
    pline = strcpy(pline,pline_save); /* copy to cut up */
    if ( (textlen(pline) > 0) && (line[0] != ';') )
       {if (line[0] == '*')
           process_ppf_aux_line(ppf,pline,plno,perr);
        else
          {if (n_data_lines == 0)
             {/* put in default values */
                      /* diagonal */
              for (i=0; i<=ppf->pcode->ncodes; i++)
                {if (ppf->diagonal_zeros) ppf->fval[i][i] = 0;
                 else   ppf->fval[i][i] = ppf->default_value;
	       }
                      /* off-diagonal */
              for (i=0; i<=ppf->pcode->ncodes-1; i++)
                {for (j=i+1; j<=ppf->pcode->ncodes; j++)
                   {ppf->fval[i][j] = ppf->default_value;
                    ppf->fval[j][i] = ppf->default_value;
             }  }  }
           n_data_lines += 1;
           process_ppf_data_line(ppf,pline,plno,perr);
          }
        if (*perr > 10)
           {fprintf(stderr,"*ERR:%s: error processing '%s'\n",proc,fname);
            fprintf(stderr," line='%s'\n",pline_save);
            goto EXIT;
           }
        if (*perr > *perr_worst) *perr_worst = *perr;
   }   }
  if (*plno < 1) 
    {fprintf(stderr,"*ERR:%s: empty file '%s'\n",proc,fname);
     *perr = 12; goto EXIT;
    }
  sym = T;
  for (i=0; i <= ppf->pcode->ncodes-1; i++)
    {for (j=i+1; j <= ppf->pcode->ncodes; j++)
      {if (ppf->fval[i][j] != ppf->fval[j][i])
        {sym = F;
    } } }
  if ( (!ppf->symmetric) && (sym) )
     {fprintf(stderr,"*WARNING:%s:symmetricity violated.\n",proc);
      fprintf(stderr," Function was declared to be not symmetric,\n");
      fprintf(stderr," but IS symmetric.\n");
      *perr = 3;
      if (*perr > *perr_worst) *perr_worst = *perr;
     }
  ppf->symmetric = sym;
  *perr = *perr_worst;
 EXIT:
  db_leave_msg(proc,1); /* debug only */
  return;
 } /* end get_ppfcn2 */
/* end file gppf2.c */
