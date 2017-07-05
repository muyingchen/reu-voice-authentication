/* file frnet2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /*******************************************************************/
 /*                                                                 */
 /*   void free_network(NETWORK *net)                               */
 /*                                                                 */
 /* Frees the memory space used by *net.                            */
 /* Method #2 - avoids the complication of recursively freeing, by  */
 /* using array-style lists of nodes and arcs to zap.               */
 /*                                                                 */
 /*******************************************************************/
 void free_network(NETWORK *net)
 {char *proc = "free_network";
  int n_nodes, n_arcs;
  NODE *nodes[MAX_NODES];
  ARC  *arcs[MAX_ARCS];
  int i, err, *perr = &err;
/* code: */
db_enter_msg(proc,1); /* debug only */
/* make list of all (unique) nodes & arcs */
  n_nodes = 0;
  n_arcs = 0;
  *perr = 0;
  make_zap_lists_r(net->start_node,nodes,&n_nodes,arcs,&n_arcs,perr);

if (db_level > 2)
 {printf("%safter making zap lists, node list is:\n",pdb);
  dump_node_list2(nodes,n_nodes);
  printf("\n");
  printf("%sand arc list is:\n",pdb);
  dump_arc_list2(arcs,n_arcs);
 } 

/* free them */
  for (i=1; i <= n_nodes; i++) kill_node2(nodes[i]);
  for (i=1; i <= n_arcs; i++)  kill_arc(arcs[i]);
  if (memory_trace) printf("%s FREE %x\n",pdb,(int)net->name);
  free ((void*)net->name);
  if (memory_trace) printf("%s FREE %x\n",pdb,(int)net);
  free ((void*)net);
db_leave_msg(proc,1); /* debug only */
  return;
  } /* end of function "free_network" */
