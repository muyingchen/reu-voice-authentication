/* file pcindex4.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /*************************************************************************/
  /*  PCIND_T pcindex_bsearch(astr,pc1)                                    */
  /*  Returns the index for the item in pcodeset "pc" whose ASCII code     */
  /*  string matches astr.  For speed, does no checking.                   */
  /*  Algorithm copies K&R 2nd ed. p. 134.                                 */
  /*  If there is no match, returns -1.                                    */
  /*  Special catch added to examine pc[0].                                */
  /*************************************************************************/
  PCIND_T pcindex_bsearch(char *astr, pcodeset *pc1)
{int compare_condition, low, high, mid;
    if (streq(astr,pc1->pc[0].ac)) return 0;
    low = 1; high = pc1->ncodes;
    while (low <= high)
      {mid = (low+high) / 2;
       if ((compare_condition = strcmp(astr,pc1->pc[mid].ac)) < 0)
         high = mid - 1;
       else
         if (compare_condition > 0)
           low = mid + 1;
         else
           return mid; 
      }
      return -1;
   } /* end function pcindex_bsearch */
/* end file pcindex4.c */
