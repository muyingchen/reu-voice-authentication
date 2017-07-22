
#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


/***************************************************************/
/* file sm_pkg1.c                                              */
/* a package of functions to post-process an alignment to      */
/* handle splits and merges.                                   */
/* For typical uses of the functions, see the program tald3c   */
/* or the function aldistsm.                                   */
/*                                                             */
/* Highest-level functions are:                                */
/* sm_correction2(alnx,smtab,...) - fills smtab based on alnx  */
/* sort_sm_by_iloc(smtab) -  sorts smtab by index position     */
/* put_sm_into_aln(alnx,smtab,...) - puts sm's into alnx       */
/* print_smtab1(smtab,...) - prints out an smtab               */
/*                                                             */
/* Other functions used:                                       */
/* sm_correction2(alnx,smtab,...)                              */
/*   find_sm2();                                               */
/*     rate_sm();                                              */
/*     save_sm();                                              */
/*   sort_sm_by_fom();                                         */
/*     interchange_entries();                                  */
/*   sm_overlap();                                             */
/*     limits()                                                */
/*   save_sm();                                                */
/*   dump_smtab();                                             */
/* sort_sm_by_iloc(smtab)                                      */
/*   interchange_entries();                                    */
/* put_sm_into_aln(alnx,smtab,...)                             */
/*   add_to_auxtab();                                          */
/*                                                             */
/***************************************************************/


/********************************************/
/*  void rate_sm(smrep,pal)                 */
/*  Calculates quantitative values for      */
/* rating the sm *smrep.                    */
/********************************************/

void rate_sm(SM_ELEMENT2 *smrep, ALIGNMENT_INT *pal1)
{pcodeset *pcode1;
  ALIGNMENT_INT al_local1, *pal2  = &al_local1, *pdummy = (ALIGNMENT_INT *)NULL;
  int a[MAX_SYMBS_IN_STR], b[MAX_SYMBS_IN_STR];
  ALIGNMENT_INT alxx, *palx = &alxx;
  int ax[MAX_SYMBS_IN_STR], bx[MAX_SYMBS_IN_STR];
  int i,j,k,err, *perr = &err;
  char sxax[LINE_LENGTH], *sxa = &sxax[0];
  int sxi[MAX_SYMBS_IN_STR];
  boolean return_alignment = T, dont_return_alignment = F;
  char *proc = "rate_sm";
/* code: */
db_enter_msg(proc,0); /* debug only */
/* initialize */
  pcode1 = pal1->pcset;
  pal2->aint = &a[0];
  pal2->bint = &b[0];
  pal2->aint[0] = 0;
  pal2->bint[0] = 0;
  pal2->aligned = F;
  pal2->pcset = pcode1->lower_code;
if (db_level > 1) dump_pcode2(pal2->pcset);
if (db_level > 1) dump_ppf2(pal2->pcset);
if (db_level > 1) dump_auxtab(pal1);
  i = smrep->ival; j = smrep->jval; k = smrep->kval;
  palx->aint = &ax[0];
  palx->bint = &bx[0];
if (db_level > 1)
 {printf("%sival=%d, jval=%d, kval=%d\n",pdb,i,j,k);
  printf("%ss_or_m=%c, l_or_r=%c\n",pdb,smrep->s_or_m,smrep->l_or_r);
 }
if (db_level > 1)
 {printf("%sival phones='%s'\n",pdb,ascii_str(sxa,i,pcode1,pal1));
  printf("%sjval phones='%s'\n",pdb,ascii_str(sxa,j,pcode1,pal1));
  printf("%skval phones='%s'\n",pdb,ascii_str(sxa,k,pcode1,pal1));
 }
/* compute pdist hypothesizing split or merge */
  if (smrep->s_or_m == 's')
    {nstrcpy(pal2->aint,lc_istr(sxi,i,pcode1,pal1));
     nstrcpy(pal2->bint,lc_istr(sxi,j,pcode1,pal1));
     nstrcat(pal2->bint,lc_istr(sxi,k,pcode1,pal1));
   }
  if (smrep->s_or_m == 'm')
    {nstrcpy(pal2->aint,lc_istr(sxi,i,pcode1,pal1));
     nstrcat(pal2->aint,lc_istr(sxi,j,pcode1,pal1));
     nstrcpy(pal2->bint,lc_istr(sxi,k,pcode1,pal1));
    }

 if (db_level > 1)
   {printf("%sun-aligned phone strings:\n",pdb);
    dump_int_alignment(pal2);
   }

if (db_level > 1)
   smrep->pdist_sm = aldist2(pal2,wod2,return_alignment,palx,perr);
else
   smrep->pdist_sm = aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);

   if (*perr > 0)
     {fprintf(stderr,"*ERR: aldist2 returns err code=%d\n",*perr);
      goto RETURN;
     }

