/* file getline.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/*****************************************/
/*  getline: read a line, return length  */
/*****************************************/
 int getline(char *line, int lmax)
{ if (fgets(line, lmax, stdin) == NULL)
     return 0;
   else
     return strlen(line);
 }
