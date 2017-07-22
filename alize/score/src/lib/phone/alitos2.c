 /* File alitos2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void alignment_i_to_s(pia1,psa1,perr)                            */
 /*   ALIGNMENT_INT *pia1; ALIGNMENT_CHR *psa1; int *perr;             */
 /*                                                                    */
 /*    Converts the integer alignment *pia1 into a character string    */
 /* alignment *psa1.                                                   */
 /*                                                                    */
 /*   *perr = 0 means a.o.k.                                           */
 /*           11  "   *pia1 is aligned, but len(aint) != len(bing)     */
 /*           13  "   naux > 0 (handles simple alignments only)        */
 /*                                                                    */
 /**********************************************************************/

 void alignment_i_to_s(ALIGNMENT_INT *pia1, ALIGNMENT_CHR *psa1, int *perr)
{
/* local data: */
  char *proc = " alignment_i_to_s";
  int na,nb,nsymbs,nspaces,i,j,k,li,lj,ichr,jchr;
  char *pi,*pj;
  pcodeset *pc1;
/* code: */
 db_enter_msg(proc,3); /* debug only */
/* error check */
  if (pia1->naux > 0)
    {fprintf(stderr,"*ERR:%s: cannot handle naux=%d\n",proc,pia1->naux);
/*     exit(2); */ /* this line for testing only */
     *perr = 15;
     goto RETURN;
    }
/* initialize */
  *perr=0;
  pc1 = pia1->pcset;
/* go */
  psa1->aligned = pia1->aligned;
  psa1->pastr = strcpy(psa1->pastr,"");
  psa1->pbstr = strcpy(psa1->pbstr,"");
  na = pia1->aint[0];
  nb = pia1->bint[0];
  if (pia1->aligned)
    {if (nb != na)
       {fprintf(stderr,"%s: pia1 is supposed to be aligned, but\n",proc);
        fprintf(stderr," length(aint) NOT = length(bint).\n");
        fprintf(stderr," length(aint)=%d, length(bint)=%d\n",na,nb);
        fprintf(stderr," SYSTEM PROBLEM\n");
/*        exit(2); */ /* this line for testing only */
        *perr = 11;
        goto RETURN;
       } /* "if (pia1->nb ... " */
     nsymbs = na;
     i=1; j=i; pi = psa1->pastr; pj = psa1->pbstr;
     while (i <= nsymbs)
	{li = 0; lj = 0;
             /* put ascii(int[i]) into *pi, add to li */
         if ( (pc1->separated) && (i>1) )
	   {pi = strcat(pi," "); li += 1;}
         ichr = pia1->aint[i];
	 pi = strcat(pi,pc1->pc[ichr].ac);
	 li += strlen(pc1->pc[ichr].ac);
          /* put ascii(int[j]) into *pj, add to lj */
         if ( (pc1->separated) && (j>1) )
           {pj = strcat(pj," "); lj=1;}
	 jchr = pia1->bint[j];
         pj = strcat(pj,pc1->pc[jchr].ac);
	 lj += strlen(pc1->pc[jchr].ac);
	 if (i < nsymbs)
	   {nspaces = abs(lj-li);
	    for (k=1; k<=nspaces; k++)
               {if (lj > li) pi = strcat(pi," ");
                else         pj = strcat(pj," ");
	       }; /* "for (k=1;..." */
	   } /* "if (i<nsymbs) ..." */
	 i++; j++;
        }; /* "while (i <= nsymbs)... " */
    } /* " if (pia1->aligned) ... else x" */
  else
    {decode2(psa1->pastr,MAX_STR_NCHARS,pc1,&(pia1->aint[0]),&na,perr);
     if (*perr > 0)
        fprintf(stderr,"Decode of string a returns perr=%d\n",*perr);
     decode2(psa1->pbstr,MAX_STR_NCHARS,pc1,&(pia1->bint[0]),&nb,perr);
     if (*perr > 0)
        fprintf(stderr,"Decode of string b returns perr=%d\n",*perr);
    }; /*end of "if (pia1->aligned) x; else ..." */
RETURN:
 db_leave_msg(proc,3); /* debug only */
  } /* end of function "alignment_i_to_s" */
 /* end file alitos2.c */
