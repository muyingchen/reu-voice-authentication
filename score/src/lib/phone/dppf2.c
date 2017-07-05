/* file dppf2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/****************************************************************/
/*                                                              */
/* dump_ppf2(pcode)                                             */
/* Dumps the ppf function in the pcodeset *pcode.               */
/* This version dumps all the table values.                     */
/*                                                              */
/****************************************************************/

void dump_ppf2(pcodeset *pcode)
{long int ndef, nndef; int i, j, k;
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
     fprintf(stdout," Concerning the array fval[][]:\n");
     fprintf(stdout," Here are the entries:\n");
     fprintf(stdout,"\n    i phone     j phone   fval[i,j]\n");
     for (i = 0; i <= pcode->ncodes; i++)
       {for (j = 0; j <= pcode->ncodes; j++)
          {fprintf(stdout,"%3d %8s %3d %8s %4d\n",
            i,pcode->pc[i].ac,j,pcode->pc[j].ac,ppf1->fval[i][j]);
           if (ppf1->fval[i][j] == ppf1->default_value)
             ndef += 1;
           else
             {nndef += 1;
       }  }  }
     fprintf(stdout," No. of   default values   = %d\n",ndef);
     fprintf(stdout," No. of non-default values = %d\n",nndef);
    }
 }

/* end dump_ppf2(pcode) */
