 /* file malint.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   ALIGNMENT_INT *make_al_int(pcode1)                               */
 /*                                                                    */
 /* Allocates memory space for an object of type ALIGNMENT_INT (an     */
 /* integer string alignment) and initializes it with a pointer to     */
 /* pcode1.  Other items are set to legitimate but informationless     */
 /* values.  Returns a pointer to the alignment or NULL if memory      */
 /* allocation failed.                                                 */
 /*                                                                    */
 /**********************************************************************/

 ALIGNMENT_INT *make_al_int(pcodeset *pcode1)
{
/* local data: */
  char *proc = "make_al_int";
 ALIGNMENT_INT *p;
/* code: */
 db_enter_msg(proc,3); /* debug only */
  p = (ALIGNMENT_INT *) malloc_safe(sizeof(ALIGNMENT_INT),proc);
  p->aint = (WCHAR_T *) malloc_safe(sizeof(WCHAR_T)*(MAX_SYMBS_IN_STR + 1),proc);
  p->bint = (WCHAR_T *) malloc_safe(sizeof(WCHAR_T)*(MAX_SYMBS_IN_STR + 1),proc);
  init_al_int(p,pcode1);
 db_leave_msg(proc,3); /* debug only */
  return p;
  } /* end of function "make_al_int" */
