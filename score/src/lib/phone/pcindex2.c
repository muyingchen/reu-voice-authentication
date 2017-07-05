/* file pcindex2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /*************************************************************************/
  /*  PCIND_T pcindex2(phone,pc1)                                          */
  /*  Returns the index for the item in pcodeset "pc1" that matches phone. */
  /*  If there is no match, tries to add item to pc1 and, if it a word     */
  /*  file, to an ".ADD" file; if that is not possible, returns -1.        */
  /*************************************************************************/
  PCIND_T pcindex2(char *phone, pcodeset *pc1)
{char *proc = "pcindex2";
    PCIND_T i;
    int err;
    boolean found;
    pcode_element x, *px = &x;
/* code: */
 db_enter_msg(proc,3);
if (db_level > 3) printf("*DB: phone='%s', pcodeset name = '%s'\n",
 phone, pc1->name);
/* error check */
    if (strlen(phone) < 1) {i = -1; goto RETURN;}
/* look hash-wise if appropriate: */
    if (pc1->hashed) i = (PCIND_T)pcindex_htab_lookup(phone,pc1,&found);
    else
      {/* do binary search if appropriate */
       if ( (pc1-> ncodes > 30) && (streq(pc1->order,"alphabetic")) )
         {i = (PCIND_T)pcindex_bsearch(phone,pc1);
          if (i < 0) found = F; else found = T;
         }
       else  /* else do straight linear search */
         {i=0; found=F;
          while ((i <= pc1->ncodes) && (! found))
            {if (pc1->separated)
	       {if (streq(phone,pc1->pc[i].ac)) found = T;
                else
                  {if (streq(pc1->type_case,"immaterial"))
                     {if (streqi(phone,pc1->pc[i].ac)) found = T;
	       }  }  }
             else /* not separated */
	       {if ((strlen(pc1->pc[i].ac) == 1)
                  &&(*phone == *(pc1->pc[i].ac))) found = T;
	       }
             if (!found) i += 1;
      }  }  }
    if (!found)
      {if (streq(pc1->element_type,"word"))
         {px = make_element(px,phone,pc1);
          if (px == NULL) i = -1;
          else
            {err = 0;
if (db_level > 0) printf("%s word not found, adding '%s'\n",pdb,phone);
             i = (PCIND_T)add_pcode_element(pc1,px,&err);
             if (err > 0) i = -1;
             else
               {report_lex_add2(i,pc1,&err);
                if (err > 0) i = -1;
         }  }  }
       else i = -1;
      }
RETURN:
db_leave_msg(proc,3);
    return i;
   }
/* end include pcindex2.c */
