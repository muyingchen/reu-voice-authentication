/* file aldistn1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


 int aldistn1(NODE *node, ALIGNMENT_INT *aln_base,
    int (*wod) (int,int,pcodeset*,int*), int return_alignment,
    ALIGNMENT_INT *aln_best, int *pathn, int *perr)
             

 /***********************************************************************/
 /*                                                                     */
 /* Fcn to find alignment distance between a string and a network.      */
 /* ("aldistn" stands for "alignment distance - network")               */
 /* Procedure is to recursively generate all paths in the network,      */
 /* call aldist2() to calculate the string alignment distance between   */
 /* the string and each of them, and save the best.                     */
 /* Different network paths are generated in aln_base->aint; the        */
 /* constant literal symbol string is in aln_base->bint.                */
 /* As a path is traced from node to node, the integers representing    */
 /* the symbols on the arcs traversed are pushed onto and popped from   */
 /* the integer string *aln->base->aint[].  When a node is reached      */
 /* that has no out_arcs, the accumulated path of symbols is processed. */
 /* The path number *pathn is kept mainly for debugging purposes.       */
 /* The best distance found so far is returned; it is also kept in      */
 /* aln_best->d.                                                        */
 /* Iff return_alignment is T(rue), the particular alignment found      */
 /* is returned in *aln_best.                                           */
 /* This function should be called on the start_node of a network.      */
 /***********************************************************************/

 {char *proc = "aldistn1";
  ARC_LIST_ATOM *p;
  int d;
  ALIGNMENT_INT aln_out_data, *aln_out = &aln_out_data;
  WCHAR_T aint_reduced[MAX_SYMBS_IN_STR], *aint_save;
/* code: */
db_enter_msg(proc,0); /* debug only */

if (db_level > 2)
{printf("%snode->name='%s'\n",pdb,node->name);
 printf("%sreturn_alignment='%s'\n",pdb,bool_print(return_alignment));
 printf("%sthe pcode is '%s'\n",pdb,(aln_base->pcset)->name);
 printf("%sna =%d, nb =%d\n",pdb,aln_base->aint[0],aln_base->bint[0]);
 printf("%saln_base->aligned='%s'\n",pdb,bool_print(aln_base->aligned));
 printf("%sthe input strings to align are:\n\n",pdb);
 dump_int_alignment(aln_base);
 printf("%s*perr=%d\n",pdb,*perr);
}
  d = 0;
  if (*perr > 10) goto RETURN;
  if (node->stop_state) /* end of a path? */
    {aln_base->aligned = F;
if (db_level > 2) printf("%sEnd of a path\n",pdb);
     if (return_alignment)
       {aln_out = make_al_int(aln_base->pcset);
        aln_out->aligned = F;
       }
  /* delete nils from aint[] before sending it to aldist2() */

     nstrcpy(aint_reduced,aln_base->aint); /* make copy to manipulate */
     remove_zeros(aint_reduced);
     remove_symb(aint_reduced,"@",aln_base->pcset);

if (db_level > 3)
 {printf("%sjust before saving aint, aint=",pdb);
  dump_nstr(aln_base->aint);
 }
     aint_save = aln_base->aint;
     aln_base->aint = aint_reduced;
      
/*
if (db_level > 0) 
 {printf("%scalling aldist2() on alignment:\n",pdb);
  dump_int_alignment(aln_base);
 }
*/
      if ((aln_base->aint[0] == 0)&&(aln_base->bint[0] == 0))
        {d = 0;
         aln_out->aligned = T;
        }
      else
         d = aldist2(aln_base,wod,return_alignment,aln_out,perr);

if (db_level > 2) printf("%saldist2 returned d=%d\n",pdb,d);
     aln_base->aint = aint_save;
     if (*perr > 10)
       {fprintf(stderr,"%s:*ERR: aldist2 returns %d\n",proc,*perr);
        goto RETURN;
       }

if (db_level > 3)
 {printf("%sjust after restoring aint, aint=",pdb);
  dump_nstr(aln_base->aint);
 }

     if (d < aln_best->d)
       {aln_best->d = d;
        if (return_alignment) aln_best = copy_int_al(aln_best,aln_out);
       }
     if (return_alignment) free_al_int(aln_out);
    }
/* for each out arc, push symbol & chase next node */
  if (db_level > 2) printf("%ssweeping thru out_arcs...\n",pdb);
  for (p = node->out_arcs; p != NULL; p = p->next)
    {if (aln_base->aint[0] < MAX_SYMBS_IN_STR)
       {aln_base->aint[0] += 1;
        aln_base->aint[aln_base->aint[0]] = p->arc->isymb;
if (db_level > 2) printf("%slooking at arc with '%s'\n",pdb,p->arc->symbol); 
if (db_level > 2) printf("%spushing aint[%d]\n",pdb,aln_base->aint[0]);
       aln_base->aligned = F;
        d =  aldistn1(p->arc->to_node,aln_base,wod,
          return_alignment,aln_best,pathn,perr);
        aln_base->aint[0] -= 1;
if (db_level > 2) printf("%saint[0] popped back to %d\n",
pdb,aln_base->aint[0]);
       }
     else
       {fprintf(stderr,"%s:*ERR: integer string overflow.\n",proc);
        fprintf(stderr," Increase MAX_SYMBS_IN_STR and recompile.\n");
        *perr = 11;
        goto RETURN;
    }  }
RETURN:
db_leave_msg(proc,0); /* debug only */
  return aln_best->d;
  } /* end of function "aldistn1" */
