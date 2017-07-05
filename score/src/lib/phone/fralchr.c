 /* file fralchr.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void free_al_chr(p)                                              */
 /*                                                                    */
 /* Frees all memory space associated with an character-string         */
 /* alignment *p                                                       */
 /**********************************************************************/

 void free_al_chr(ALIGNMENT_CHR *p)
{
   if (memory_trace) printf("*DB: FREE %x\n",(int)p->id);
   free((char*)p->id);
   if (memory_trace) printf("*DB: FREE %x\n",(int)p->pastr);
   free((char*)p->pastr);
   if (memory_trace) printf("*DB: FREE %x\n",(int)p->pbstr);
   free((char*)p->pbstr);
   if (memory_trace) printf("*DB: FREE %x\n",(int)p);
   free((char*)p);
   return;
  }
