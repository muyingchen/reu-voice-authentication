/* file frules1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  void free_rules1(RULESET1 *rset)
  /************************************************************/
  /*                                                          */
  /*  Frees allocated space in RULESET1 *rset.                */
  /*                                                          */
  /************************************************************/

   {char *proc = "free_rules1";
/* data */
    int i;
/* coding  */
 db_enter_msg(proc,0); /* debug only */
    free_str(rset->name);
    free_str(rset->directory);
    free_str(rset->desc);
    free_str(rset->format);
    for (i = 1; i <= rset->nrules; i++)
      {free_str(rset->rule[i].sin);
       free_str(rset->rule[i].sout);
      }
 db_leave_msg(proc,0); /* debug only */
    return;
  } /* end free_rules1 */
