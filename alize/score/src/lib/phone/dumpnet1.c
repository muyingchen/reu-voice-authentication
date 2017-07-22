/* file dumpnet1.c                                      */
/* package of functions to dump a network or its parts. */
/*                                                      */
/*  dump_arc(arc)                                       */
/*  dump_node(node)                                     */
/*  dump_node_r(node)                                   */
/*  dump_network(net)                                   */
/*  dump_node_arcs_r(node)                              */
/*  dump_network_arcs(net)                              */
/*  dump_arc_list(p)                                    */
/********************************************************/

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


 void dump_arc(ARC *arc)          
/*************************************************/
/*  Dumps an arc of a network.                   */
/*************************************************/
 {char *proc = "dump_arc";
db_enter_msg(proc,1); /* debug only */
  if (arc != NULL)
    {if (arc->from_node != NULL) printf("%9s",arc->from_node->name);
     else                        printf("**NULL**");
     printf("%13s",arc->symbol);
     if (arc->to_node != NULL)   printf("%9s",arc->to_node->name);
     else                        printf(" **NULL**");
     printf("  %6x\n",(int)arc);
    }
  else printf(" DUMP_ARC sent null arc.\n");
db_leave_msg(proc,1); /* debug only */
  return;
  } /* end of function "dump_arc" */

 void dump_node(NODE *node)            
/*************************************************/
/*  Dumps a node of a network.                   */
/*************************************************/
 {char *proc = "dump_node";
  ARC_LIST_ATOM *p;
db_enter_msg(proc,1); /* debug only */
  if (node != NULL)
    {printf("Node name: '%s'  address: %x\n",node->name,(int)node);
     printf(" start state = '%s', stop state = '%s', flag1 = '%s'\n",
       bool_print(node->start_state),bool_print(node->stop_state),
       bool_print(node->flag1));
     printf(" In_arcs are:");
     if (node->in_arcs == NULL) printf(" NONE\n");
     else
       {printf("\n");
        printf(" from_node       symbol  to_node   address\n");
        for (p = node->in_arcs; p != NULL; p = p->next) dump_arc(p->arc);
       }
     printf(" Out_arcs are:");
     if (node->out_arcs == NULL) printf(" NONE\n");
     else
       {printf("\n");
        printf(" from_node       symbol  to_node   address\n"); 
        for (p = node->out_arcs; p != NULL; p = p->next) dump_arc(p->arc);
    }  }
db_leave_msg(proc,1); /* debug only */
  return;
  } /* end of function "dump_node" */


 void dump_node_r(NODE *node)            
/*************************************************/
/*  Dumps a node of a network and nodes following*/
/* it, recursively.                              */
/*************************************************/
 {char *proc = "dump_node_r";
  ARC_LIST_ATOM *p;
db_enter_msg(proc,1); /* debug only */
  if ((node != NULL)&&(!node->flag2))
    {node->flag2 = T;
     dump_node(node);
     for (p = node->out_arcs; p != NULL; p = p->next)
       dump_node_r(p->arc->to_node);
    }
db_leave_msg(proc,1); /* debug only */
  return;
  } /* end of function "dump_node_r" */


 void dump_network(NETWORK *net)
/*************************************************/
/*  Dumps a network, nodes and arcs.             */
/*************************************************/
              
 {char *proc = "dump_network";
/* code: */
db_enter_msg(proc,1); /* debug only */

  printf("\n");
  if (net != NULL)
    {printf(" Network name: '%s'\n",net->name);
     printf(" highest_nnode_name = %d\n",net->highest_nnode_name);
     if (net->start_node == NULL) printf(" Start node is NULL.\n");
     else    printf(" Start node name is '%s'\n",net->start_node->name);
     if (net->stop_node  == NULL) printf(" Stop  node is NULL.\n"); 
     else    printf(" Stop node name is '%s'\n",net->stop_node->name);
     printf(" Nodes and Links in network:\n");
     deflag2_node_r(net->start_node);
     dump_node_r(net->start_node);
     printf("\n");
    }

db_leave_msg(proc,1); /* debug only */
  return;
  } /* end of function "dump_network" */


 void dump_node_arcs_r(NODE *node)            
/*************************************************/
/*  Dumps arcs of a node and all following nodes */
/* recursively.                                  */
/*************************************************/
 {char *proc = "dump_node_arcs_r";
  ARC_LIST_ATOM *p;
db_enter_msg(proc,1); /* debug only */
  if (node != NULL)
    {if ((!node->flag2)&&(node->out_arcs != NULL))
       {node->flag2 = T;
        for (p = node->out_arcs; p != NULL; p = p->next)
          {printf("%8s %12s %8s\n",
             p->arc->from_node->name,
             p->arc->symbol,
             p->arc->to_node->name);
           if (p->arc->to_node != NULL)
             dump_node_arcs_r(p->arc->to_node);
    }  }  }
db_leave_msg(proc,1); /* debug only */
  return;
  } /* end of function "dump_node_arcs_r" */


 void dump_network_arcs(NETWORK *net)              
/*************************************************/
/*  Dumps all arcs of a network.                 */
/*************************************************/
 {char *proc = "dump_network_arcs";
/* code: */
db_enter_msg(proc,1); /* debug only */

  printf("\n");
  if (net != NULL)
    {printf(" Network name: '%s'\n",net->name);
     printf(" highest_nnode_name = %d\n",net->highest_nnode_name);
     if (net->start_node == NULL) printf(" Start node is NULL.\n");
     else    printf(" Start node name is '%s'\n",net->start_node->name);
     if (net->stop_node  == NULL) printf(" Stop  node is NULL.\n"); 
     else    printf(" Stop node name is '%s'\n",net->stop_node->name);
     printf(" Arcs in network:\n");
     printf("  from_node      symbol  to_node\n");
     deflag2_node_r(net->start_node);
     dump_node_arcs_r(net->start_node);
     printf("\n");
    }

db_leave_msg(proc,1); /* debug only */
  return;
  } /* end of function "dump_network_arcs" */


void dump_arc_list(ARC_LIST_ATOM *p)                  
/*************************************************/
/*  Dumps addresses of the arc list itself       */
/* rather than its contents.                     */
/*************************************************/
{ARC_LIST_ATOM *px;
 printf("%sARC LIST ATOM ADDRESSES:\n",pdb);
 printf("%s  prev  current   next  (p->arc)\n",pdb);
 for (px=p; px != NULL; px = px->next)
   printf("%s %6x  %6x  %6x  %6x\n",pdb,
     (int)px->prev,(int)px,(int)px->next,(int)px->arc);
 printf("\n");
 return;
}
/* end file "dumpnet1.c" */
