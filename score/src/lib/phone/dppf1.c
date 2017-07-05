/* file dppf1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/****************************************************************/
/*                                                              */
/* dump_ppf1(pcode)                                             */
/* Dumps the ppf function in the pcodeset *pcode.               */
/* This version dumps only a sample of the non-default values.  */
/*                                                              */
/****************************************************************/
void dump_ppf1(pcodeset *pcode)
{long int ndef, nndef; int i, j, k;
  int ki[241],kj[241], klim = 12;
  ppfcn *ppf1;
  fprintf(stdout," Here is some data from the ppf table:\n");
  ppf1 = pcode->ppf;
  if (ppf1 == NULL)
    fprintf(stdout," pcode->ppf is NULL.\n");
  else
    {fprintf(stdout,"   pcodeset name:'%s'\n",ppf1->pcode->name);
     fprintf(stdout,"   pcodeset ncodes = %d\n",ppf1->pcode->ncodes);
     fprintf(stdout,"   default_value = %d\n",ppf1->default_value);
     fprintf(stdout,"   symmetric = %s\n",bool_print(ppf1->symmetric));
     fprintf(stdout,"   diagonal_zeros = %s\n",
        bool_print(ppf1->diagonal_zeros));
     ndef = 0; nndef = 0; k = 0;
     for (i = 0; i <= pcode->ncodes; i++)
       {for (j = 0; j <= pcode->ncodes; j++)
          {if (ppf1->fval[i][j] == ppf1->default_value)
             ndef += 1;
           else
             {nndef += 1;
              if (k < klim) {++k; ki[k] = i; kj[k] = j;}
       }  }  }
     fprintf(stdout," Concerning the array fval[][]:\n");
     fprintf(stdout," No. of   default values   = %d\n",ndef);
     fprintf(stdout," No. of non-default values = %d\n",nndef);
     fprintf(stdout," Here are some non-default entries:\n");
     fprintf(stdout,"\n    i phone     j phone   fval[i,j]\n");
     if (nndef < klim) klim = nndef;
     for (k=1;k<=klim;k++)
       {fprintf(stdout,"  %8s    %8s       %d\n",pcode->pc[ki[k]].ac,
            pcode->pc[kj[k]].ac, ppf1->fval[ki[k]][kj[k]]);
       }
    }
 }
/* end dump_ppf1(pcode) */
