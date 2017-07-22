/* file dumpnstr.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

void dump_nstr(int *nstr)
{int i;
 printf("[%d",nstr[0]);
 for (i = 1; i <= nstr[0]; i++) printf(" %d",nstr[i]);
 printf("]\n");
 return;
}
