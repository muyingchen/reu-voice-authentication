 /* File alstoi2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void alignment_s_to_i(pia1,psa1,pcode1,perr)                     */
 /*   ALIGNMENT_INT *pia1; ALIGNMENT_CHR *psa1; pcode *pc1, int *perr; */
 /*                                                                    */
 /*    Converts the character string alignment *psa1 into an           */
 /* integer alignment *psa1, using pcode *pcode1.                      */
 /*                                                                    */
 /*   *perr = 0 means a.o.k.                                           */
 /*           1 means encoding error                                   */
 /*           2 means pia1->aligned == T, but na != nb                 */
 /*                                                                    */
 /**********************************************************************/

  void alignment_s_to_i(ALIGNMENT_INT *pia1, ALIGNMENT_CHR *psa1, pcodeset *pcode1, int *perr)
{
/* local data: */
  char *proc = " alignment_s_to_i";
/* code: */
 db_enter_msg(proc,3); /* debug only */
/* initialize */
  *perr=0;
/* go */
  pia1->aligned = psa1->aligned;
  encode2(psa1->pastr,pcode1,&(pia1->aint[0]),&(pia1->aint[0]),perr);
  if (*perr > 0)
    {fprintf(stderr,"*ERR:%s: encode2 returns *perr=%d\n",proc,*perr);
     fprintf(stderr," trying to encode '%s'\n",psa1->pastr);
     fprintf(stderr," using pcode '%s'\n",pcode1->name);
     *perr = 1;
     goto RETURN;}
  encode2(psa1->pbstr,pcode1,&(pia1->bint[0]),&(pia1->bint[0]),perr);
  if (*perr > 0)
    {fprintf(stderr,"*ERR:%s: encode2 returns *perr=%d\n",proc,*perr);
     fprintf(stderr," trying to encode '%s'\n",psa1->pbstr);
     fprintf(stderr," using pcode '%s'\n",pcode1->name);
     *perr = 1;
     goto RETURN;}
  if ((pia1->aligned) && (pia1->aint[0] != pia1->bint[0]))
    {fprintf(stderr,"*ERR:%s: psa1 is supposed to be aligned, but\n",proc);
     fprintf(stderr," length(aint) NOT = length(bint).\n");
     *perr = 11;
     goto RETURN;
    } /* "if (pia1->aligned ... " */
 RETURN:
 db_leave_msg(proc,3); /* debug only */
  } /* end of function "alignment_s_to_i" */
 /* end file alstoi2.c */
