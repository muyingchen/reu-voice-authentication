/* file aldist_static.c */


#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   int aldist_static(pal,perr)                                      */
 /*                                                                    */
 /*   ALIGNMENT_INT *pal; int *perr;                                   */
 /*                                                                    */
 /*    Computes and returns the alignment distance between the         */
 /* symbols in the two integer strings in *pal (aint[] and bint[]).    */
 /*    Assumes the strings are already in alignment; designed to work  */
 /* with pal->auxtab[].                                                */
 /*                                                                    */
 /*   *perr = 0 means a.o.k.                                           */
 /*   error codes:                                                     */
 /*   11:invalid input parameters                                      */
 /*                                                                    */
 /**********************************************************************/

  int aldist_static(ALIGNMENT_INT *pal, int *perr)
  {
/* data: */
  char *proc = "aldist_static";
  pcodeset *pcode1;
  int i,j,k,distance;
  ALIGNMENT_INT al_local1, *pal2  = &al_local1, *pdummy;
  int a[MAX_SYMBS_IN_STR], b[MAX_SYMBS_IN_STR];
  int sxi[MAX_SYMBS_IN_STR];
  boolean dont_return_alignment = F;
/* code: */
db_enter_msg(proc,0); /* debug only */
  *perr=0;
  pcode1 = pal->pcset;
  distance = 0;
  pal2->aint = &a[0];
  pal2->bint = &b[0];
  pal2->aint[0] = 0;
  pal2->bint[0] = 0;
  pal2->aligned = F;
  pal2->pcset = pcode1->lower_code;
  if (!pal->aligned)
    {fprintf(stderr,"%s: *ERR: input alignment NOT ALIGNED.\n",proc);
     *perr=11; goto RETURN;
    }
  if (pal->aint[0] != pal->bint[0])
    {fprintf(stderr,"%s: *ERR: astr length=%d but bstr length=%d.\n",
      proc,pal->aint[0],pal->bint[0]);
     *perr=11; goto RETURN;
    }
  pcode1 = pal->pcset;
  for (k=1; k <= pal->aint[0]; k++) 
    {i = pal->aint[k];
     nstrcpy(pal2->aint,lc_istr(sxi,i,pcode1,pal));
     j = pal->bint[k];
     nstrcpy(pal2->bint,lc_istr(sxi,j,pcode1,pal));
     distance += aldist2(pal2,wod2,dont_return_alignment,pdummy,perr);
    }
RETURN:
 db_leave_msg(proc,0); /* debug only */
  return distance;
  } /* end of function "aldist_static" */
