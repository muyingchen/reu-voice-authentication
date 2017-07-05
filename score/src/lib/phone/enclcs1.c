/*  File enclcs1.c  */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  /************************************************************/
  /*                                                          */
  /*  encode_lcs(p1,perr)                                     */
  /*                                                          */
  /*  Encodes the lower code representation of each element   */
  /*  of pcode *p1.                                           */
  /*  *Perr indicates success:                                */
  /*           0 iff a.o.k.                                   */
  /*          11 iff encode2 returns an error.                */
  /*   (Fatal error if memory allocation fails.)              */
  /* (Uses global parameter LINE_LENGTH.)                     */
  /*                                                          */
  /************************************************************/
  void encode_lcs(pcodeset *p1, int *perr)
{
/* data */
 int aint[MAX_SYMBS_IN_STR];
 int n, *pn = &n;
 int i;
int dbi = 0, dbj = 0;
 char *proc = "encode_lcs";
/* coding  */
 db_enter_msg(proc,0); /* debug only */
if (db_level > 0) printf("%spcode file:'%s'\n",pdb,p1->name);
if (db_level > 0) printf("%spctab_length=%d\n",pdb,p1->pctab_length);
if (db_level > 0) printf("%shighest_code_value=%d\n",pdb,
   p1->highest_code_value);
if (db_level > 2) dump_pcode3(p1);
 *perr = 0;
 if (p1->compositional)
   {for (i=0; ((i < p1->pctab_length)&&(i <= p1->highest_code_value)); i++)
      {if (strlen(p1->pc[i].ac) > 0)
         {if (p1->pc[i].lc_ext == NULL)
            {fprintf(stderr,"*ERR: %s: a lower-code",proc);
             fprintf(stderr," representation is null.\n");
             fprintf(stderr," pcode name='%s', entry='%s'\n",
                p1->name, p1->pc[i].ac);
             *perr = 11; goto RETURN;
            }
          encode2(p1->pc[i].lc_ext,p1->lower_code,aint,pn,perr);

if (db_level > 0)
 {if((++dbi % 1000) == 1) printf("%sencoded %d k\n",pdb,++dbj);
 }
          if (*perr > 0)
            {fprintf(stderr,"*ERR: %s: a lower-code",proc);
             fprintf(stderr," representation cannot be encoded.\n");
             fprintf(stderr," encode2 returns *perr=%d\n",*perr);
             fprintf(stderr," pcode name='%s', entry='%s' = /%s/\n",
                p1->name, p1->pc[i].ac,p1->pc[i].lc_ext);
             *perr = 11; goto RETURN;
            }
          aint[0] = *pn;
          p1->pc[i].lc_int = nstrdup_safe(aint,proc);
   }  }  }
RETURN:
 db_leave_msg(proc,0); /* debug only */
  return;
  } /* end encode_lcs */
