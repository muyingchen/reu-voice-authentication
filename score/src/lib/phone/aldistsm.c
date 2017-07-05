 /* file aldistsm.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   int aldistsm(pt1,w,return_alignment,fom_threshold,pt2,perr)     */
 /*                                                                    */
 /*   ALIGNMENT_INT *pt1, *pt2; int (*w)(int*,int*,pcodeset*);         */
 /*   boolean return_alignment; int *perr;                             */
 /*   double fom_threshold;                                            */
 /*                                                                    */
 /*    Computes and returns the alignment distance between the         */
 /* symbols in the two integer strings in *pt1 (aint[] and bint[]).    */
 /* This version considers splits and merges, recursively, using the   */
 /* value of "fom_threshold".                                          */
 /* If a split or merge is found, an auxiliary symbol representing two */
 /* or more pcode symbols is put into the aux table of pt2.            */
 /* W(i,j) is a pointer to an integer cost (weight or distance)        */
 /* function of two integer variables and a pcodeset, which returns    */
 /* the distance from symbol i to j.                                   */
 /* w(i,j) is the cost of substituting int j for int i.                */
 /* w(0,j) is the cost of inserting int j;                             */
 /* w(i,0) is the cost of deleting int j;                              */
 /*    If <return_alignment> is T, this function returns as a          */
 /* by-product the particular alignment found, in *pt2.  The two       */
 /* strings in *pt2 will have been put into alignment by inserting     */
 /* nulls (zero) into one or both of them.                             */
 /*                                                                    */
 /*   *perr = 0 means a.o.k.                                           */
 /*   error codes:                                                     */
 /*   11:invalid input parameters                                      */
 /*                                                                    */
 /**********************************************************************/

  int aldistsm(ALIGNMENT_INT *aln_in, int (*w) (int,int,pcodeset*,int*),
      int return_alignment, double fom_threshold, ALIGNMENT_INT *aln_out,
      int *perr)
{/* data: */
    char *proc = "aldistsm";
    pcodeset *pcode1;
    short int d, dx;
    int smdel, sm_corr;
    boolean return_simple_alignment = T;
    ALIGNMENT_INT aln_local, *alnx = &aln_local;
    SM_TAB_STR2 smtabx, *smtab = &smtabx;
/* code: */
 /**** start debug ****/
db_enter_msg(proc,0); /* debug only */
if (db_level > 0 )
   {if (return_alignment) printf("*DB:   return_alignment = T\n");
    else                  printf("*DB:   return_alignment = F\n");
    printf("*DB:   the pcode is '%s'\n",(aln_in->pcset)->name);
    printf("*DB:   na =%d, nb =%d\n",aln_in->aint[0],aln_in->bint[0]);
    printf("*DB:   the input strings to align are:\n\n");
    dump_int_alignment(aln_in);
   }
 /**** debug ****/

 /* initialize */
  *perr=0;
  d=0;
  pcode1 = aln_in->pcset;
  if (return_alignment) alnx = aln_out;
  init_al_int(alnx,pcode1);
/* start with a non-sm alignment: */
  d = aldist2(aln_in,w,return_simple_alignment,alnx,perr);
  if (*perr > 10) goto RETURN;

if (db_level > 0)
 {printf("*DB: simple alignment (aldist2) d=%d\n",d);
  printf("*DB: simple alignment (static)  d=%d\n",aldist_static(alnx,perr));
  printf("*DB: phon alignment is:\n");
  dump_int_alignment(alnx);
 }

/* modify it to represent splits and merges: */
  smtab->nsaved = 0;
  alnx->naux = 0;
  sm_corr = 0;
  while ((smdel = sm_correction2(alnx,fom_threshold,smtab,perr)) > 0)
    {sm_corr += smdel;
     sort_sm_by_iloc(smtab);
     put_sm_into_aln(alnx,smtab,perr);
    }
  d -= sm_corr;

if (db_level > 0)
  {/*** check ***/
   dx = aldist_static(alnx,perr);
   if (dx != d)
     {printf("*ERR:%s:sm1 sm_corr not correct.\n",proc);
      printf(" d=%d, dx(from aldist_static)=%d\n",d,dx);
  }  }

RETURN:

 db_leave_msg(proc,0); /* debug only */

  return d;
  } /* end of function "aldistsm" */
 /* end of file "aldistsm.c" */
