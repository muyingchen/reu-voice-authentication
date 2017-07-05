/* file strmap1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  /************************************************************/
  /*                                                          */
  /*  char *strmap1(s,x,y)                                    */
  /*  Does a character replacement operation on string *s.    */
  /*  Each occurrence of the ith character in string *x is    */
  /* replaced by the ith character in string *y.              */
  /*  A pointer to s is returned.                             */
  /*                                                          */
  /************************************************************/
  char *strmap1(char *s, char *x, char *y)
   {char *ps, *px, *py;
    for (ps = s; *ps != '\0'; ps++)
      {py = y;
       for (px = x; *px != '\0'; px++)
         {if (*ps == *px) *ps =  *py;
          py++;
      }  }
    return s;
   } /* end strmap1 */
