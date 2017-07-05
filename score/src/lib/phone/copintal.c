 /* File copintal.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /*****************************************/
 /*                                       */
 /*   ALIGNMENT_INT *copy_int_al(p2,p1)   */
 /*   ALIGNMENT_INT *p2,*p1;              */
 /*                                       */
 /*   Copies *p1 into *p2, both of which  */
 /* are assumed to already exist.         */
 /* Returns *p2.                          */
 /*****************************************/

  ALIGNMENT_INT *copy_int_al(ALIGNMENT_INT *p2, ALIGNMENT_INT *p1)
{
/* local data: */
  char *proc = " copy_int_al";
  int i;
/* code: */
 db_enter_msg(proc,3); /* debug only */
  p2->aligned = p1->aligned;
  p2->aint = nstrcpy(p2->aint,p1->aint);
  p2->bint = nstrcpy(p2->bint,p1->bint);
  p2->pcset = p1->pcset;
  p2->ia = p1->ia;
  p2->jb = p2->jb;
  p2->naux = p1->naux;
  for (i=1; i <= p1->naux; i++)
    {if (p2->auxtab[i] == NULL)
       p2->auxtab[i] = nstrdup_safe(p1->auxtab[i],proc);
     else
       p2->auxtab[i] = nstrcpy(p2->auxtab[i],p1->auxtab[i]);
    }
 db_leave_msg(proc,3); /* debug only */
  return p2;
  } /* end of function "copy_int_al" */
 /* end file copintal.c */