if (db_level > 0) printf("%spdist_sm=%d\n",pdb,smrep->pdist_sm);
if (db_level > 1) 
 {printf("%saligned phone strings:\n",pdb);
  dump_int_alignment(palx);
  }
/* compute pdist not hypothesizing split or merge */
  if (smrep->s_or_m == 's')
    {if (smrep->l_or_r == 'l')
       {/* i->j + 0->k */
        nstrcpy(pal2->aint,lc_istr(sxi,i,pcode1,pal1));
        nstrcpy(pal2->bint,lc_istr(sxi,j,pcode1,pal1));
        smrep->pdist = aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);
        nstrcpy(pal2->aint,lc_istr(sxi,0,pcode1,pal1));
        nstrcpy(pal2->bint,lc_istr(sxi,k,pcode1,pal1));
        smrep->pdist += aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);
       }
     if (smrep->l_or_r == 'r')
       {/* 0->j + i->k */
        nstrcpy(pal2->aint,lc_istr(sxi,0,pcode1,pal1));
        nstrcpy(pal2->bint,lc_istr(sxi,j,pcode1,pal1));
        smrep->pdist = aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);
        nstrcpy(pal2->aint,lc_istr(sxi,i,pcode1,pal1));
        nstrcpy(pal2->bint,lc_istr(sxi,k,pcode1,pal1));
        smrep->pdist += aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);
       }
    }
  if (smrep->s_or_m == 'm')
    {if (smrep->l_or_r == 'l')
       {/* i->k + j->0 */
        nstrcpy(pal2->aint,lc_istr(sxi,i,pcode1,pal1));
        nstrcpy(pal2->bint,lc_istr(sxi,k,pcode1,pal1));
        smrep->pdist = aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);
        nstrcpy(pal2->aint,lc_istr(sxi,j,pcode1,pal1));
        nstrcpy(pal2->bint,lc_istr(sxi,0,pcode1,pal1));
        smrep->pdist += aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);
       }
     if (smrep->l_or_r == 'r')
       {/* i->0 + j->k*/
        nstrcpy(pal2->aint,lc_istr(sxi,i,pcode1,pal1));
        nstrcpy(pal2->bint,lc_istr(sxi,0,pcode1,pal1));
        smrep->pdist = aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);
        nstrcpy(pal2->aint,lc_istr(sxi,j,pcode1,pal1));
        nstrcpy(pal2->bint,lc_istr(sxi,k,pcode1,pal1));
        smrep->pdist += aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);
       }
    }

if (db_level > 0) printf("%spdist=%d\n",pdb,smrep->pdist);

  smrep->fom = ((double)smrep->pdist_sm == 0) ? HUGE_VAL : 
      ((double)smrep->pdist)/((double)smrep->pdist_sm);

if (db_level > 0) printf("%sfigure_of_merit=%f\n",pdb,smrep->fom);

RETURN:
db_leave_msg(proc,0); /* debug only */
  return;
 } /* end fcn rate_sm() */

/******************************************************/
/*  fcn interchange_entries(table,i,j)                */
/*  interchanges entries i and j in  *table.          */
/******************************************************/

void interchange_entries(SM_TAB_STR2 *table, int i, int j)
{SM_ELEMENT2 temp_entry;
  temp_entry = table->entry[i];
  table->entry[i] = table->entry[j];
  table->entry[j] = temp_entry;
 }

/********************************************/
/*  void sort_sm_by_fom(smtab)              */
/*  Sorts the sm table *smtab by fom        */
/*  Uses a simple sort that is just fine if */
/*  file is short or already almost sorted. */
/********************************************/

