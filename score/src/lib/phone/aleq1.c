 /* File aleq1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /*******************************/
 /*                             */
 /*   boolean aleq1(p1,p2)      */
 /*   ALIGNMENT_INT *p1, *p2;   */
 /*                             */
 /*   Returns T iff p1 == p2.   */
 /*                             */
 /*******************************/

  boolean aleq1(ALIGNMENT_INT *p1, ALIGNMENT_INT *p2)
{
/* local data: */
  char *proc = " aleq1";
  boolean eq; int i,j;
/* code: */
 db_enter_msg(proc,3); /* debug only */
  if ((p1->aligned != p2->aligned) ||
      (p1->ia != p2->ia) ||
      (p1->jb != p2->jb))
    {eq = F;
     goto RETURN;
    }
  for (i=0; i<=p1->aint[0]; i++)
    {if (p1->aint[i] != p2->aint[i])
       {eq = F;
        goto RETURN;
    }  }
  for (i=0; i<=p1->bint[0]; i++)
    {if (p1->bint[i] != p2->bint[i])
       {eq = F;
        goto RETURN;
     }  }
  if (p1->naux != p2->naux)
    {eq = F;
     goto RETURN;
    }
  for (i=1; i<=p1->naux; i++)
    {if (p1->auxtab[i] != p2->auxtab[i])
       {eq=F;
        goto RETURN;
       }
     for (j = 1; j <= *(p1->auxtab[0]); j++)
       {if (p1->auxtab[i][j] != p2->auxtab[i][j])
          {eq=F;
           goto RETURN;
    }  }  }
/* passed all tests */
  eq = T;
RETURN:
 db_leave_msg(proc,3); /* debug only */
  return eq;
  } /* end of function "aleq1" */
 /* end file aleq1.c */
