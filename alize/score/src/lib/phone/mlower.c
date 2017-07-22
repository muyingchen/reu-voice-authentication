/* file mlower.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /*********************************************/
 /*  make_lower(s)                            */
 /*  makes *s be all lower case               */
 /*  returns pointer to *s                    */
 /*********************************************/
  char *make_lower(char *s)
{char *pi;
   pi = s;
   while (*pi != '\0')
      {*pi = (char)tolower((int)*pi);
       pi += 1;
      }
   return s;
  }
