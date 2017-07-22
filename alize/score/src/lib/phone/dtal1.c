/* file dtal1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/*************************************/
/*  fcn dump_tally1(table)           */
/*  dumps the tally info in *table   */
/*************************************/

void dump_tally1(TALLY_STR *table)
{int k;
  fprintf(stdout,"\n DUMP OF TALLY INFO\n");
  fprintf(stdout,  " phtot = %d\n",table->phtot);
  fprintf(stdout,  " nsaved = %d\n",table->nsaved);
  fprintf(stdout,  " nover = %d\n",table->nover);
  fprintf(stdout,"\n   i word    j word   freq\n");
  for (k = 0; k < table->nsaved; k++)
    {fprintf(stdout," %7d %7d %8u\n",table->entry[k].ival,
       table->entry[k].jval, table->entry[k].frequency);
    }
 }
/* end function dump_tally1() */
/* end file dtal1.c */
