/* file dintal2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/************************************************************/
/*  void dump_int_alignment(p1)                             */
/*  dumps *p1, an ALIGNMENT_INT                             */
/************************************************************/
 void dump_int_alignment(ALIGNMENT_INT *p1)
 {char *proc = " dump_int_alignment";
  ALIGNMENT_CHR *p2;
  int err, *perr = &err;
 db_enter_msg(proc,4); /* debug only */
  p2 = make_al_chr();
  if (p2 == NULL)
    {fprintf(stdout,"%s: make_al_chr err=%d\n",proc,*perr);
     goto RETURN;
    }
  if (p1->naux > 0) aln_i_to_s_fancy2(p1,p2,perr);
  else               alignment_i_to_s(p1,p2,perr);
  if (*perr > 0)
    {fprintf(stdout,"%s: converting to s alignment err=%d\n",proc,*perr);
     goto RETURN;
    }
  dump_chr_alignment(p2);
RETURN:
  free_al_chr(p2);
 db_leave_msg(proc,4); /* debug only */
  return;
 } /* end dump_int_alignment */
 /* end file dintal2.c */
