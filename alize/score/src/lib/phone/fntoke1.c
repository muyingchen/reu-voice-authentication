/* file fntoke1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void find_next_token(x,y); SUBSTRING *x,*y;                      */
 /*                                                                    */
 /*   Finds the next token in the substring *x and returns it in *y.   */
 /*   A token is a string of characters bounded by                     */
 /*     1. ( and ) - at same level; (may contain anything)             */
 /*     2. " and "; (may contain anything except ")                    */
 /*     3. ' and '; (may contain anything except ')                    */
 /*     4. whitespace and whitespace; (may not contain whitespace)     */
 /*   x->start and x->end also may be token boundaries.                */
 /*   If there are nonesuch, returns y->end > x->end;                  */
 /**********************************************************************/

 void find_next_token(SUBSTRING *x, SUBSTRING *y)
{char *proc = "find_next_token";
  int level;
  boolean yend_not_located;
  char *dbp;
 /* code */
  db_enter_msg(proc,1); /* debug only */

if (db_level > 1)
 {printf("*DB: input substring x=[");
  for (dbp=x->start; dbp <= x->end; dbp++){printf("%c",*dbp);}
  printf("]\n");
 }

  y->end = x->end + 1; /* no-find condition */
/* move y->start through left-most spaces */
  for (y->start = x->start;
       ( (y->start <= x->end) && (isspace((int)*y->start)) );
       y->start++) {}
  if (y->start <= x->end)
    {if (*(y->start) == '(')
       {/* find matching ) */
        level = 0; yend_not_located = T;
        y->end = y->start + 1;
        while ( (y->end <= x->end) && yend_not_located)
          {if ( (*(y->end) == ')') && (level == 0) ) yend_not_located = F;
           else
             {if (*(y->end) == ')') level--;
              if (*(y->end) == '(') level++;
              y->end++;
       }  }  }
     else
       {if ((*(y->start) == '"') || (*(y->start) == '\''))
          {for (y->end = y->start + 1;
               (y->end <= x->end) && (*(y->end) != *(y->start));
                y->end++) {}
          }
        else
          {for (y->end = y->start;
               (y->end <= x->end) && (!isspace((int)*(y->end)));
                y->end++) {}
           y->end -= 1;
    }  }  }
  db_leave_msg(proc,1); /* debug only */
  } /* end of function "find_next_token" */
