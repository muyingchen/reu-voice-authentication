/* file dintal3.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/************************************************************/
/*  void dump_int_alignment3(paln,pcode)                    */
/*  dumps *p1, an ALIGNMENT_INT                             */
/* This style shows external form of symbols & distances.   */
/************************************************************/
 void dump_int_alignment3(ALIGNMENT_INT *paln, pcodeset *pcode)
 {char proc[] = "dump_int_alignment3";
  int d, i, total, err, *perr = &err;
db_enter_msg(proc,4); /* debug only */
  fprintf(stdout,"\n");
/****/
  fprintf(stdout," DUMP OF ALIGNMENT:\n");
  fprintf(stdout," (pcodeset = '%s')\n",pcode->name);
/****/
  if (paln->aligned)
    {fprintf(stdout,"  i  D(a[i],b[i])  Mapping\n");
     total = 0;
     for (i=1; i <= paln->aint[0]; i++)
       {d = wod2(paln->aint[i],paln->bint[i],pcode,perr);
        if (*perr != 0)
          {fprintf(stderr,"*ERR:%s: wod2 returns %d\n",proc,*perr);
           goto RETURN;
          }
        fprintf(stdout," %2d    %5d     (%s => %s)\n",i,d,
         pcode->pc[paln->aint[i]].ac,pcode->pc[paln->bint[i]].ac);
        total += d;
       }
     fprintf(stdout,"      ------\n");
     fprintf(stdout,"      %6d\n",total);
    }
RETURN:
db_leave_msg(proc,4); /* debug only */
  return;
 } /* end dump_int_alignment3 */
 /* end file dintal3.c */
