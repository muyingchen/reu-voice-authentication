/* File pcext1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/******************************************************************/
/*  char *pcext1(iphone,pcstr)                                    */
/*  Returns a string containing the external representation of    */
/*  the phone in pcstr whose index value is iphone.               */
/*  If there is no match, returns NULL.                           */
/******************************************************************/
char *pcext1(int iphone, pcodeset *pcstr)
{char *proc = "pcext1";
 if ((iphone >= 0) && (iphone <= pcstr->ncodes))
   return pcstr->pc[iphone].ac;
 else
   {fprintf(stderr,"%s: invalid iphone=%d\n",proc,iphone);
    return NULL;
}  }
