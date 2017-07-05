 /* File cerrs1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif



 /**********************************************************************/
 /*                                                                    */
 /*   void calc_aln_errs1(al1,sub,ins,del)                             */
 /*                                                                    */
 /*   ALIGNMENT_INT *al1; int *sub,*ins,*del;                          */
 /*                                                                    */
 /* Calculates & returns the number of substitutions, insertions, and  */
 /* deletions indicated by the int alignment *al1.                     */
 /*                                                                    */
 /**********************************************************************/

  void calc_aln_errs1(ALIGNMENT_INT *al1, int *sub, int *ins, int *del)
{int i;
   *sub = 0;
   *ins = 0;
   *del = 0;
   for (i=1; i <=al1->aint[0]; i++)
     {if (al1->aint[i] == 0) {*ins += 1; continue;}
      if (al1->bint[i] == 0) {*del += 1; continue;}
      if (al1->aint[i] != al1->bint[i]) *sub += 1;
     }
   return;
  }
