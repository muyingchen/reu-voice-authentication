/* file wbtrim.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


 /***********************************************/
 /*  wbtrim(s)                                  */
 /*  Trims leading and trailing word boundaries */
 /* from string *s, returning s, which is not   */
 /* changed.                                    */
 /***********************************************/
  char *wbtrim(char *s)
{char *from, *to;
   /* skip leading blanks and word boundary symbols */
   from = s;
   while ((*from != '\0') && (isspace(*from)||(*from == '#'))) from += 1;
   /* copy rest of string */
   for (to = s; *from != '\0'; from++) *to++ = *from;
   /* trim tail of blanks and word boundary symbols */
   while (isspace(*(to-1))||(*(to-1) == '#')) to -= 1;
   *to = '\0';
   return s;
  }
