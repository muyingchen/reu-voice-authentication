/* file snor1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /************************************************************/
  /*                                                          */
  /*  char *snor1(s)                                          */
  /*  Converts s to SNOR (close enough for jazz).             */
  /*                                                          */
  /************************************************************/
  char *snor1(char *s)
{char *p; char *xchar = "!:;,.?\"()-_[]{}~<>/";
    if (s != NULL)
      {s = make_upper(s);
       for (p = s; *p != '\0'; p++) if (!isgraph(*p)) *p = ' ';
       for (p = s; *p != '\0'; p++) if (strchr(xchar,*p)) *p = ' ';
       for (p = s; *p != '\0'; p++)
         {if (strncmp(p,"SAN ",4) == 0)   *(p+3) = '_';
          if (strncmp(p,"FORT W",6) == 0) *(p+4) = '_';
          if (strncmp(p,"LOS A",5) == 0)  *(p+3) = '_';
      }  }
    return s;
   } /* end snor1 */
