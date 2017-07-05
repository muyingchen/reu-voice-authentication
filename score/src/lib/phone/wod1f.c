/* file wod1f.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/******************************************************/
/* function wod1f(i,j,pcij)                           */
/* Elementary weight or distance function definition: */
/* to be used with privative feature sets.  Makes     */
/* substitutions prohibitively expensive.             */ 
/******************************************************/

  int wod1f(int i, int j, pcodeset *pcij)
{int distance; char *proc = "wod1f";
db_enter_msg(proc,2); /* debug only */
  distance = 0;
  if ((i == 0) && (j == 0))
    fprintf(stderr,"*SYSTEM ERR: %s got i,j = 0\n",proc);
  else
    {if ((i != 0) && (j != 0))
          distance = 2000;
     else distance = 1;
    }
db_leave_msg(proc,2); /* debug only */
 return distance;
  } /* end wod1f.c */
