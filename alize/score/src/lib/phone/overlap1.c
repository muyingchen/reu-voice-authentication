/* file overlap1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


/****************************************************/
/*  int overlap1(interval1,interval2)               */
/* Returns the overlap of the two intervals.        */
/* NOTE: these intervals are given inclusively by   */
/* integers, so that if the ending point of one     */
/* interval is the same as the starting point of    */
/* the other interval, there is an overlap of 1,    */
/* not 0.                                           */
/****************************************************/

int overlap1(struct INTERVAL1 a, struct INTERVAL1 b)
{char *proc = "overlap1";
  int amt;
db_enter_msg(proc,0); /* debug only */
  if (a.l1 <= b.l1) /* cases 1,2,3 */
    {if (b.l1 > a.l2)  amt = 0;                 /* case 1 */
     else             /* cases 2,3 */
       {if (b.l2 > a.l2) amt = a.l2 - b.l1 + 1; /* case 2 */
        else             amt = b.l2 - b.l1 + 1; /* case 3 */
    }  }
  else              /* cases 4,5,6 */
    {if (b.l2 <= a.l2) amt = b.l2 - a.l1 + 1;   /* case 4 */
     else             /* cases 5,6 */
       {if (b.l2 < a.l1) amt = 0;               /* case 5 */
        else            amt = a.l2 - a.l1 + 1;  /* case 6 */
   }  }
db_leave_msg(proc,0); /* debug only */
  return amt;
 }
