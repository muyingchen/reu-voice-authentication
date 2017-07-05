/* file addadd1.c                   */
/* (uses sub-functions in gpcaux.c) */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  /************************************************************/
  /*                                                          */
  /*  add_add_file(p1,full_fname,perr)                        */
  /*                                                          */
  /*  Adds to pcodeset *p1 the entries, if any, in an "ADD"   */
  /*  file associated with its file *full_fname.              */
  /*  Output: *p1 is a pcodeset structure                     */
  /*          *perr indicates success:                        */
  /*           0 iff a.o.k.                                   */
  /* (Uses global parameters LINE_LENGTH, LONG_LINE.)         */
  /*                                                          */
  /************************************************************/

  void add_add_file(pcodeset *p1, char *full_fname, int *perr)
{char *proc = "get_pcode3";
/* data */
    FILE *fp;
    char afnxx[LONG_LINE], *afname = &afnxx[0];
    char line[LINE_LENGTH], *pline = &line[0];
    int n_data_lines_processed;
/* coding  */
 db_enter_msg(proc,0); /* debug only */
 if (db_level > 0) printf("%sfull_fname='%s'\n",pdb,full_fname);
    *perr = 0;
    afname = strcpy(afname,full_fname);
    afname = strcat(afname,".ADD");
if (db_level > 1) printf("%slooking for '%s'\n",pdb,afname);
    if ( (fp = fopen(afname,"r")) != NULL)
      {/* process file contents */
       fprintf(stdout,"\n");
       fprintf(stdout,"*NOTICE: Contents of an .ADD file are being\n");
       fprintf(stdout," used with the pcodeset '%s'.\n",full_fname);
       fprintf(stdout,"\n");
       n_data_lines_processed = 0;
       while (fgets(pline,LINE_LENGTH,fp) != NULL) /* get next line */
         {n_data_lines_processed += 1;
          pline = prtrim2(del_eol(pline));
          if (valid_data_line(pline,p1->comment_flag))
            {remove_comments(pline,p1->comment_flag);
             process_pcode_data_line(p1,pline,perr);
             if (*perr > 0)
               {fprintf(stderr,"%s: error processing '%s'\n",proc,afname);
                fprintf(stderr," line #%d\n",n_data_lines_processed);
                fprintf(stderr," line='%s'\n",pline);
                goto RETURN;
        }  }  }
      if (fclose(fp) != 0) fprintf(stderr,
        "%s: error return from fclose\n",proc);
     }
RETURN:
 db_leave_msg(proc,0); /* debug only */
    return;
  } /* end add_add_file */

/* end file addadd1.c */
