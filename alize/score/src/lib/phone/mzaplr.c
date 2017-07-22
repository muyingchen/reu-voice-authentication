/* file mzaplr.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /*******************************************************************/
 /*                                                                 */
 /*   void make_zap_lists_r(node1,nodes,n_nodes,arcs,n_arcs,perr)   */
 /*                                                                 */
 /* Makes array-style lists of sets of nodes and arcs found while   */
 /*recursively traversing the network at and below *node1.          */
 /*                                                                 */
 /*******************************************************************/
 void make_zap_lists_r(NODE *node1, NODE *nodes[MAX_NODES], int *n_nodes,
   ARC *arcs[MAX_ARCS], int *n_arcs, int *perr)
 {char *proc = "make_zap_lists_r";
  ARC_LIST_ATOM *p;
  boolean found;
  int i;
/* code: */
db_enter_msg(proc,0); /* debug only */

  if (*perr > 10) goto RETURN;

 /* add this node to node list */
  found = F;
  for (i=1; (i <= *n_nodes)&&(!found); i++)
    if (nodes[i] == node1) found = T;
  if (!found)
    {if (*n_nodes < MAX_NODES)
       {*n_nodes += 1;
        nodes[*n_nodes] = node1;
       }
     else
       {fprintf(stderr,"*ERR:%s: node list overflow.\n",proc);
        fprintf(stderr," Increase MAX_NODES and re-compile.\n");
        *perr = 11; goto RETURN;
    }  }
 /* add out-arcs to arc list */
  for (p = node1->out_arcs; p != NULL; p = p->next)
    {found = F;
     for (i=1; (i <= *n_arcs)&&(!found); i++)
       if (arcs[i] == p->arc) found = T;
     if (!found)
       {if (*n_arcs < MAX_ARCS)
          {*n_arcs += 1;
           arcs[*n_arcs] = p->arc;
          }
        else
          {fprintf(stderr,"*ERR:%s: arc list overflow.\n",proc);
           fprintf(stderr," Increase MAX_ARCS and re-compile.\n");
           *perr = 11; goto RETURN;
    }  }  }
 /* perform this on all lower nodes */
  for (p = node1->out_arcs; p != NULL; p = p->next)
    {make_zap_lists_r(p->arc->to_node,nodes,n_nodes,arcs,n_arcs,perr);
    }
RETURN:
db_leave_msg(proc,0); /* debug only */
  return;
  } /* end of function "make_zap_lists_r" */
