 /* file malchr.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   ALIGNMENT_CHR *make_al_chr()                                     */
 /*                                                                    */
 /* Allocates memory space for an object of type ALIGNMENT_CHR (a      */
 /* character string alignment) and initializes it with legitimate     */
 /* but informationless values.  Returns a pointer to the alignment    */
 /* or NULL if memory allocation failed.                               */
 /*                                                                    */
 /**********************************************************************/

 ALIGNMENT_CHR *make_al_chr(void)
{
/* local data: */
  char *proc = "make_al_chr";
 ALIGNMENT_CHR *p;
/* code: */
 db_enter_msg(proc,3); /* debug only */
  p = (ALIGNMENT_CHR *) malloc_safe(sizeof(ALIGNMENT_CHR),proc);
  p->id = (char *) malloc_safe(sizeof(char)*LINE_LENGTH+1,proc);
  p->pastr = (char *) malloc_safe(sizeof(char)*(MAX_STR_NCHARS + 1),proc);
  p->pbstr = (char *) malloc_safe(sizeof(char)*(MAX_STR_NCHARS + 1),proc);
  init_al_chr(p);
 db_leave_msg(proc,3); /* debug only */
  return p;
  } /* end of function "make_al_chr" */
/* end file malchr.c */
