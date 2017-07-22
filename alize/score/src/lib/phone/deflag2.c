/* file deflag2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 void deflag2_node_r(NODE *node)            
 /*******************************************************************/
 /*   Turns off flag2 of NODE *node and all nodes following it,     */
 /* recursively.                                                    */
 /*******************************************************************/
 {char *proc = "deflag2_node_r";
  ARC_LIST_ATOM *p;
db_enter_msg(proc,1); /* debug only */
  if (node != NULL)
    {node->flag2 = F;
if (db_level > 0) printf("%snode '%s' flag2 = '%s'\n",
pdb,node->name,bool_print(node->flag2));
     if (node->out_arcs != NULL)
       {for (p = node->out_arcs; p != NULL; p = p->next)
          deflag2_node_r(p->arc->to_node);
    }  }
db_leave_msg(proc,1); /* debug only */
  return;
  } /* end of function "deflag2_node_r" */
