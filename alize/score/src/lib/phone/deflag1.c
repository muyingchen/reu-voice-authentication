/* file deflag1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


 void deflag1_node_r(NODE *node)            
 /*******************************************************************/
 /*   Turns off flag1 of NODE *node and all nodes following it,     */
 /* recursively.                                                    */
 /*******************************************************************/
 {char *proc = "deflag1_node_r";
  ARC_LIST_ATOM *p;
db_enter_msg(proc,2); /* debug only */
  if (node != NULL)
    {node->flag1 = F;

if (db_level > 1) printf("%snode '%s' flag1 set = '%s'\n",
pdb,node->name,bool_print(node->flag1));

     if (node->out_arcs != NULL)
       {for (p = node->out_arcs; p != NULL; p = p->next)
          deflag1_node_r(p->arc->to_node);
    }  }
db_leave_msg(proc,2); /* debug only */
  return;
  } /* end of function "deflag1_node_r" */
