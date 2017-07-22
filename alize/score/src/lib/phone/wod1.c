/* file wod1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/********************************/
/* function wod1(i,j,pcij)      */
/* weight or distance function  */
/* depending only on i and j    */
/********************************/

  int wod1(int i, int j, pcodeset *pcij)
{int distance; char *proc = "wod1";
db_enter_msg(proc,2); /* debug only */
/* if there is a ppf, use it */
    if (pcij->ppf != NULL)
       distance = pcij->ppf->fval[i][j];
    else       /* if pcode is a feature set, use special wod1f fcn */
      {if (streq(pcij->element_type,"feat"))
         {if ((i==0)||(j==0)) distance = 1;    /* adds & dels     */
          else                distance = 2000; /* subs            */
         }
       else    /* default, if no more special cases */
         {if ((i==0)||(j==0)) distance = 1;   /* adds & dels      */
          else                distance = 1;   /* subs             */
      }  }
db_leave_msg(proc,2); /* debug only */
 return distance;
  } /* end of wod1 */
