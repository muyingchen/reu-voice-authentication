/* file stal1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/********************************************/
/*  fcn sort_tally1(table)                  */
/*  Sorts the tally list in *table by freq  */
/*  Uses a simple sort that is just fine if */
/*  file is already almost sorted.          */
/********************************************/

void sort_tally1(TALLY_STR *table)
{int i,j;
  for(i = 0; i < (int)(table->nsaved - 1); i++)
    {for (j = i + 1; j < table->nsaved; j++)
       {if (table->entry[j].frequency > table->entry[i].frequency)
          interchange_tallies(table,i,j);
 }  }  }
/* end function sort_tally1() */
/* end file stal1.c */
