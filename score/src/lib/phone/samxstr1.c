/* file samxstr1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


 /********************************************************/
 /*  same_except_stress(i,j,pcode)                       */
 /* Returns judgement of whether phones i and j in the   */
 /* pcodeset *pcode are the same except for stress.      */
 /********************************************************/
 boolean same_except_stress(int i, int j, pcodeset *pcode_ph)
{char *proc = "same_except_stress";
  boolean diff,found;
  int nifeats,njfeats,ix,jx,ixfeat,jxfeat;
  pcodeset *pcode_feat;
 db_enter_msg(proc,9); /* debug only */
  pcode_feat = pcode_ph->lower_code;
  nifeats = pcode_ph->pc[i].lc_int[0];
  njfeats = pcode_ph->pc[j].lc_int[0];

if (db_level > 9)
  {printf("%s i='%s',ifeats='%s'\n",pdb,pcode_ph->pc[i].ac,pcode_ph->pc[i].lc_ext);
   printf("%s j='%s',jfeats='%s'\n",pdb,pcode_ph->pc[j].ac,pcode_ph->pc[j].lc_ext);
  }

  diff = F;
/* is each non-stress feature in i matched by a non-stress feature in j? */
  for (ix = 1; (!diff)&&(ix <= nifeats); ix++)
    {ixfeat = pcode_ph->pc[i].lc_int[ix];
     if (strstr(pcode_feat->pc[ixfeat].ac,"stress") == 0)
       {found = F;
        for (jx = 1; (!found)&&(jx<=njfeats); jx++)
          {jxfeat = pcode_ph->pc[j].lc_int[jx];
           if (jxfeat == ixfeat) found = T;
          }
        if (!found) diff = T;

if (db_level > 9)
{if (!found)
  printf("%s non-stress i feature not found in  j: '%s'\n",
    pdb,pcode_feat->pc[ixfeat].ac);
}

    }  }
  if (!diff)
/* is each non-stress feature in j matched by a non-stress feature in i? */
    {for (jx = 1; (!diff)&&(jx <= njfeats); jx++)
       {jxfeat = pcode_ph->pc[j].lc_int[jx];
        if (strstr(pcode_feat->pc[jxfeat].ac,"stress") == 0)
          {found = F;
           for (ix = 1; (!found)&&(ix<=nifeats); ix++)
             {ixfeat = pcode_ph->pc[i].lc_int[ix];
              if (ixfeat == jxfeat) found = T;
             }
           if (!found) diff = T;

if (db_level > 9)
{if (!found)
  printf("%s non-stress j feature not found in  i: '%s'\n",
    pdb,pcode_feat->pc[jxfeat].ac);
}

    }  }  }

if (db_level > 9) printf("%s at return, diff=%s\n",pdb,bool_print(diff));

 db_leave_msg(proc,9); /* debug only */
 if (diff) return F;
 else      return T;
 }
/* end samxstr1.c */
