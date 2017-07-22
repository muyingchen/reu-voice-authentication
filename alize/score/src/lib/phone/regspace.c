/* file regspace.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  /************************************************************/
  /*                                                          */
  /*  char *regularize_spacing(s)                             */
  /*  Deletes consecutive whitespace.                         */
  /*  Changes all whitespace to '[space]'.                    */
  /*  Trims head and tail spaces.                             */
  /*                                                          */
  /************************************************************/
  char *regularize_spacing(char *s)
{char *p, *pm1, *q;
    pm1 = "#";
    q = s;
    for (p = s; *p != '\0'; p++)
      {if (!(isspace(*p) && isspace(*pm1))) *q++ = *p; 
       pm1 = p;
      } 
    *q = '\0';
    for (p = s; *p != '\0'; p++) if (isspace(*p)) *p = ' ';
    s = pltrim(prtrim2(s));
    return s;
   } /* end regularize_spacing() */
