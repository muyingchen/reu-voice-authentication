/* file dctable.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /************************************************************/
  /*                                                          */
  /*  dump_code_table(ps1)                                    */
  /*  dumps ps1, a TABLE_OF_CODESETS                          */
  /************************************************************/
  void dump_code_table(TABLE_OF_CODESETS *ps1)
{int i; pcodeset *pcode, *p_lower_code;
  char *proc = "dump_code_table";
 db_enter_msg(proc,4); /* debug only */
    fprintf(stdout,"\nDUMP OF A TABLE_OF_CODESETS\n\n");
    fprintf(stdout,"ncodesets = %d\n",ps1->ncodesets);
    fprintf(stdout,"directory = '%s'\n",ps1->directory);
    if (ps1->ncodesets > 0)
      {fprintf(stdout,"  i    fname(i)    type(i)  nmax(i)  order(i)");
       fprintf(stdout,"  comp?  lower_code(i)\n");
       for (i = 1; i <= ps1->ncodesets; i++)
         {pcode = ps1->cd[i].pc;
          p_lower_code = (ps1->cd[i].pc)->lower_code;
      	  fprintf(stdout," %2d %16s %5s %8ld ",i,
      	     ps1->cd[i].pfname, pcode->element_type,pcode->max_ncodes);
          if (pcode->hashed) fprintf(stdout,"(hashed) ");
          else               fprintf(stdout,"%9s",pcode->order);
          fprintf(stdout,"    %s",bool_print((ps1->cd[i].pc)->compositional));
          if (ps1->cd[i].pc->compositional)
      	    fprintf(stdout," %15s\n",safe_print(p_lower_code->name));
          else 
            fprintf(stdout,"      (none)\n");
         }
      }
 db_leave_msg(proc,4); /* debug only */
    return;
  } /* end dump_code_table */

/* end of file dctable.c */
