/*  File gctable.c  */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/**********************************************************/
/*  function get_code_table and its supporting functions  */
/*  (This function reads in a table of codesets.)         */
/**********************************************************/

  /************************************************************/
  /*                                                          */
  /*  gct_analyze_line(pline,ps1,perr)                        */
  /*                                                          */
  /*  Analyzes an input line for function Get_codes.          */
  /*  Input lines have 4 fields: file name (without type),    */
  /*  type, nmax, and order.                                  */
  /*  An aux line may specify the directory.                  */
  /*                                                          */
  /*          *perr indicates success:                        */
  /*           0 iff a.o.k.                                   */
  /*    (1-3 from get_pcode)                                  */
  /*           4 iff a field in the line was bungled.         */
  /*          11 iff full name of pcode file is too long.     */
  /*          12 iff there are too many codesets.             */
  /*          14 iff pcode has more elements than the nmax    */
  /*                 that was declared in the table.          */
  /*          15 iff fatal error from get_pcode.              */
  /*                                                          */
  /*   (Fatal error if memory allocation fails.)              */
  /* Note: in order to use STRTOK, we had to put in and use   */
  /* a separate pointer, "the_rest", because one of the       */
  /* functions called here also uses strtok, and strtok's     */
  /* side effects make this not work.                         */
  /************************************************************/
  void gct_analyze_line(char *pline, TABLE_OF_CODESETS *ps1, int *perr)
  {
  char *proc = "gct_analyze_line";
  char *pix;
  char iyx[LINE_LENGTH], *piy = &iyx[0];
  char local_line[LINE_LENGTH], *line = &local_line[0];
  int icode, err2, *perr2 = &err2;
/* coding  */
  db_enter_msg(proc,2); /* debug only */
  *perr = 0;
/* make local copy of pline to screw around with */
 line = strcpy(line,pline);
/* now go to work on line: */ 
  line = del_eol(line);
/* check for aux line with directory name */
  if (*line == '*')
/* aux line: process it */
     {if (strstr(line,"directory") != NULL)
        {pix = strtok(line,"\"'"); /* 1st token starts at cc0 */
         if ((pix = strtok((char*)NULL,"\"'")) != NULL)
           ps1->directory = strdup_safe(pix,proc);
         else
           {fprintf(stderr,"%s: bungled directory line\n",proc);
            *perr = 4;
            goto EXIT;
	}  }
      goto EXIT;
     } /* end of aux line processing (if (*line == '*') ...) */
/* data line processing */
/*  tentatively get new number of codesets */
  icode = ps1->ncodesets + 1;
  if (icode > MAX_CODE_SETS)
     {fprintf(stderr,"%s: too many codesets\n",proc);
      fprintf(stderr," Increase MAX_CODE_SETS parameter & recompile.\n");
      *perr=12;
      goto EXIT;}
/* initialize a place in the table of codesets */
  ps1->cd[icode].pfname = EMPTY_STRING;
  ps1->cd[icode].pc =     NULL;
/* get pfname */
  if ((pix = strtok(line," ")) != NULL)
     {piy = strcpy(piy,pix);
      if (strchr(piy,'.') == NULL) piy = strcat(piy,".txt");
      ps1->cd[icode].pfname = strdup_safe(piy,proc);
     }
  else
     {fprintf(stderr,"%s: strtok doesn't work for fname.\n",proc);
      *perr=4;
     }
/* get pcode from file pfname */
   /* get area for new pcode */
  ps1->cd[icode].pc = (pcodeset*)malloc_safe(sizeof(pcodeset),proc);
  get_pcode3(ps1->cd[icode].pc,
             ps1->directory,
             ps1->cd[icode].pfname,
             perr2);
  if (*perr2 > 0)
    {fprintf(stderr,"%s: get_pcode3 returns err=%d\n",proc,*perr2);
     fprintf(stderr,"   trying to read in file '%s'\n",
       ps1->cd[icode].pfname);
     fprintf(stderr,"   from directory '%s'\n",ps1->directory);
     *perr = 15;  goto EXIT;
    }
/* all ok - set new number of codesets: */
  ps1->ncodesets = icode;
EXIT:
  db_leave_msg(proc,2); /* debug only */
  return;
  } /* end gct_analyze_line */


  /************************************************************/
  /*                                                          */
  /*  get_code_table(ps1,fname,perr)                          */
  /*                                                          */
  /*  Reads in a structure consisting of several pcodes,      */
  /* from an ASCII text file, and encode the lower codes.     */
  /*  Input: *fname contains the name of the file             */
  /*  Output: *ps1 is a table of pcodes                       */
  /*          *perr indicates success:                        */
  /*           0 iff a.o.k.                                   */
  /*           1 iff a pcode has in its defining file a       */
  /*             different lower pcode than in this table.    */
  /*           2 iff a pcode string had to be truncated.      */
  /*           4 a data (code) line was bungled.              */
  /*          14 iff pcode has more elements than the nmax    */
  /*                 that was declared in the table.          */
  /*          15 iff fatal error from get_pcode3.             */
  /*          16 iff file *p1 can't be opened.                */
  /*          17 iff the lower code string can't be encoded.  */
  /* (Uses global parameter LINE_LENGTH.)                     */
  /*                                                          */
  /************************************************************/
  void get_code_table(TABLE_OF_CODESETS *ps1, char *fname, int *perr)
{
  /* data */
    char *proc = "get_code_table";
    int i, dbi, err2;
    FILE *fp;
    char line[LINE_LENGTH], *pline = &line[0];
    char fnx[LINE_LENGTH], *fnamex = &fnx[0];
    pcodeset *pcx;
/* coding  */
  db_enter_msg(proc,0); /* debug only */
/* initialize  */
    *perr = 0;                   /* error return                        */
    ps1->ncodesets = 0;
    ps1->directory = EMPTY_STRING;
/* open pcode structure file */
    fnamex = del_eol(strcpy(fnamex,fname));
/* expand environmental variables, if any */
    fnamex = expenv(fnamex,LINE_LENGTH);
    if ( (fp = fopen(fnamex,"r")) == NULL)
      {fprintf(stderr,"%s: can't open %s\n",proc,fnamex);
       *perr = 16;
       goto EXIT;
      }
/* read and process lines */
    while (fgets(pline,LINE_LENGTH,fp) != NULL) /* get next line */
      {pline = prtrim2(pline); /* trim off trailing whitespace */
       if ( (textlen(pline) > 0) && (line[0] != ';') )
         {err2 = 0;
          gct_analyze_line(pline,ps1,&err2);
          if (err2 > 0)
            {fprintf(stderr,"%s: bad line.\n",proc);
             fprintf(stderr,"%s: line='%s'\n",proc,pline);
             if (err2 > *perr) *perr = err2;
            }
          if (*perr > 10) goto EXIT;
      }  }
/* set pointers to next lower codes */
if (db_level > 0) printf("%ssetting pointers to lower codes...\n",pdb);
/* assumes that cd[i+1] contains lower code for cd[i] */
   for (i = 1; i < ps1->ncodesets; i++)
     {pcx = ps1->cd[i].pc;
      if (i < ps1->ncodesets)
        {if (!pcx->compositional)
           {fprintf(stderr,"\n");
            fprintf(stderr,"*WARNING: pcodeset '%s' is NOT compositional,\n",
               pcx->name);
            fprintf(stderr,"  but the table of pcodesets declares a\n");
            fprintf(stderr,"  lower pcodeset, '%s'.\n",ps1->cd[i+1].pc->name);
            *perr = 2;
	   }
         else
           {if (! streq(pcx->lower_code_fname, ps1->cd[i+1].pc->file_name) )
              {fprintf(stderr,"\n");
               fprintf(stderr,"*WARNING: the specification of a lower \n");
               fprintf(stderr,"pcodeset in pcodeset '%s', '%s', has been\n",
                   pcx->name,pcx->lower_code_fname);
               fprintf(stderr," overriden in favor of the table entry '%s'.\n",
                   ps1->cd[i+1].pc->file_name);
               *perr = 1;
	}  }  }
      if (*perr < 2) pcx->lower_code = ps1->cd[i+1].pc;
/*      pcx->compositional = T; */
     }
/* make last one have no lower pcodeset, not compositional */
   pcx = ps1->cd[ps1->ncodesets].pc;
   if ((pcx->lower_code != NULL)||(pcx->compositional))
     {fprintf(stderr,"\n");
      fprintf(stderr,"*WARNING: the specification of a lower pcodeset\n");
      fprintf(stderr," in pcodeset '%s', '%s', has been\n",
          pcx->name,pcx->lower_code_fname);
      fprintf(stderr," overriden.  The pcode table entry specifies that\n");
      fprintf(stderr," there is NO lower pcodeset.\n");
      *perr = 1;
     }
   pcx->lower_code = NULL;
   pcx->compositional = F;
if (db_level > 1)
 {for (dbi = 1; dbi <= ps1->ncodesets; dbi++)  dump_pcode3(ps1->cd[dbi].pc);
 }
/* encode lower code representations */
if (db_level > 0) printf("%snow starting to encode lcs...\n",pdb);
   if (ps1->ncodesets > 1)
     {for (i = 1; i < ps1->ncodesets; i++)
        {pcx = ps1->cd[i].pc;
         if ((pcx->compositional)&&(pcx->lower_code != NULL))
             encode_lcs(pcx,perr);
         if (*perr > 0)
           {fprintf(stderr," encode_lcs returns *perr=%d\n",*perr);
            *perr = 17;
            goto EXIT;
     }  }  }
/* and exit */
EXIT:
  db_leave_msg(proc,0); /* debug only */
   return;
  } /* end get_code_table */

/* end of file gctable.c */
