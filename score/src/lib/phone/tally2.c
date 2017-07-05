/* file tally2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/********************************************/
/*  fcn tally2(i,j,table)                   */
/*  Tallies the pair (i,j) into the         */
/*  frequency histogram table *table.       */
/*  In this version, table entries are kept */
/*  in a blocked list, and if the entry     */
/*  added to has a frequency greater that   */
/*  the frequency of the preceding entry,   */
/*  they are interchanged.                  */
/********************************************/

void tally2(int i, int j, TALLY_STR *table)
{int k; boolean tallied; char *proc = "tally2";
 db_enter_msg(proc,2); /* debug only */
  tallied = F;
/* if we already have the pair, tally 1 into its pot */
  for (k = 0; ((!tallied)&&(k < table->nsaved)); k++)
    {if ((i == table->entry[k].ival)&&(j == table->entry[k].jval))
       {table->entry[k].frequency += 1;
        tallied = T;
        if (k > 1)
          {if (table->entry[k].frequency > table->entry[k-1].frequency)
             interchange_tallies(table,k,k-1);
    }  }  }
/* otherwise, put it in with a tally of 1 */
  if (!tallied)
    {if (table->nsaved < MAX_FHIST_SAVED_VALUES)
       {k = table->nsaved;
        table->nsaved += 1;
        table->entry[k].ival = i;
        table->entry[k].jval = j;
        table->entry[k].frequency = 1;
       }
     else
       table->nover += 1;
    }
 db_leave_msg(proc,2); /* debug only */
 }
/* end function tally2() */
/* end file tally2.c */
