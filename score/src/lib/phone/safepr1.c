/* file safepr1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   char *safe_print(s)                                              */
 /*                                                                    */
 /* If s == NULL, returns NIL_STRING; else returns s.                  */
 /* (For safe printing a char string that might be NULL.)              */
 /*                                                                    */
 /**********************************************************************/

 char *safe_print(char *s)
  {if (s == NULL) return NIL_STRING; else return s;
  }
