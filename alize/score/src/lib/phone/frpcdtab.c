 /* file frctable.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void free_code_table(p)                                          */
 /*                                                                    */
 /* Frees all memory space associated with a pcode table *p            */
 /**********************************************************************/

 void free_code_table(TABLE_OF_CODESETS *p)
  {int i;

   if (p->cd[1].pc != NULL) free_pcode(p->cd[1].pc);
/* NOTE: free_pcode() will recursively free contents of lower pcodesets. */
   for (i=1; i <= p->ncodesets; i++)
     {free_str(p->cd[i].pfname);
      free((void*)p->cd[i].pc);
     }
   free_str(p->directory);
   return;
  }