void sort_sm_by_fom(SM_TAB_STR2 *smtab)
{int i,j;
  for(i = 0; i < smtab->nsaved - 1; i++)
    {for (j = i + 1; j < smtab->nsaved; j++)
       {if (smtab->entry[j].fom > smtab->entry[i].fom)
          interchange_entries(smtab,i,j);
 }  }  }
/* end function sort_sm_by_fom() */

/********************************************/
/*  void sort_sm_by_iloc(smtab)             */
/*  Sorts the sm table *smtab into          */
/* descending order by iloc.                */
/*  Uses a simple sort that is just fine if */
/*  file is short or already almost sorted. */
/********************************************/

void sort_sm_by_iloc(SM_TAB_STR2 *smtab)
{int i,j;
  for(i = 0; i < smtab->nsaved - 1; i++)
    {for (j = i + 1; j < smtab->nsaved; j++)
       {if (smtab->entry[j].iloc > smtab->entry[i].iloc)
          interchange_entries(smtab,i,j);
 }  }  }
/* end function sort_sm_by_iloc() */


/**********************************************/
/*  void save_sm(smrep,smtab)                 */
/*  Stores the s/m entry *smrep into the s/m  */
/* table *smtab                               */
/**********************************************/

void save_sm(SM_ELEMENT2 *smrep, SM_TAB_STR2 *smtab)
{char *proc = "save_sm";
 db_enter_msg(proc,0); /* debug only */
  if (smtab->nsaved < MAX_SMTAB_ENTRIES)
    {smtab->nsaved += 1;
     smtab->entry[smtab->nsaved - 1] = *smrep;
if (db_level > 0) printf("%snew nsaved = %d\n",pdb,smtab->nsaved);
    }
  else
    {fprintf(stderr,"*ERR:%s: SMTAB overflow.\n",proc);
    }
 db_leave_msg(proc,0); /* debug only */
 }
/* end function save_sm() */


/********************************************/
/*  void dump_smtab(smtab)                  */
/*  dumps the split/merge info in *smtab.   */
/********************************************/
void dump_smtab(SM_TAB_STR2 *smtab)
{int ix;
  SM_ELEMENT2 x;
  fprintf(stdout,"\n");
  fprintf(stdout," DUMP OF SPLIT/MERGE TABLE INFO\n");
  fprintf(stdout," nsaved = %d\n",smtab->nsaved);
  fprintf(stdout," nover  = %d\n",smtab->nover);
  fprintf(stdout,
    "           figure\n");
  fprintf(stdout,
    "    freq  of merit  pdist  pdist_sm  iloc    i     j     k  s/m l/r\n");
  for (ix = 0; ix < smtab->nsaved; ix++)
    {x = smtab->entry[ix];
     fprintf(stdout," %7lu  %7.3f %6d %6d    %5d %5d %5d %5d   %c   %c\n",
        x.frequency,x.fom,x.pdist,x.pdist_sm,x.iloc,
        x.ival,x.jval,x.kval,x.s_or_m,x.l_or_r);
    }
  fprintf(stdout,"\n");
 }
/* end function dump_smtab() */



