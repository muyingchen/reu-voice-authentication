/* file grules1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/***********************************************************/
/* get_rules1 and its sub-functions                        */
/* Revised 4/28/94 by WMF to expand environment variables. */
/* Revised 11/4/94 by WMF to handle mem ovf errors better. */
/***********************************************************/

  /************************************************************/
  /*   r_process_aux_line(rset,pline,perr)                    */
  /*                                                          */
  /*   error codes: 3: bungled aux line                       */
  /*   (Fatal error if memory allocation fails.)              */
  /************************************************************/
  static void r_process_aux_line(RULESET1 *rset, char *pline, int *perr)
   {char *proc = "r_process_aux_line";
    char svx[LINE_LENGTH], *sval = &svx[0];
    char upx[LINE_LENGTH], *upr_pline = &upx[0];
    SUBSTRING sspx, *ssp = &sspx;
/* coding */
 db_enter_msg(proc,1); /* debug only */
    upr_pline = make_upper(strcpy(upr_pline,pline));
  /* locate value in quotes */
    *ssp = sstok2(pline,"\"'");
    if (substr_length(ssp) < 0)
      {fprintf(stderr,"%s: aux line contains no quoted field.\n",proc);
       fprintf(stderr," line:'%s'\n",pline);
       *perr=3;
      }
    else
      {sval = substr_to_str(ssp,sval,LINE_LENGTH);
/* identify field and store its value */
       if (strstr(upr_pline,"NAME"))
         {free_str(rset->name);
          rset->name       = strdup_safe(sval,proc);
	 }
       if (strstr(upr_pline,"DESC"))
         {free_str(rset->desc);
          rset->desc       = strdup_safe(sval,proc);
	 }
       if (strstr(upr_pline,"FORMAT"))
         {free_str(rset->format);
          rset->format       = strdup_safe(sval,proc);
	 }
       if (strstr(upr_pline,"MAX_NRULES")) rset->max_nrules = atol(sval);
       if (strstr(upr_pline,"COPY_NO_HIT")) rset->copy_no_hit = atobool(sval);
      }
 db_leave_msg(proc,1); /* debug only */
   } /* end r_process_aux_line */


  /************************************************************/
  /*   r_process_data_line(rset,pline,perr)                   */
  /*    error codes:   1: no valid information in pline       */
  /*                  31: memory ovf (too many rules)         */
  /*                  32: no arrow                            */
  /*   (Fatal error if memory allocation fails.)              */
  /************************************************************/
  static void r_process_data_line(RULESET1 *rset, char *pline, int *perr)
   {char *proc = "r_process_data_line";
    char *ieq;
    char sx_data[LINE_LENGTH], *sx = &sx_data[0];
    RULE1 x, *rx = &x;
    SUBSTRING ssx_data, *ssx = &ssx_data;
 db_enter_msg(proc,1); /* debug only */
    *perr = 0;
  /* check - blank line? */
    if (textlen(pline) < 1)
      {fprintf(stderr,"%s invalid input:blank line.\n",proc);
       *perr=1; goto RETURN;
      }
  /* check for which format  */
  /* NIST1: literal ascii  */
    if (streq(rset->format,"NIST1"))
      {ieq = strstr(pline,"=>");
       if (ieq == NULL)
         {fprintf(stderr,"*ERR:%s: no arrow, line='%s'\n",proc,pline);
          *perr = 32; goto RETURN;
	 }
     /* initialize rule */
       rx->sin = EMPTY_STRING;
       rx->sinl = 0;
       rx->sout = EMPTY_STRING;
       rx->soutl = 0;
     /* get left hand side */
       ssx->start = pltrimf(pline);
       ssx->end = ieq;
       if (ssx->start < ieq)
         {ssx->end -= 1;
          while ((ssx->end > ssx->start)&&(isspace(*(ssx->end)))) ssx->end -= 1;
      /* remove boundary characters, if any */
          if (((*ssx->start == '[')&&(*ssx->end == ']')) ||
              ((*ssx->start == '"')&&(*ssx->end == '"')) )
            {ssx->start += 1;
             ssx->end -= 1;
	    }
          rx->sinl = substr_length(ssx);
          rx->sin = strdup_safe(substr_to_str(ssx,sx,LINE_LENGTH),proc);
if (db_level > 2) printf("%s lhs='%s',len=%d\n",pdb,rx->sin,rx->sinl);
	 }
     /* get right hand side */
       ssx->end = prtrim(pline);
       ssx->start = ieq;
       while ((!isspace(*(ssx->start)))&&(*(ssx->start) != '\0')) ssx->start += 1;
       if (ssx->start < ssx->end)
         {while (isspace(*(ssx->start))) ssx->start += 1;
      /* remove boundary characters, if any */
          if (((*ssx->start == '[')&&(*ssx->end == ']')) ||
              ((*ssx->start == '"')&&(*ssx->end == '"')) )
            {ssx->start += 1;
             ssx->end -= 1;
	    }
          rx->soutl = substr_length(ssx);
          rx->sout = strdup_safe(substr_to_str(ssx,sx,LINE_LENGTH),proc);
      }  }
     if (rset->nrules < rset->max_nrules)
       {rset->nrules += 1;
        rset->rule[rset->nrules] = *rx;
       }
     else
       {fprintf(stderr,"*ERR:%s: too many rules.\n",proc);
        fprintf(stderr,"  (max_nrules = %d)\n",rset->max_nrules);
        *perr = 31; goto RETURN;
       }

RETURN:
 db_leave_msg(proc,1); /* debug only */
    return;
  } /* end r_process_data_line */


  /************************************************************/
  /*                                                          */
  /*  get_rules1(rset, path, fname, perr);                    */
  /*                                                          */
  /*  Function #1 to read in a ruleset from a file.           */
  /*  Input: *path+*fname contains the name of the file       */
  /*  Output: *rset is a ruleset structure                    */
  /*          *perr indicates success:                        */
  /*           0 iff a.o.k.                                   */
  /*           2 iff a pcode string had to be truncated.      */
  /*           3 an aux line was bungled.                     */
  /*           4 a data (code) line was bungled.              */
  /*          11 iff file *p1 can't be opened.                */
  /*          12 iff file *p1 is empty                        */
  /*          14 iff get_ppfcn2 returns an error indication.  */
  /*        (from function r_process_data_line:)              */
  /*           1: no valid information in pline               */
  /*          31: memory ovf (too many rules)                 */
  /*          32: no arrow                                    */
  /*   (Fatal error if memory allocation fails.)              */
  /* (Uses global parameter LINE_LENGTH.)                     */
  /*                                                          */
  /************************************************************/
  void get_rules1(RULESET1 *rset, char *path, char *fname, int *perr)
   {char *proc = "get_rules1";
/* data */
    FILE *fp;
    char line[LINE_LENGTH], *pline = &line[0];
    char fnxx[LONG_LINE], *full_fname = &fnxx[0];
    char cmx[LINE_LENGTH], *comment_flag = &cmx[0];
    int n_data_lines_processed;
/* coding  */
 db_enter_msg(proc,0); /* debug only */
 if (db_level > 0) printf("%sfname='%s'\n",pdb,fname);
    *perr = 0;
    n_data_lines_processed = 0;
/* make full name of pcodeset file */
    full_fname = make_full_fname(full_fname,path,fname);
/* expand environment variables in it */
    full_fname = expenv(full_fname,LONG_LINE);
/* initialize  */
    rset->name = strdup_safe(fname,proc);
    rset->directory = strdup_safe(path,proc);
    rset->desc = EMPTY_STRING;
    rset->format = strdup_safe("NIST1",proc);
    rset->copy_no_hit = F;
    rset->nrules = 0;
    rset->max_nrules = 200;
  /* open rules file */
if (db_level > 1) printf("*DB: trying to open '%s'\n",full_fname);
    if ( (fp = fopen(full_fname,"r")) == NULL)
      {fprintf(stderr,"%s: can't open %s\n",proc,full_fname);
       *perr = 11;  goto RETURN;
      }
  /* read first line to get the comment_flag characters */
    if (fgets(pline,LINE_LENGTH,fp) != NULL)
      {get_comment_flag(pline,comment_flag);
      }
    else
      {fprintf(stderr,"%s: file %s is empty.\n",proc,fname);
       *perr = 12;  goto RETURN;
      }
    if (streq(comment_flag,"*"))
      {fprintf(stderr,"%s: WARNING: the comment_flag character in",proc);
       fprintf(stderr," file %s\n",fname);
       fprintf(stderr,"  is '*', which also marks auxiliary lines.\n");
      }
  /* process file contents */
    while (fgets(pline,LINE_LENGTH,fp) != NULL) /* get next line */
     {pline = prtrim2(del_eol(pline));
if (db_level > 2) printf("%s line:'%s'\n",pdb,pline);
     if (valid_data_line(pline,comment_flag))
       {  /* process a non-blank, non-comment line in s */
       if (line[0] == '*')
         {r_process_aux_line(rset,pline,perr);
          if (*perr > 0) goto RETURN;
	 }
       else
         {if (n_data_lines_processed == 0) rset->rule = (RULE1*)
            calloc_safe((size_t)rset->max_nrules,sizeof(RULE1),proc);
          remove_comments(pline,comment_flag);
          r_process_data_line(rset,pline,perr);
          if (*perr > 0) goto RETURN;
          n_data_lines_processed += 1;
	 }
       if (*perr > 0)
          {fprintf(stderr,"%s: error processing '%s'\n",proc,fname);
           fprintf(stderr," line='%s'\n",pline);
           goto RETURN;
     } }  }
  /* close file & adjust some variables  */
    if (fclose(fp) != 0)
      fprintf(stderr,"%s: error return from fclose\n",proc);
RETURN:
 db_leave_msg(proc,0); /* debug only */
    return;
  } /* end get_rules1 */

/* end include grules1.c */
