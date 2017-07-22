/* file encarcs1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 void encode_arcs_r(NODE *node, pcodeset *pcode, int *perr)
                                        
/*************************************************/
/*  Encodes symbols on out_arcs of a node and all*/
/* following nodes recursively.                  */
/*************************************************/
 {char *proc = "encode_arcs_r";
  ARC_LIST_ATOM *p;
db_enter_msg(proc,1); /* debug only */
  if (node != NULL)
    {if ((!node->flag1)&&(node->out_arcs != NULL))
       {node->flag1 = T;
        for (p = node->out_arcs; p != NULL; p = p->next)
          {p->arc->isymb = pcindex2(p->arc->symbol,pcode);
           if (p->arc->isymb < 0)
             {fprintf(stderr,"%s: encoding of an arc symbol failed.\n",proc);
              fprintf(stderr,
                 " symbol='%s', parent node='%s', pcode='%s'\n",
                 p->arc->symbol,node->name, pcode->name);
              p->arc->isymb = 0;
	     }
           else /* go further only if no serious error so far */
             {if (p->arc->to_node != NULL)
               encode_arcs_r(p->arc->to_node,pcode,perr);
    }  }  }  }
db_leave_msg(proc,1); /* debug only */
  return;
  } /* end of function "encode_arcs_r" */
