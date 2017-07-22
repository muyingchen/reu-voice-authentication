/* file dalist2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 void dump_arc_list2(ARC *arcs[MAX_ARCS], int n_arcs)
/*********************************************************/
/* Dumps array-style arc list                            */
/*********************************************************/
 {int i;
  printf("%s       DUMP OF ARC LIST (n=%d)\n",pdb,n_arcs);
  printf("     FROM        SYMBOL     TO    ADDRESS\n");
  for (i=1; i <= n_arcs; i++) dump_arc(arcs[i]);
  printf("\n");
  return;
 }

