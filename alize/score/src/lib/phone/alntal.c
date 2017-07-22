 /* File alntal.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void align_and_tally(pn1,wb,table,perr)                          */
 /*   ALIGNMENT_INT *pn1; int wb; TALLY_STR *table; int *perr;         */
 /*                                                                    */
 /*   Aligns the phone strings implied by the word strings in *pn1     */
 /* and tallies the phone occurrences into *table.  Wb is the integer  */
 /* phone code for "word boundary".                                    */
 /*                                                                    */
 /*   *perr = 0 means a.o.k.                                           */
 /*           11  "   *pia1 is aligned, but len(aint) != len(bing)     */
 /*                                                                    */
 /**********************************************************************/

 void align_and_tally(ALIGNMENT_INT *pn1, int wb, TALLY_STR *tally_table, int *perr)
{
/* local data: */
  char *proc = " align_and_tally";
  ALIGNMENT_INT al_local1, *pal  = &al_local1;
  ALIGNMENT_INT al_local2, *palt = &al_local2;
#define MAX_PHONES_IN_UTT 200

  int a[MAX_PHONES_IN_UTT], b[MAX_PHONES_IN_UTT];
  int at[MAX_PHONES_IN_UTT], bt[MAX_PHONES_IN_UTT];
  int *p1,na,nb,i,j,k,iph,jph,distance,ix,jx;
  pcodeset *pcij;
  boolean return_alignment;
#define LL 500
  char string_a[LL], *stra = &string_a[0];
  char string_b[LL], *strb = &string_b[0];
/* code: */
 db_enter_msg(proc,0); /* debug only */
/* initialize */
  *perr=0;
  pcij = pn1->pcset;
  pal->aint = &a[0];
  pal->bint = &b[0];
  palt->aint = &at[0];
  palt->bint = &bt[0];
/* go */
/* make phone strings a,b from sentences */
  na = 0;
  for (i=1; i <= pn1->aint[0]; i++)
    {iph = pn1->aint[i];
     if (iph != 0)
       {p1 = pcij->pc[iph].lc_int;
        if (wb != 0) {na += 1; a[na] = wb; }
        for (k=1; k <= p1[0]; k++)
          {if (na < MAX_PHONES_IN_UTT)
             {na += 1; a[na] = p1[k];
             }
           else
             {fprintf(stderr,"*ERR:%s: ovf on string a.\n",proc);
              decode2(stra,LL,pcij->lower_code,a,&na,perr);
              fprintf(stderr," string so far is: '%s'\n",stra);
              fprintf(stderr," Skipping processing of this alignment:\n");
              dump_int_alignment(pn1);
              *perr = 1; goto RETURN; 
    }  }  }  }
  nb = 0;
  for (j=1; j <= pn1->bint[0]; j++)
    {jph = pn1->bint[j];
     if (jph != 0)
       {p1 = pcij->pc[jph].lc_int;
        if (wb != 0) {nb += 1; b[nb] = wb; }
        for (k=1; k <= p1[0]; k++)
          {if (nb < MAX_PHONES_IN_UTT)
             {nb += 1; b[nb] = p1[k];
             }
           else
             {fprintf(stderr,"*ERR:%s: ovf on string b.\n",proc);
              decode2(strb,LL,pcij->lower_code,b,&nb,perr);
              fprintf(stderr," string so far is: '%s'\n",strb);
              fprintf(stderr," Skipping processing of this alignment:\n");
              dump_int_alignment(pn1);
              *perr = 1; goto RETURN;
    }  }  }  }
   a[0] = na;
   b[0] = nb;
   pal->aligned = F;
   pal->pcset = pcij->lower_code;
   if (!streq(pal->pcset->element_type,"phone"))
     {fprintf(stderr,"*ERR:%s: lower code = '%s'\n",
                proc,pal->pcset->element_type);
      fprintf(stderr," This fcn works only with 'phone' lower codes.\n");
      fprintf(stderr," Skipping processing of this alignment:\n");
      dump_int_alignment(pn1);
      *perr = 12; goto RETURN;
     }
   return_alignment = T;

 if (db_level > 0)
   {printf("*DB: un-aligned phone strings:\n");
    dump_int_alignment(pal);
   }

   distance = aldist2(pal,wod2,return_alignment,palt,  perr);

 if (db_level > 0)
   {printf("*DB: aligned phone strings:\n");
    dump_int_alignment(palt);
   }
   if (*perr > 0)
     fprintf(stderr,"%s: aldist2 returns err code=%d\n",
       proc,*perr);
   else
     for (k = 1; k <= palt->aint[0]; k++)
       {ix = palt->aint[k];
        jx = palt->bint[k];
        tally_table->phfreq[ix][jx] += 1;
        tally_table->phtot += 1;
       }
RETURN:
 db_leave_msg(proc,0); /* debug only */
  } /* end of function "align_and_tally" */
 /* end file alntal.c */
