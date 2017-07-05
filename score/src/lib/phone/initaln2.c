 /* File initaln2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void init_al_int(paln1,pcode1)                                   */
 /*                                                                    */
 /* Initializes the integer array alignment *paln1 with                */
 /*   pcodeset *pcode1.  Other items are set to legitimate             */
 /*   but informationless values.                                      */
 /*                                                                    */
 /**********************************************************************/

 void init_al_int(ALIGNMENT_INT *paln1, pcodeset *pcode1)
{
/* local data: */
  char *proc = "init_al_int";
/* code: */
 db_enter_msg(proc,3); /* debug only */
  *(paln1->aint) = 0; /* aint[0] holds na */
  *(paln1->bint) = 0; /* bint[0] holds nb */
  paln1->ia = 0;
  paln1->jb = 0;
  paln1->aligned = T;
  paln1->pcset = pcode1;
  paln1->naux = 0;
 db_leave_msg(proc,3); /* debug only */
  } /* end of function "init_al_int" */
/* end of file initaln2.c */
