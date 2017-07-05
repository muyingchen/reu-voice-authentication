/* file fntoke2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void find_next_token2(x,y); SUBSTRING *x,*y;                     */
 /*                                                                    */
 /*   Finds the next token in the substring *x and returns it in *y.   */
 /*   A token is a string of characters bounded by whitespace or       */
 /* x->start or x->end, in this version.                               */
 /*   If there are nonesuch, returns y such that substr_length(y) < 1  */
 /**********************************************************************/

 void find_next_token2(SUBSTRING *x, SUBSTRING *y)
{char *proc = "find_next_token2";
  char *dbp;
 /* code */
  db_enter_msg(proc,1); /* debug only */

if (db_level > 1)
 {printf("*DB: input substring x=[");
  for (dbp=x->start; dbp <= x->end; dbp++){printf("%c",*dbp);}
  printf("]\n");
 }

/* move y->start through left-most spaces */
  y->start = x->start;
  while ((y->start<= x->end)&&(isspace((int)*y->start))) y->start += 1;
  if (y->start > x->end) y->end = y->start - 1;
  else
    {
/* move y->end thru following non-spaces */
     y->end = y->start;
     while ((y->end <= x->end)&&(!isspace((int)*(y->end)))) y->end += 1;
     y->end -= 1;
    }
  db_leave_msg(proc,1); /* debug only */
  } /* end of function "find_next_token2" */
