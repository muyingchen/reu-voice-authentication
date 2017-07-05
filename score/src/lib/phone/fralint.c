 /* file fralint.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void free_al_int(p)                                              */
 /*                                                                    */
 /* Frees all memory space associated with an integer alignment *p     */
 /* NOTE: should NOT be used if p->aint has been set to point into a   */
 /* pcodeset!!!!                                                       */
 /**********************************************************************/

 void free_al_int(ALIGNMENT_INT *p)
{int i;
   if (memory_trace) printf("*DB: FREE %x\n",(int)p->aint);
   free((char*)p->aint);
   if (memory_trace) printf("*DB: FREE %x\n",(int)p->bint);
   free((char*)p->bint);
   if (p->naux > 0)
     {for (i=1; i <= p->naux; i++)
        {if (memory_trace) printf("*DB: FREE %x\n",(int)p->auxtab[i]);
         free((char*)p->auxtab[i]);
     }  }
   if (memory_trace) printf("*DB: FREE %x\n",(int)p);
   free((char*)p);
   return;
  }
