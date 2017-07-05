/* file nsylbs1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

int nsylbs1(PCIND_T iword, pcodeset *pcode_word, int *perr)
/******************************************************************/
/*  Computes & returns the number of syllables (actually, the     */
/* number of syllabic phones) in word *iword of word pcodeset     */
/* *pcode_word.                                                   */
/*  Uses the external feature representations of the phones that  */
/* are in the phonemic representation of the word.                */
/*  Returns *perr == 0 to signal "a.o.k.", > 0 to signal error.   */
/******************************************************************/
 {char proc[] = "nsylbs1";
  int n_phones, i, nsyl;
  PCIND_T iph;
  pcodeset *pcode_phon;
/* code */
db_enter_msg(proc,0); /* debug only */

if (db_level > 0) printf("%s pcode_word='%s', iword=%d\n",pdb,
pcode_word->name,iword);

  *perr = 0;
  nsyl = 0;
  n_phones = pcode_word->pc[iword].lc_int[0];
  pcode_phon = pcode_word->lower_code;
/* error check - is there a lower phonemic code? */
  if (pcode_phon == NULL)
    {fprintf(stderr,"*ERR:%s: no phonemic lower code is defined.\n",proc);
     *perr = 11;
     goto RETURN;
    }
/* error check - for the phonemic code, is its lower code (features) defined? */ 
  if (pcode_phon->pc[1].lc_ext == NULL)
    {fprintf(stderr,"*ERR:%s: features of phones are not defined.\n",proc);
     *perr = 12;
     goto RETURN;
    }
/* now count */

if (db_level > 0) printf("%sn_phones = %d\n",pdb,n_phones);

  for (i = 1; i <= n_phones; i++)
    {iph = pcode_word->pc[iword].lc_int[i];

if (db_level > 0) printf("%sph='%s',feats='%s'\n",pdb,
pcode_phon->pc[iph].ac,pcode_phon->pc[iph].lc_ext);

     if (strstr(pcode_phon->pc[iph].lc_ext,"syl") != NULL) nsyl += 1;
    }
RETURN:
db_leave_msg(proc,0); /* debug only */
  return nsyl;
 }
/* end of file nsylbs1.c */
