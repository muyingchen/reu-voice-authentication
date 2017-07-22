/* file pcindex5.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /*************************************************************************/
  /*  PCIND_T pcindex5(symb,pc1,add_to_core,add_to_file)                   */
  /*  Returns the index for the item in pcodeset "pc1" that matches *symb. */
  /*  Tries to make a new item if symb isn't found and pcodeset is "word". */
  /*    If add_to_core==T(rue), tries to add item to *pc1 (in core).       */
  /*    If add_to_file==T(rue), tries to add item to external ".ADD" file. */
  /*  If symb is not found and efforts to make a new item and add it to    */
  /*  the pcodeset in core or in the appropriate ".ADD" file fail, then    */
  /*  -1 is returned to signal an error.                                   */
  /*************************************************************************/
  PCIND_T pcindex5(char *symb, pcodeset *pc1, int add_to_core, int add_to_file)
{char *proc = "pcindex5";
    PCIND_T i;
    int err;
    boolean found;
    pcode_element x, *px = &x;
/* code: */
 db_enter_msg(proc,3);
if (db_level > 3) printf("*DB: symb='%s', pcodeset name = '%s'\n",
 symb, pc1->name);
/* error check */
    if (strlen(symb) < 1) {i = -1; goto RETURN;}
/* look hash-wise if appropriate: */
    if (pc1->hashed) i = (PCIND_T)pcindex_htab_lookup(symb,pc1,&found);
    else
      {/* do binary search if appropriate */
       if ( (pc1-> ncodes > 30) && (streq(pc1->order,"alphabetic")) )
         {i = (PCIND_T)pcindex_bsearch(symb,pc1);
          if (i < 0) found = F; else found = T;
         }
       else  /* else do straight linear search */
         {i=0; found=F;
          while ((i <= pc1->ncodes) && (! found))
            {if (pc1->separated)
	       {if (streq(symb,pc1->pc[i].ac)) found = T;
                else
                  {if (streq(pc1->type_case,"immaterial"))
                     {if (streqi(symb,pc1->pc[i].ac)) found = T;
	       }  }  }
             else /* not separated */
	       {if ((strlen(pc1->pc[i].ac) == 1)
                  &&(*symb == *(pc1->pc[i].ac))) found = T;
	       }
             if (!found) i += 1;
      }  }  }
    if (!found)
      {i = -1;
       if (streq(pc1->element_type,"word"))
         {if (add_to_core || add_to_file)
            {px = make_element(px,symb,pc1);
             if (px != NULL)
               {err = 0;
                if (add_to_core) i = (PCIND_T)add_pcode_element(pc1,px,&err);
                if ((i != -1)&&(add_to_file)) report_lex_add2(i,pc1,&err);
                if (err > 0) i = -1;
      }  }  }  }
RETURN:
db_leave_msg(proc,3);
    return i;
   }
/* end include pcindex5.c */
