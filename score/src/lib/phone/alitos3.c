 /* File alitos3.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void aln_i_to_s_fancy(pia1,psa1,perr)                            */
 /*   ALIGNMENT_INT *pia1; ALIGNMENT_CHR *psa1; int *perr;             */
 /*                                                                    */
 /*    Converts the integer alignment *pia1 into a character string    */
 /* alignment *psa1, fancily.  Puts strings of asterisks, and          */
 /* changes the type case of symbols that are in error.                */
 /*                                                                    */
 /*   *perr = 0 means a.o.k.                                           */
 /*           11  "   *pia1 is aligned, but len(aint) != len(bing)     */
 /*           13  "   naux > 0 (handles simple alignments only)        */
 /*                                                                    */
 /**********************************************************************/

 void aln_i_to_s_fancy(ALIGNMENT_INT *pia1, ALIGNMENT_CHR *psa1, int *perr)
{
/* local data: */
  char *proc = " aln_i_to_s_fancy";
  int na,nb,nsymbs,nspaces,i,k,li,lj,ichr,jchr;
  char *pi,*pj;
  char xsymb[MAX_STR_NCHARS], *x = &xsymb[0];
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
        fprintf(stderr," SYSTEM PROBLEM\n");
/*        exit(2); */ /* this line for testing only */
        *perr = 11;
        goto RETURN;
       } /* "if (pia1->nb ... " */
     nsymbs = na;
     i=1; pi = psa1->pastr; pj = psa1->pbstr;
     while (i <= nsymbs)
	{li = 0; lj = 0;
      /* calc indexes to i symbol and j symbol */
         ichr = pia1->aint[i];
         jchr = pia1->bint[i];
      /* if separated, put " " into *pi and *pj */
         if ( (pc1->separated) && (i>1) )
	   {pi = strcat(pi," "); li += 1;
            pj = strcat(pj," "); lj += 1;
	   }
      /* put ascii for symbol #ichr into *pi */
         if (ichr == 0)
           {for (k=1; k<=strlen(pc1->pc[jchr].ac); k++)
             {pi = strcat(pi,"*"); li += 1;}
	   }
         else
	   {if (streq(pc1->pc[ichr].ac,pc1->pc[jchr].ac))
              pi = strcat(pi,pc1->pc[ichr].ac);
            else  /* switch case */
              {if (streq(pc1->type_case,"upper"))
                  pi = strcat(pi,make_lower(x=strcpy(x,pc1->pc[ichr].ac)));
               else
                 {if (streq(pc1->type_case,"lower"))
                    pi = strcat(pi,make_upper(x=strcpy(x,pc1->pc[ichr].ac)));
                  else
                    pi = strcat(pi,pc1->pc[ichr].ac);
	         }
	      }
            li += strlen(pc1->pc[ichr].ac);
	   }
      /* put ascii for symbol #jchr into *pj */
         if (jchr == 0)
           {for (k=1; k<=strlen(pc1->pc[ichr].ac); k++)
             {pj = strcat(pj,"*"); lj += 1;}
	   }
         else
	   {if (streq(pc1->pc[jchr].ac,pc1->pc[ichr].ac))
              pj = strcat(pj,pc1->pc[jchr].ac);
            else  /* switch case */
              {if (streq(pc1->type_case,"upper"))
                  pj = strcat(pj,make_lower(x=strcpy(x,pc1->pc[jchr].ac)));
               else
                 {if (streq(pc1->type_case,"lower"))
                    pj = strcat(pj,make_upper(x=strcpy(x,pc1->pc[jchr].ac)));
                  else
                    pj = strcat(pj,pc1->pc[jchr].ac);
	         }
	      }
            lj += strlen(pc1->pc[jchr].ac);
	   }
	 if (i < nsymbs)
	   {nspaces = abs(lj-li);
	    for (k=1; k<=nspaces; k++)
               {if (lj > li) pi = strcat(pi," ");
                else         pj = strcat(pj," ");
	       }; /* "for (k=1;..." */
	   } /* "if (i<nsymbs) ..." */
	 i++;
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
  } /* end of function "aln_i_to_s_fancy" */
 /* end file alitos3.c */
