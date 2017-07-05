/* file dchral.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/************************************************************/
/*  void dump_chr_alignment(p1)                             */
/*  dumps *p1, an ALIGNMENT_CHR                             */
/************************************************************/
 void dump_chr_alignment(ALIGNMENT_CHR *p1)
{char *proc = " dump_chr_alignment";
db_enter_msg(proc,4); /* debug only */
  if (p1->aligned) printf(" The sequences are aligned.\n");
  else             printf(" The sequences are NOT aligned.\n");
  fprintf(stdout,"a: %s\n",p1->pastr);
  fprintf(stdout,"b: %s\n",p1->pbstr);
  fprintf(stdout,"\n");
db_leave_msg(proc,4); /* debug only */
  return;
 } /* end dump_chr_alignment */

 /* end file dchral.c */
