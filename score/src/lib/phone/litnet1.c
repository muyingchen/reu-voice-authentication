/* file litnet1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


 void literalize_network(NETWORK *net, NODE *node, int *perr)
                                     

 /*******************************************************************/
 /*                                                                 */
 /*   Recursively expands symbols in network *net which are on or   */
 /* below the out_arcs of *node and which represent several         */
 /* literal words.                                                  */
 /* NOTE: for this to work right, the nodes in the net must first   */
 /* have had their flags turned off by "deflag1_node_r()".          */
 /*                                                                 */
 /*******************************************************************/
 {char *proc = "literalize_network";
  char sxx[LINE_LENGTH], *word = &sxx[0];
  char dbsxx[LINE_LENGTH], *dbsx = &dbsxx[0];
  SUBSTRING trx, *the_rest = &trx;
  SUBSTRING wrx, *rest_of_words = &wrx;
  SUBSTRING tox, *token = &tox;
  SUBSTRING ssox, *ss_word = &ssox;
  NODE *last_node, *new_node, *next_node;
  ARC *new_arc, *arcx;
  boolean done, words_done;
  ARC_LIST_ATOM *p, *px, *next_arc;
/* code: */
db_enter_msg(proc,0); /* debug only */

 /* initialize output variables */
  *perr=0;

if (db_level > 0) printf("%s node='%s'\n",pdb,node->name);
if (db_level > 2)
 {printf("%s out_arcs are:\n",pdb);
  printf(" from_node       symbol  to_node\n"); 
  for (px = node->out_arcs; px != NULL; px = px->next) dump_arc(px->arc);
 }

 /* have we visited this node already? */
  if (node->flag1) goto RETURN;
  else node->flag1 = T;
 /* do any out_arcs have non-literal symbols? */
  if (node->out_arcs != NULL)
    {if (db_level > 2) printf("%s searching over out_arcs\n",pdb);
     for (p = node->out_arcs; p != NULL; p = next_arc)
       {

if (db_level > 2)
{printf("%s looking at p->arc w/symbol '%s'\n",pdb,p->arc->symbol);
 printf("%s calling lit() on arc's next node '%s'\n",
 pdb,p->arc->to_node->name);
}
        literalize_network(net,p->arc->to_node,perr);
        if (*perr > 0)
          {fprintf(stderr,"%s: literalize_network err return=%d\n",pdb,*perr);
           fprintf(stderr," applied to node '%s'\n",
             p->arc->to_node->name);
           goto RETURN;
	  }
    /* save next arc in case we delete this one */
       next_arc = p->next;

if (db_level > 2)
 {printf("%s returned from lit() of next node to this node, '%s'\n",
  pdb,node->name);
}
        if (*(p->arc->symbol) == '{')
          {/* yes - expand it */

if (db_level > 2)
 {printf("%s expanding p->arc symbol '%s'\n",pdb,p->arc->symbol);
  printf("%sThe node->out_arcs arc list itself is:\n",pdb);
  dump_arc_list(node->out_arcs);
 }
           the_rest->start = p->arc->symbol+1;
           the_rest->end   = prtrim(p->arc->symbol)-1;
           done = F;
           while (!done)
             {*token = sstok(the_rest,"/"); /* ignore "or" */
              if (token->start > the_rest->end) done = T;
              else
                {

if (db_level > 2)
 {dbsx = substr_to_str(token,dbsx,LINE_LENGTH);
  printf("%s next token = '%s'\n",pdb,dbsx);
 }

                 last_node = p->arc->from_node;
              /* add parsed string to network, treating all tokens as units */
                 rest_of_words->start = token->start;
                 rest_of_words->end  = token->end;
                 words_done = F;
                 while (!words_done)
                   {/* get next word, make arc, new node */

if (db_level > 2)
 {dbsx = substr_to_str(rest_of_words,dbsx,LINE_LENGTH);
  printf("%s at top of word loop, rest_of_words = '%s'\n",pdb,dbsx);
 }
                    find_next_token3(rest_of_words,ss_word);
                    if (substr_length(ss_word) < 1) words_done = T;
                    else
                      {word = substr_to_str(ss_word,word,LINE_LENGTH);
if (db_level > 2) printf("%s adding word '%s'\n",pdb,word);
                       arcx = make_arc(word,last_node,(NODE*)NULL,perr);
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
                       rest_of_words->start = ss_word->end+1;
                   }  }
                 /* make an epsilon arc to original to-node */

if (db_level > 2) printf("%smaking new @ arc\n",pdb);

                new_arc = make_arc("@",last_node,p->arc->to_node,perr);
                 last_node->out_arcs = add_to_arc_list(last_node->out_arcs,new_arc,perr);
                 add_to_arc_list(new_arc->to_node->in_arcs,new_arc,perr);

if (db_level > 2)
{printf("%s at bottom of word loop, net=\n",pdb);
 dump_network(net);
 fflush(stdout);
}

                 the_rest->start = token->end+1;
             }  }
           /* then delete the old version */
if (db_level > 2) printf("%s killing old arc\n",pdb);
if (db_level > 2)
  {printf("%s before killing, node is:\n",pdb);
   dump_node(node);
  }
/* save pointer to arc to be deleted and next_node; fcns below may free(p) */
           arcx = p->arc;
           next_node = p->arc->to_node;
/* delete p->arc from in_arc list of next node */
           del_from_arc_list(&(next_node->in_arcs),arcx,perr);
/* delete p->arc from out_arc list of this node */
           del_from_arc_list(&(node->out_arcs),arcx,perr);
/* and free arc itself */
           kill_arc(arcx);

if (db_level > 2)
  {printf("%s after killing, node is:\n",pdb);
   dump_node(node);
  }

    }  }  }


 RETURN:

if (db_level > 2)
{printf("%s on exit, net=\n",pdb);
 dump_network(net);
}

db_leave_msg(proc,0); /* debug only */
  return;
  } /* end of function "literalize_network" */
