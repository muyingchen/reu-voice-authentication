/* file strcutl.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /***********************************************/
 /*  char *strcutl(ps,n)                        */
 /*  Cuts n characters from head of *ps.        */
 /***********************************************/
  char *strcutl(char *ps, int n)
{ps = strcpy(ps,ps+n);
   return ps;
  } /* end of strcutl() */

