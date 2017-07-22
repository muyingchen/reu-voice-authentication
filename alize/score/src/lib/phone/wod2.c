/* file wod2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/********************************************************/
/*                                                      */
/*                    wod2.c                            */
/*                                                      */
/* int wod2(i,j,pcij,perr) int i,j; pcode *pcij;        */
/*   int *perr;                                         */
/* Weight or distance function:                         */
/*                                                      */
/* Returns the weight or distance between the           */
/* integers i and j; these integers are internal        */
/* numeric codes in the pcode *pcij.  This pcode        */
/* may have an element-to-element distance table (ppf), */
/* and it may be either basic (holistic, atomic) or     */
/* compositional (syntactic).  If there is a ppf        */
/* table present, it is used.  Otherwise, if the code   */
/* is not compositional (basic), a simple calculation   */
/* based only on <i> and <j> is done; but if it is      */
/* compositional, an alignment structure is             */
/* created, in which the "a" and "b" strings            */
/* are the strings of codes in the next lower           */
/* pcode that are given for <i> and <j> in *pcij.       */
/* (A pointer to this lower pcode is found in *pcij.)   */
/* The pcode declaration in this alignment structure    */
/* is set at the next lower pcode, and a call is        */
/* made to one of the "aldist" functions to determine   */
/* the weight or distance between i and j as            */
/* an alignment distance between their representations  */
/* in the next lower pcode.                             */
/*                                                      */
/* This function and aldist2 form a recursive           */
/* set of functions.                                    */
/*                                                      */
/********************************************************/

 int wod2(int i, int j, pcodeset *pcij, int *perr)
{

/* local data: */
  char *proc = "wod2";
  ALIGNMENT_INT al_local, *pal = &al_local, *pdummy=NULL;
  int distance;
  boolean return_alignment;
/* code: */

/****** de-bug *****/
 db_enter_msg(proc,1); /* debug only */
 if (db_level > 1)
   {printf("%spcodeset name: '%s'\n",pdb,pcij->name);
    printf("%spcodeset element_type = '%s'\n",pdb,pcij->element_type);
    printf("%stwo input codes: i=%d='%s', j=%d='%s'\n",pdb,i,
       pcij->pc[i].ac,j,pcij->pc[j].ac);
   }
/****** de-bug *****/

  *perr = 0;
/* a little error-checking */
  if (i > pcij->highest_code_value)
    {fprintf(stderr,"*ERR: wod2 sent invalid i=%d\n",i);
     fprintf(stderr," (maximum is %d)\n",pcij->highest_code_value);
     *perr = 21; goto RETURN;
    }
  if (i < 0)
    {fprintf(stderr,"*ERR: wod2 sent invalid i=%d\n",i);
     *perr = 22; goto RETURN;
    }
  if (j > pcij->highest_code_value)
    {fprintf(stderr,"*ERR: wod2 sent invalid j=%d\n",j);
     fprintf(stderr," (maximum is %d)\n",pcij->highest_code_value);
     *perr = 23; goto RETURN;
    }
  if (j < 0)
    {fprintf(stderr,"*ERR: wod2 sent invalid j=%d\n",j);
     *perr = 24; goto RETURN;
    }
/* now go */
  if (i == j) {distance = 0; goto RETURN;}  /* flash check */
/* if there is a ppf, use it */
  if (pcij->ppf != NULL)
    {distance = pcij->ppf->fval[i][j];
     if (db_level > 1) printf("*DB: pcode ppf(%3d,%3d)=%d is used.\n",
        i,j,distance);
    }
  else
/* else do WOD1 (inline) if code is basic, ALDIST if code is compositional */
    {if (! pcij->compositional)
       {if (db_level > 1) printf("*DB: pcode is basic.\n");
        if (streq(pcij->element_type,"pfeat"))
          {if ((i==0)||(j==0)) distance = 1;    /* adds & dels     */
           else                distance = 2000; /* subs            */
          }
        else    /* default, if no more special cases */
          {if ((i==0)||(j==0)) distance = 3;   /* adds & dels      */
           else                distance = 4;   /* subs             */
          }
       }
     else
       {if (db_level > 1) printf("*DB: pcode is compositional.\n");
   /* build alignment for next lower code */
        pal->aint = pcij->pc[i].lc_int;
        pal->bint = pcij->pc[j].lc_int;
        pal->aligned = F;
        pal->pcset = pcij->lower_code;
        pal->naux = 0;
        return_alignment = F;
   /* and send it to aldist for distance computation */
        distance = aldist2(pal,wod2,return_alignment,pdummy,perr);
       } /*  "if (! pcij->compositional) X else ..."  */
    } /* "if (pcij->ppf != NULL) X else ..." */



 RETURN:
/****** de-bug ******/
 if (db_level > 2)
   printf("*DB:  about to leave wod2, distance=%d\n",distance);
 db_leave_msg(proc,1); /* debug only */
/****** de-bug ******/

  return distance;
 } /* end of function wod2 */
/* end of file wod2.c */
