 /* file frpcode.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


 /**********************************************************************/
 /*                                                                    */
 /*   void free_pcode(p)                                               */
 /*                                                                    */
 /* Frees all memory space associated with a pcode *p                  */
 /**********************************************************************/

 void free_pcode(pcodeset *p)
  {int i;
   free_str(p->name);
   free_str(p->file_name);
   free_str(p->directory);
   free_str(p->desc);
   free_str(p->format);
   free_str(p->order);

   if (p->hash_pars != NULL)
     {free_str(p->hash_pars->hfcn_name);
      if (memory_trace) printf("*DB: FREE %x\n",(int)p->hash_pars);
      free((char*)p->hash_pars);
     }

   free_str(p->type_case);
   free_str(p->comment_flag);
   free_str(p->sep_codes);
   free_str(p->element_type);
   free_str(p->lower_code_fname);
   if (p->lower_code != NULL) free_pcode(p->lower_code);

   free_str(p->ppf_fname);
   if (p->ppf != NULL)
     {free_str(p->ppf->external_format);
      if (memory_trace) printf("*DB: FREE %x\n",(int)p->ppf);
      free((char*)p->ppf);
     }

   free_str(p->from_stn_rules_fname);
   if (p->from_stn_rules != NULL)
     {free_rules1(p->from_stn_rules);
      if (memory_trace) printf("*DB: FREE %x\n",(int)p->from_stn_rules);
      free((char*)p->from_stn_rules);
     }

   free_str(p->to_stn_rules_fname);
   if (p->to_stn_rules != NULL)
     {free_rules1(p->to_stn_rules);
      if (memory_trace) printf("*DB: FREE %x\n",(int)p->to_stn_rules);
      free((char*)p->to_stn_rules);
     }

   if (p->pc != NULL)
     {for (i=0; ((i < p->pctab_length)&&(i <= p->highest_code_value)); i++)
        {if (strlen(p->pc[i].ac) > 0)
           {free_str(p->pc[i].ac);
            free_str(p->pc[i].lc_ext);
            free_str((char*)p->pc[i].lc_int);
        }  }
      if (memory_trace) printf("*DB: FREE %x\n",(int)p->pc);
      free((char*)p->pc);
     }

   return;
  }
