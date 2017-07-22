/* file addpcd.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  /********************************************************************/
  /*   int add_pcode_element(p1,px,perr)                              */
  /*   adds the pcode element *px to the pcode structure *p1          */
  /*   returns the integer internal code for px.                      */
  /*   error codes:   0: a.o.k.                                       */
  /*                  11: more than MAX_CODE_ELEMENTS code elements   */
  /*                  13: memory ovf                                  */
  /*   if element could not be added, returns -1.                     */
  /********************************************************************/
  int add_pcode_element(pcodeset *p1, pcode_element *px, int *perr)
{char *proc = "add_pcode_element";
    int i;
    boolean duplicate_key, table_full;
/* for debugging DOS Turbo C only:        */
/* unsigned coreleft(void); unsigned dbi; */
 db_enter_msg(proc,3); /* debug only */
    i = -1;
    if (p1->hashed)
      {i = pc_htab_store(p1,px,&duplicate_key,&table_full);
       if (duplicate_key & (detail_level > 0))
         fprintf(stderr,"*WARNING:%s: duplicate:'%s'\n",proc,px->ac);
       if (table_full) *perr = 11;
       else p1->ncodes += 1;
      }
    else
      {if (p1->ncodes < p1->pctab_length)
         {p1->ncodes += 1;
          i = p1->ncodes;
          p1->pc[i] = *px;
         }
       else *perr = 11;
      }
    if (i > p1->highest_code_value) p1->highest_code_value = i;
    if (*perr == 11)
      {fprintf(stderr,"%s: More than %d code elements\n",
         proc,p1->pctab_length);
       fprintf(stderr,
         "(Increase MAX_NCODES or RECOMMENDED_LOAD_FACTOR value.)\n");
      }
 db_leave_msg(proc,3); /* debug only */
   return i;
  } /* end add_pcode_element */
