 /* File simpnull.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   char *simplify_null_reps(s)                                      */
 /*   char *s;                                                         */
 /*                                                                    */
 /*   Simplifies the representations of null elements in s by          */
 /* replacing all strings of '*' by just one '*'.                      */
 /*                                                                    */
 /**********************************************************************/
 char *simplify_null_reps(char *s)
 {
/* local data: */
  char *proc = "simplify_null_reps";
  char *p1, *p2;
  boolean in_null_symbol;
/* code: */
 db_enter_msg(proc,3); /* debug only */
/* p1 = (char *)strchr(s,(int)'*'); */ /* BCD 4.2 */
  p1 = strchr(s,(int)'*'); /* ANSI (cf. K&R 2nd ed. p. 249) */
  if (p1 != NULL)
    {in_null_symbol = T;
     for (p2=p1+1; *p2 != '\0'; p2++)
       {if (in_null_symbol)
	  {if (*p2 != '*') in_null_symbol = F; else continue;}
        else
	  {if (*p2 == '*') in_null_symbol = T;}
	p1++;
	*p1 = *p2;
       }
     p1++;
     *p1 = '\0';
    }
 db_leave_msg(proc,3); /* debug only */
   return s;
  } /* end of function "simplify_null_reps" */

