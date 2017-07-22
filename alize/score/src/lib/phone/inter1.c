/* file inter1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/******************************************************/
/*  fcn interchange_tallies(table,i,j)                */
/*  interchanges entries i and j in the tally table.  */
/******************************************************/

void interchange_tallies(TALLY_STR *table, int i, int j)
{FELEMENT temp_entry;
  temp_entry = table->entry[i];
  table->entry[i] = table->entry[j];
  table->entry[j] = temp_entry;
 }
/* end function interchange_tallies() */
/* end file inter1.c */
