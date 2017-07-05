/* file addfwarn.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  /************************************************************/
  /*                                                          */
  /*  warn_if_add_file(fname,time)                            */
  /*                                                          */
  /*  Displays a warning if an "ADD" file corresponding to    */
  /* pcode file "fname" exits.  If *time is "start" or "end", */
  /* special additional messages appropriate to the start or  */
  /* end of a run are also displayed.                         */
  /*                                                          */
  /************************************************************/
  void warn_if_add_file(char *fname, char *time)
   {
/* data */
    char *proc = "warn_if_add_file";
    FILE *fp;
    char sxx[LINE_LENGTH], *afname = &sxx[0];
/* coding  */
 db_enter_msg(proc,0); /* debug only */
    afname = strcpy(afname,fname);
    afname = strcat(afname,".ADD");
if (db_level > 0) printf("%slooking for '%s'\n",pdb,afname);
    if ( (fp = fopen(afname,"r")) != NULL)
      {fclose(fp);
       fprintf(stdout,"\n");
       fprintf(stdout,"*WARNING: there exists an .ADD file for the pcode\n");
       fprintf(stdout," file '%s' at the %s of this run.\n",
         fname,time);
       if (streq(time,"start"))
	   {fprintf(stdout,"   Please consider whether its contents should\n");
            fprintf(stdout," have been checked and added to the pcode file\n");
            fprintf(stdout," before the run. If so, do it and then re-run.\n");
	   }
       if (streq(time,"end"))
	   {fprintf(stdout,"  Please consider whether its contents should\n");
            fprintf(stdout," be checked and added to the pcode file before\n");
            fprintf(stdout," the next run.\n");
	   }
       fprintf(stdout,"\n");
      }
 db_leave_msg(proc,0); /* debug only */
    return;
  } /* end warn_if_add_file */
