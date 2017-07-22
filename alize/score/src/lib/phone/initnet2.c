/* file initnet2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


NETWORK *init_network2(char *name, char *s0, int *perr)                            
/***********************************************************/
/* Allocates memory for and initializes a network.         */
/* The initial version of the network will generate only   */
/* the sentence in *s0.                                    */
/***********************************************************/
 {char *proc = "init_network2";
  NETWORK *net; 
  NODE *last_node, *new_node;
  ARC *arcx;
  SUBSTRING trx, *the_rest = &trx;
  SUBSTRING tox, *token = &tox;
  char sxx[LINE_LENGTH], *sx = &sxx[0];
  boolean done;
 /* code */
  db_enter_msg(proc,0); /* debug only */
if (db_level > 0) printf("%ss0='%s'\n",pdb,s0);
  *perr = 0;
  net = (NETWORK *)malloc_safe(sizeof(NETWORK),proc);
  net->name = strdup_safe(name,proc);
  net->highest_nnode_name = 0;
  net->start_node = make_node("START",(ARC*)NULL,(ARC*)NULL,
     &(net->highest_nnode_name),perr);
  if (*perr > 0)
    {printf("%s:*ERR: make_node(START) returns %d\n",proc,*perr);
     goto RETURN;
    }
  net->start_node->start_state = T;
  net->start_node->stop_state  = F;
  last_node = net->start_node;
 /* add parsed string to network, treating all tokens as units */
  the_rest->start = pltrim(s0);
  the_rest->end   = prtrim(s0);
  done = F;
  while (!done)
    {/* get next token, make arc, new node */
     find_next_token3(the_rest,token);
     if (substr_length(token) < 1) done = T;
     else
       {sx = substr_to_str(token,sx,LINE_LENGTH);
if (db_level > 0) printf("%s adding token '%s'\n",pdb,sx);
        arcx = make_arc(sx,last_node,(NODE*)NULL,perr);
        last_node->out_arcs = 
           add_to_arc_list(last_node->out_arcs,arcx,perr);
        new_node = make_node((char*)NULL,arcx,(ARC*)NULL,
           &(net->highest_nnode_name),perr);
        if (*perr > 0)
          {printf("%s:*ERR: make_node() returns %d\n",proc,*perr);
           goto RETURN;
          }
        arcx->to_node = new_node;
        last_node = new_node;
        the_rest->start = token->end+1;
    }  }
  last_node->stop_state = T;
  net->stop_node = last_node;
  if (last_node != net->start_node)
    {net->highest_nnode_name -= 1;/* last node will be a/the STOP node */
     if (memory_trace) printf("%s FREEing %x\n",pdb,(int)new_node->name);
     free(new_node->name);
     new_node->name = strdup_safe("STOP",proc);
    }

 RETURN:
  db_leave_msg(proc,0); /* debug only */
  return net;
 } /* end init_network2() */
