/* file compact1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void compact(pal,ndel)                                           */
 /*                                                                    */
 /*   ALIGNMENT_INT *pal; int *ndel;                                   */
 /*                                                                    */
 /*   Compacts the alignment *pal by deleting all items in which null  */
 /* matches null; the number of items deleted is returned in *ndel.    */
 /*                                                                    */
 /**********************************************************************/

  void compact(ALIGNMENT_INT *pal, int *ndel)
{int nold,nnew,i;
   nold = pal->aint[0];
   nnew = 0;
   *ndel = 0;
   for (i=1; i<=nold; i++)
     {if ( (pal->aint[i]!=0) || (pal->bint[i]!=0) )
        {nnew += 1;
         pal->aint[nnew] = pal->aint[i];
         pal->bint[nnew] = pal->bint[i];
        }
      else *ndel += 1;
     }
  pal->aint[0]=nnew;
  pal->bint[0]=nnew;
  return;
  } /* end of function "compact" */