/********************************************************************/
/*  fcn find_sm2(pal,fom_threshold,smtab)                           */
/*  looks in *pal for splits/merges to store in  s/m table *smtab.  */
/********************************************************************/
void find_sm2(ALIGNMENT_INT *pal, double fom_threshold, SM_TAB_STR2 *smtab)
{int ix; SM_ELEMENT2 smrep;
 char *proc = "find_sm2";
db_enter_msg(proc,0); /* debug only */
if (db_level > 0) printf("%sfom_threshold='%f'\n",pdb,fom_threshold);
/* initialize */
  smrep.pdist = 0;
  smrep.pdist_sm = 0;
  smrep.frequency = 0;
  smtab->nsaved = 0;
/* check for splits */
  for (ix=1; ix <= pal->aint[0]; ix++)
    {if (ix > 1)   /*check for left splits and merges */
       {smrep.l_or_r = 'l';
        if (pal->aint[ix] == 0)  /* split */
          {smrep.s_or_m = 's';
if (db_level > 1) printf("%spotential split @ix=%d\n",pdb,ix);
           if ((pal->aint[ix-1] != 0)&&(pal->bint[ix-1] != 0))
             {smrep.ival = pal->aint[ix-1];
              smrep.jval = pal->bint[ix-1];
              smrep.kval = pal->bint[ix];
              smrep.iloc = ix-1;
              rate_sm(&smrep,pal);
              if (smrep.fom > fom_threshold) save_sm(&smrep,smtab);
          }  }
        if (pal->bint[ix] == 0)  /* merge */
          {smrep.s_or_m = 'm';
if (db_level > 1) printf("%spotential merge @ix=%d\n",pdb,ix);
           if ((pal->aint[ix-1] != 0)&&(pal->bint[ix-1] != 0))
             {smrep.ival = pal->aint[ix-1];
              smrep.jval = pal->aint[ix];
              smrep.kval = pal->bint[ix-1];
              smrep.iloc = ix-1;
              rate_sm(&smrep,pal);
              if (smrep.fom > fom_threshold) save_sm(&smrep,smtab);
       }  }  }
     if (ix < pal->aint[0])   /*check for right splits and merges */
       {smrep.l_or_r = 'r';
        if (pal->aint[ix] == 0)  /* split */
          {smrep.s_or_m = 's';
           if ((pal->aint[ix+1] != 0)&&(pal->bint[ix+1] != 0))
             {smrep.ival = pal->aint[ix+1];
              smrep.jval = pal->bint[ix];
              smrep.kval = pal->bint[ix+1];
              smrep.iloc = ix+1;
              rate_sm(&smrep,pal);
              if (smrep.fom > fom_threshold) save_sm(&smrep,smtab);
          }  }
        if (pal->bint[ix] == 0)  /* merge */
          {smrep.s_or_m = 'm';
           if ((pal->aint[ix+1] != 0)&&(pal->bint[ix+1] != 0))
             {smrep.ival = pal->aint[ix];
              smrep.jval = pal->aint[ix+1];
              smrep.kval = pal->bint[ix+1];
              smrep.iloc = ix;
              rate_sm(&smrep,pal);
              if (smrep.fom > fom_threshold) save_sm(&smrep,smtab);
    }  }  }  }
db_leave_msg(proc,0); /* debug only */
  return;
 }
/* end function find_sm2() */


/****************************************************/
/*  struct INTERVAL1 limits(smrep)                  */
/* Returns the interval of indexes from *smrep.     */
/****************************************************/

struct INTERVAL1 limits(SM_ELEMENT2 *smrep)
{char *proc = "limits";
  struct INTERVAL1 x;
db_enter_msg(proc,0); /* debug only */
  if (smrep->s_or_m == 'm')
    {x.l1 = smrep->iloc;
     x.l2 = smrep->iloc + 1;
    }
  else
    {if (smrep->l_or_r == 'l')
       {x.l1 = smrep->iloc;
        x.l2 = smrep->iloc + 1;
       }
     else
       {x.l1 = smrep->iloc - 1;
        x.l2 = smrep->iloc;
    }  }
db_leave_msg(proc,0); /* debug only */
  return x;
 }



/****************************************************/
/*  boolean sm_overlap(i,j,smtab)                   */
/*  Determines if there is an overlap between       */
/* entries i and j in the split/merge table *smtab. */
/****************************************************/

