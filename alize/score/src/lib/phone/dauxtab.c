/* file dauxtab.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/*************************************************/
/*  void dump_auxtab(pal)                        */
/*  dumps the auxiliary pcodes in pla->auxtab.   */
/*************************************************/
void dump_auxtab(ALIGNMENT_INT *pal)
{int ix,jx;
  printf("\n");
  printf(" DUMP OF ALIGNMENT PCODE AUXTAB:\n");
  printf(" naux = %d\n",pal->naux);
  if (pal->naux > 0)
    {printf("   i  nsymbs auxtab[i][*]-->\n");
     for (ix = 1; ix <= pal->naux; ix++)
       {
        printf(" %3d   %3d",ix,pal->auxtab[ix][0]);
        for (jx = 1; jx <= pal->auxtab[ix][0]; jx++)
           printf(" %5d",pal->auxtab[ix][jx]);
        printf("\n");
       }
    }
  printf("\n");
  return;
 }
/* end function dump_auxtab() */
