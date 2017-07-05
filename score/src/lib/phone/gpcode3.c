/* file gpcode3.c                   */
/* (uses sub-functions in gpcaux.c) */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  /************************************************************/
  /*                                                          */
  /*  get_pcode3(p1,path,fname,perr)                          */
  /*                                                          */
  /*  Function #3 to read in a pcodeset from a file.          */
  /*  Input: *path+*fname contains the name of the file       */
  /*  Output: *p1 is a pcodeset structure                     */
  /*          *perr indicates success:                        */
  /*           0 iff a.o.k.                                   */
  /*           2 iff a pcode string had to be truncated.      */
  /*           3 an aux line was bungled.                     */
  /*           4 a data (code) line was bungled.              */
  /*          11 iff file *p1 can't be opened.                */
  /*          12 iff file *p1 is empty                        */
  /*          14 iff get_ppfcn2 returns an error indication.  */
  /*   (Fatal error if memory allocation fails.)              */
  /* (Uses global parameters LONG_LINE.)                      */
  /*                                                          */
  /* Revised 4/28/94 by WMF to expand environment variables   */
  /*   that might be in path or fname.                        */
  /* Revised 6/30/94 by WMF to use the first token on the     */
  /*   first line in the file *fname as the comment flag, also*/
  /*   saving it in the p1 pcodeset structure.                */
  /* Revised 7/8/94 by WMF to process new auxiliary lines that*/
  /*   call out the names of files of rulesets that can be    */
  /*   used to transform these code symbols to or from a      */
  /*   "standard" representation.                             */
  /* Revised 7/21/94 by WMF to use name & file_name in *p1    */
  /* Revised 11/4/94 by WMF to handle mem ovf errors better.  */
  /************************************************************/
  void get_pcode3(pcodeset *p1, char *path, char *fname, int *perr)
{
/* data */
    FILE *fp;
    char line[LONG_LINE], *pline = &line[0];
    char fnxx[LONG_LINE], *full_fname = &fnxx[0];
    char fpx1[LONG_LINE], *real_path = &fpx1[0];
    char fpx2[LONG_LINE], *real_fname = &fpx2[0];
    char sxx[LONG_LINE], *sx = &sxx[0];
    char *proc = "get_pcode3";
    char *px1;
    int err2, *perr2 = &err2;
    int n_data_lines_processed;
/* coding  */
 db_enter_msg(proc,0); /* debug only */
 if (db_level > 0) printf("%sfname='%s'\n",pdb,fname);
    *perr = 0;
    n_data_lines_processed = 0;
/* re-analyze in case fname has part of a path in it */
    real_path  = strcpy(real_path,path);
    real_fname = strcpy(real_fname,fname);
    px1 = strrchr(real_fname,'/');
    if (px1 != NULL)
      {*px1 = '\0';
       real_path = strcat(real_path,real_fname);
       real_fname = strcpy(real_fname,px1+1);
      }

if (db_level > 1) printf("%s real_path ='%s'\n",pdb,real_path);
if (db_level > 1) printf("%s real_rname='%s'\n",pdb,real_fname);

/* make full name of pcodeset file */
    full_fname = make_full_fname(full_fname,real_path,real_fname);
/* expand environmental variables, if any */
    full_fname = expenv(full_fname,LONG_LINE);
/* issue warning if an *.ADD file exists */
    warn_if_add_file(full_fname,"start");
/* initialize  */
    p1->file_name = strdup_safe(real_fname,proc);
    p1->name      = strdup_safe(p1->file_name,proc);
    p1->directory = strdup_safe(real_path,proc);
    p1->desc      = strdup_safe("",proc);
    p1->format    = strdup_safe("NIST2",proc);
    p1->order     = strdup_safe("arbitrary",proc);
    p1->hashed = F;
    p1->hash_pars = NULL;
    p1->type_case = strdup_safe("mixed",proc);
    p1->comment_flag = strdup_safe("",proc);
    p1->separated = T;
    p1->sep_codes = strdup_safe(" ",proc);
    p1->compositional = F;
    p1->element_type = strdup_safe("phone",proc);
    p1->lower_code_fname = strdup_safe("",proc);
    p1->lower_code = NULL;
    p1->ppf_fname = strdup_safe("",proc);
    p1->ppf = NULL;
    p1->from_stn_rules_fname = strdup_safe("",proc);
    p1->from_stn_rules = NULL;
    p1->to_stn_rules_fname = strdup_safe("",proc);
    p1->to_stn_rules = NULL;
    p1->highest_code_value = 0;
    p1->ncodes = 0;
    p1->max_ncodes = 1000; /* allow for small lexicons */
  /* open pcode file */
    if ( (fp = fopen(full_fname,"r")) == NULL)
      {fprintf(stderr,"%s: can't open %s\n",proc,full_fname);
       *perr = 11;  goto RETURN;
      }
  /* read first line to get the comment_flag characters */
    if (fgets(pline,LONG_LINE,fp) != NULL)
      {get_comment_flag(pline,sx);
       free(p1->comment_flag);
       p1->comment_flag = strdup_safe(sx,proc);
      }
    else
      {fprintf(stderr,"%s: file %s is empty.\n",proc,fname);
       *perr = 12;  goto RETURN;
      }
    if (streq(p1->comment_flag,"*"))
      {fprintf(stderr,"%s: WARNING: the comment_flag character in",proc);
       fprintf(stderr," file %s\n",fname);
       fprintf(stderr,"  is '*', which also marks auxiliary lines.\n");
      }
  /* process file contents */
    while (fgets(pline,LONG_LINE,fp) != NULL) /* get next line */
     {pline = prtrim2(del_eol(pline));
     if (valid_data_line(pline,p1->comment_flag))
       {  /* process a non-blank, non-comment line in pline */
        if (line[0] == '*')
          {process_pcode_aux_line(p1,pline,perr);
           if (*perr > 0) goto RETURN;
          }
        else
          {if (n_data_lines_processed == 0)
             {if (p1->hashed)
                {p1->hash_pars =
                   (HASH_PARAMETERS*)malloc_safe(sizeof(HASH_PARAMETERS),proc);
                 hash_init(p1->hash_pars);
  if (db_level > 3) dump_hash_pars(p1->hash_pars);
	        }
              make_pc_table(p1);
              if (p1->hashed) init_pc_table(p1);
           /* now make sure (null) element 0 is defined */
              p1->pc[0].ac = strdup_safe("*",proc);   /* the null symbol     */
              p1->pc[0].lc_ext = EMPTY_STRING; /* its lower code rep  */
              p1->pc[0].lc_int = EMPTY_INT_STRING;
	     }
           remove_comments(pline,p1->comment_flag);
           process_pcode_data_line(p1,pline,perr);
           if (*perr > 0) goto RETURN;
           n_data_lines_processed += 1;
	  }
       if (*perr > 0)
          {fprintf(stderr,"%s: error processing '%s'\n",proc,fname);
           fprintf(stderr," line='%s'\n",pline);
           goto RETURN;
          }
       } /* if ((textlen(pline) > ) && ... */
     } /* while (fgets(pline,LONG_LINE,fp) != NULL) ...  */
  /* close file & adjust some variables  */
    if (fclose(fp) != 0)
      fprintf(stderr,"%s: error return from fclose\n",proc);
    if (!p1->hashed)
      {if ((streq(p1->order,"alphabetic"))||(streq(p1->order,"alpha")))
         sort_pcodeset_a(p1);
      }
  /* read in ppf values if there is a ppf file */
    if (!streq(p1->ppf_fname,EMPTY_STRING))
      {p1->ppf = (ppfcn*)malloc_safe(sizeof(ppfcn),proc);
   /* make full name of ppf file */
       full_fname = 
         make_full_fname(full_fname,p1->directory,p1->ppf_fname);
if (db_level > 0) printf("*DB: calling get_ppf on  '%s'\n",full_fname);
       get_ppfcn2(p1->ppf,full_fname,p1,perr2);
       if (*perr2 > 10)
         {fprintf(stderr,"%s: get_ppfcn2 returns fatal error code.\n",proc);
          *perr = 14;
	 }
      }
  /* read in from-standard ruleset file if there is one */
    if (!streq(p1->from_stn_rules_fname,EMPTY_STRING))
      {p1->from_stn_rules = (RULESET1*)malloc_safe(sizeof(RULESET1),proc);
       get_rules1(p1->from_stn_rules,p1->directory,p1->from_stn_rules_fname,perr);
      }
  /* read in to-standard ruleset file if there is one */
    if (!streq(p1->to_stn_rules_fname,EMPTY_STRING))
      {p1->to_stn_rules = (RULESET1*)malloc_safe(sizeof(RULESET1),proc);
       get_rules1(p1->to_stn_rules,p1->directory,p1->to_stn_rules_fname,perr);
      }
RETURN:
 db_leave_msg(proc,0); /* debug only */
    return;
  } /* end get_pcode3 */

/* end file gpcode3.c */
