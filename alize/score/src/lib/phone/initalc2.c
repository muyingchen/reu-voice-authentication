 /* File initalc2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void init_al_chr(palc1)                                          */
 /*                                                                    */
 /* Initializes the character string alignment *palc1 to legitimate    */
 /* but informationless values.                                        */
 /*                                                                    */
 /**********************************************************************/

 void init_al_chr(ALIGNMENT_CHR *palc1)
{
/* local data: */
  char *proc = "init_al_chr";
/* code: */
 db_enter_msg(proc,3); /* debug only */
  palc1->id    = strcpy(palc1->id,EMPTY_STRING);
  palc1->pastr = strcpy(palc1->pastr,EMPTY_STRING);
  palc1->pbstr = strcpy(palc1->pbstr,EMPTY_STRING);
  palc1->pia = palc1->pastr; /* set at valid but meaningless value */
  palc1->pjb = palc1->pbstr; /* set at valid but meaningless value */
  palc1->aligned = T;
 db_leave_msg(proc,3); /* debug only */
  return;
  } /* end of function "init_al_chr" */
