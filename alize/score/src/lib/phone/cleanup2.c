
#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/* file cleanup2.c */

 /************************************************/
 /*  clean_up2(w,allowed)                        */
 /*  Cleans up word *w, returning w.             */
 /*  This version deletes leading and trailing   */
 /* punctuation, except for *allowed characters, */
 /* e.g. "-[]", which are regarded as part of    */
 /* the word.                                    */
 /************************************************/
  char *clean_up2(char *w, char *allowed)
{char *from, *to;
   /* skip leading punctuation except for allowed characters */
   for (from = w;
       ((*from != '\0') &&
        (ispunct((int)*from)) &&
        (strchr(allowed,*from) == NULL) );
       from++);
   /* copy rest of string */
   for (to = w; *from != '\0'; from++) *to++ = *from;
   *to = '\0';
   /* zap trailing punctuation except for allowed characters */
   for (to -= 1;
       ((to >= w) &&
        (ispunct((int)*to)) &&
        (strchr(allowed,*to) == NULL) );
       to--);
   *(to+1) = '\0';
   return w;
  } /* end function clean_up2(w,allowed) */
