/* file del_eol.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /***********************************************/
 /*  del_eol(s)                                 */
 /*  Deletes trailing end-of-line codes         */
 /* from the line *s.                           */
 /*  returns s.                                 */
 /***********************************************/
 char *del_eol(char *ps)
{char *p1;
  p1 = ps + strlen(ps) - 1;
  while ( (p1 >= ps) && (*p1 == '\n') ) {*p1 = '\0'; p1--;};
  return ps;
 }
