/* file dnlist2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 void dump_node_list2(NODE *nodes[MAX_NODES], int n_nodes)
/********************************************************/
/* Dumps array-style list of nodes.                     */
/********************************************************/
 {int i;
  printf("%s DUMP OF NODE LIST (n=%d\n",pdb,n_nodes);
  printf("%s   NAME  ADDRESS\n",pdb);
  for (i=1; i <= n_nodes; i++)
    printf("%s %6.6s  %x\n",pdb,nodes[i]->name,(int)nodes[i]);
  printf("\n");
  return;
 }