boolean sm_overlap(int i, int j, SM_TAB_STR2 *smtab)
{char *proc = "sm_overlap";
  boolean ovl;
  SM_ELEMENT2 *ismrep,*jsmrep;
db_enter_msg(proc,0); /* debug only */
/* initialize */
  ovl = F;
  ismrep = (SM_ELEMENT2*)&(smtab->entry[i]);
  jsmrep = (SM_ELEMENT2*)&(smtab->entry[j]);
  if (overlap1(limits(ismrep),limits(jsmrep)) > 0) ovl = T;
  else                                             ovl = F;
db_leave_msg(proc,0); /* debug only */
  return ovl;
 }


 /**********************************************************************/
 /*                                                                    */
 /*   int sm_correction2(pt1,fom_threshold,smtab,perr)                 */
 /*                                                                    */
 /*   ALIGNMENT_INT *pt1; double fom_threshold;                        */
 /*   SM_TAB_STR2 *smtab; int *perr;                                   */
 /*                                                                    */
 /*    Determines the correction to the alignment distance of the      */
 /* alignment in *pt1 that should be made on account of splits and     */
 /* merges.  Returns this correction and leaves the s/m's in the       */
 /* split/merge table *smtab.  Only s/m's with a figure of merit       */
 /* greater than <fom_threshold> are kept.                             */
 /*    To get the s/m's, this version uses find_sm2(), which calls     */
 /* rate_sm(), which uses aldist2() to calculate the distance          */
 /* between the lower-code versions of the symbols in *pt1.            */
 /* If the integers in the strings in *pt1 are greater than the        */
 /* highest code in pt1's pcode, then they represent entries in the    */
 /* auxiliary table auxtab[], which represents a list of pcode symbols */
 /* to be regarded as one.                                             */
 /*    This version DOES zero the smtab but not the auxtab.            */
 /*                                                                    */
 /*   *perr = 0 means a.o.k.                                           */
 /*   error codes:                                                     */
 /*   11:invalid input parameters                                      */
 /*                                                                    */
 /**********************************************************************/

  int sm_correction2(ALIGNMENT_INT *pt1, double fom_threshold, SM_TAB_STR2 *smtab, int *perr)
{
/* data: */
    char *proc = "sm_correction2";
    int correction,i,j;
    SM_TAB_STR2 smtabxx, *smtabx = &smtabxx;
    boolean sm_used[MAX_SMTAB_ENTRIES], conflict;
/* code: */

 /**** start debug ****/
db_enter_msg(proc,0); /* debug only */
if (db_level > 0 )
   {printf("%sthe pcode is '%s'\n",pdb,(pt1->pcset)->name);
    printf("%sna =%d, nb =%d\n",pdb,pt1->aint[0],pt1->bint[0]);
    printf("%sthe input strings to align are:\n\n",pdb);
    dump_int_alignment(pt1);
   }
 /**** debug ****/

 /* initialize output variables */
  *perr=0;
  correction = 0;
  smtab->nsaved = 0;
/* get the preliminary list of s/m's */
  find_sm2(pt1,fom_threshold,smtabx);
  sort_sm_by_fom(smtabx);
if (db_level > 0) dump_smtab(smtabx);
  if (smtabx->nsaved > 0)
    {/* mark all sm candidates as unused */
     for (i=0; i < smtabx->nsaved; i++) sm_used[i] = F;
     /* go down the list, using the highest one that hasn't been
        used yet and doesn't conflict (overlap) with one that has
        already been chosen (used). */
     for (i=0; i < smtabx->nsaved; i++)
       {if (!sm_used[i])
          {conflict = F;
           for (j=1; (!conflict)&&(j < i); j++)
             if (sm_used[j]) conflict = sm_overlap(i,j,smtabx);
           if (!conflict)
             {save_sm(&(smtabx->entry[i]),smtab);
              correction += (smtabx->entry[i].pdist
                             -smtabx->entry[i].pdist_sm);
    }  }  }  }

/**** de-bug ****/
 if (db_level > 0) printf("%scorrection=%d\n",pdb,correction);
 db_leave_msg(proc,0); /* debug only */
/**** de-bug ****/

  return correction;
  } /* end of function "sm_correction2" */



 /**********************************************************************/
 /*                                                                    */
 /*   int add_to_auxtab(i,j,pal)                                       */
 /*   int i,j; ALIGNMENT_INT *pal;                                     */
 /* Adds the entry (i+j) to the auxiliary symbol table pal->auxtab[],  */
 /* returning the index of the new entry.                              */
 /* NOTE: i and j may themselves be either pcode symbols or symbols in */
 /*pal->auxtab[].  Returns 0 on ovf.                                   */
 /* (If i or j is an entry in the auxtab[], as indicated by being a    */
 /* value greater than pal->pcset->pctab_length, the ith or jth entry  */
 /* is just added to; otherwise, a new entry is created and *pnaux is  */
 /*  bumped.                                                           */
 /*                                                                    */
 /**********************************************************************/

 int add_to_auxtab(int i, int j, ALIGNMENT_INT *pal)
{
/* data: */
   char *proc = "add_to_auxtab";
   int iaux,jaux,kaux,k,n,iret;
   int auxx[MAX_SYMBS_IN_STR];
/* code: */
 db_enter_msg(proc,0); /* debug only */
   if (i > pal->pcset->pctab_length)
     {iaux = i - pal->pcset->pctab_length;
      if (j > pal->pcset->pctab_length)
        {jaux = j - pal->pcset->pctab_length;
         /* add aux j onto tail of aux i */
         pal->auxtab[iaux] = nstrcat(pal->auxtab[iaux],pal->auxtab[jaux]);
         iret = iaux;
         /* delete j from auxtab */
         for (kaux=jaux+1; kaux<=pal->naux; kaux++)
           {/*free auxtab[kaux-1]?? */
            pal->auxtab[kaux-1] = pal->auxtab[kaux];
           }
         pal->naux -= 1;
        }
      else
        {/* add word j onto tail of aux i */
         n = pal->auxtab[iaux][0];
         pal->auxtab[iaux][n+1] = j;
         pal->auxtab[iaux][0] += 1;
         iret = iaux;
     }  }
   else
     {if (j > pal->pcset->pctab_length)
        {/* add word i onto head of aux j */
         jaux = j - pal->pcset->pctab_length;
         n = pal->auxtab[jaux][0];
         for (k=n; k > 0; k--)
           pal->auxtab[jaux][k+1] = pal->auxtab[jaux][k];
         pal->auxtab[jaux][1] = i;
         pal->auxtab[jaux][0] += 1;
         iret = jaux;
        }
      else
        {/* make new entry for word i + word j */
         if (pal->naux < MAX_AUX_SYMBOLS)
           {pal->naux += 1;
            auxx[0] = 2;
            auxx[1] = i;
            auxx[2] = j;
            pal->auxtab[pal->naux] = nstrdup(auxx);
            iret = pal->naux;
           }
         else
           {fprintf(stderr,"*ERR:%s: auxtab ovf.\n",proc);
            iret = 0;
     }  }  }
 
 db_leave_msg(proc,0); /* debug only */
   return iret;
  } /* end of function "add_to_auxtab" */


 /**********************************************************************/
 /*                                                                    */
 /*   void put_sm_into_aln(pal,smtab,perr)                             */
 /*                                                                    */
 /*   ALIGNMENT_INT *pal; SM_TAB_STR2 *smtab; int *naux,*perr;         */
 /*                                                                    */
 /*    Puts the splits/merges in smtab into the alignment *pal and the */
 /* auxiliary table pal->auxtab[] (and pal->naux).                     */
 /*   *perr = 0 means a.o.k.                                           */
 /*   error codes:                                                     */
 /*   11:invalid input parameters                                      */
 /*                                                                    */
 /**********************************************************************/

 void put_sm_into_aln(ALIGNMENT_INT *pal, SM_TAB_STR2 *smtab, int *perr)
{
/* data: */
   char *proc = "put_sm_into_aln";
   SM_ELEMENT2 smrepx, *smx = &smrepx;
   int ix,k,iaux;
   int dbi;
/* code: */

 /**** start debug ****/
db_enter_msg(proc,0); /* debug only */
if (db_level > 0 )
   {printf("%sthe pcode is '%s'\n",pdb,(pal->pcset)->name);
    printf("%sna =%d, nb =%d\n",pdb,pal->aint[0],pal->bint[0]);

    printf("%spal->aint =",pdb);
    for (dbi=1; dbi <= pal->aint[0]; dbi++) printf(" %5d",pal->aint[dbi]);
    printf("\n");
    printf("%spal->bint =",pdb);
    for (dbi=1; dbi <= pal->bint[0]; dbi++) printf(" %5d",pal->bint[dbi]);
    printf("\n");

    printf("%sthe alignment strings are:\n\n",pdb);
    dump_int_alignment(pal);
    printf("%ssmtab->nsaved=%d\n",pdb,smtab->nsaved);
    printf("%sand the smtab is:\n",pdb);
    dump_smtab(smtab);
    printf("%sand the auxtab is:\n",pdb);
    dump_auxtab(pal);
   }
 /**** debug ****/

 /* initialize */
   *perr=0;
/* check input data */
 /* NOTE - SMTAB ENTRIES MUST BE IN DECREASING ORDER BY ILOC */
    for (ix = 0; ix < smtab->nsaved - 1; ix++)
      if (smtab->entry[ix].iloc < smtab->entry[ix+1].iloc)
        {fprintf(stdout,
          "*ERR:%s: smtab not in decreasing order by iloc.\n",proc);
         *perr = 11; goto RETURN;
        }
/* loop on s/m's */
   for (ix = 0; ix < smtab->nsaved; ix++)
     {*smx = smtab->entry[ix];
if (db_level > 0) printf("%sprocessing smtab entry %d\n",pdb,ix);
      if (smx->s_or_m == 's') /* SPLIT */
        {/* combine j and k into one aux symbol */
         iaux = add_to_auxtab(smx->jval,smx->kval,pal);
         if (iaux > 0)
           {/* replace HYP jval i aln with iaux + pal->pcset->pctab_length*/
            /* and delete REF null and HYP kval         */
            if (smx->l_or_r == 'l')  /* LEFT */
              {pal->bint[smx->iloc] = iaux + pal->pcset->pctab_length;
               for (k=smx->iloc+2; k <= pal->bint[0]; k++)
                 {pal->aint[k-1] = pal->aint[k];
                  pal->bint[k-1] = pal->bint[k];
              }  }
            else                     /* RIGHT */
              {pal->bint[smx->iloc - 1] = iaux + pal->pcset->pctab_length;
               for (k=smx->iloc; k <= pal->aint[0]; k++)
                 {pal->aint[k-1] = pal->aint[k];
                 } 
               for (k=smx->iloc + 1; k <= pal->bint[0]; k++)
                 {pal->bint[k-1] = pal->bint[k];
           }  }  }
         else
           {fprintf(stderr,"*ERR:%s: auxtab ovf.\n",proc);
            *perr = 11; goto RETURN;
        }  }
      else     /* MERGE */
        {/* combine i and j into one aux symbol */
         iaux = add_to_auxtab(smx->ival,smx->jval,pal);
         if (iaux > 0)
           {/* replace REF ival in aln with iaux + pal->pcset->pctab_length */
            /* and delete HYP null and REF jval         */
            if (smx->l_or_r == 'l')  /* LEFT */
              {pal->aint[smx->iloc] = iaux + pal->pcset->pctab_length;
               for (k=smx->iloc+2; k <= pal->bint[0]; k++)
                 {pal->aint[k-1] = pal->aint[k];
                  pal->bint[k-1] = pal->bint[k];
              }  }
            else                     /* RIGHT */
              {pal->aint[smx->iloc] = iaux + pal->pcset->pctab_length;
               for (k=smx->iloc+2; k <= pal->aint[0]; k++)
                 {pal->aint[k-1] = pal->aint[k];
		 }
               for (k=smx->iloc+1; k <= pal->bint[0]; k++)
                 {pal->bint[k-1] = pal->bint[k];
           }  }  }
         else
           {fprintf(stderr,"*ERR:%s: auxtab ovf.\n",proc);
            *perr = 11; goto RETURN;
        }  }
      pal->aint[0] -= 1;
      pal->bint[0] -= 1;
     }  /* END OF LOOP OVER SMTAB */
RETURN:

if (db_level > 0)
  {printf("%son exit,\n",pdb);
    printf("%spal->aint =",pdb);
    for (dbi=1; dbi <= pal->aint[0]; dbi++) printf(" %5d",pal->aint[dbi]);
    printf("\n");
    printf("%spal->bint =",pdb);
    for (dbi=1; dbi <= pal->bint[0]; dbi++) printf(" %5d",pal->bint[dbi]);
    printf("\n");
    printf("%sand the auxtab is:\n",pdb);
    dump_auxtab(pal);
  }

 db_leave_msg(proc,0); /* debug only */
   return;
  } /* end of function "put_sm_into_aln" */

/* end file sm_pkg1.c */
