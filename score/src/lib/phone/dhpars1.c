/* file dhpars1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

void dump_hash_pars(HASH_PARAMETERS *hpars)
{/* dumps parameter values and function name */
  int i; int n;
  fprintf(stdout,"   Hash function parameters:\n");
  fprintf(stdout,"   hfcn_name = '%s'\n",hpars->hfcn_name);
  fprintf(stdout,"   recommended_load_factor = %f\n",
     hpars->recommended_load_factor);
  fprintf(stdout,"   no. of parameters = %d\n",hpars->nparams);
  n = 0;
  fprintf(stdout,"   ");
  for (i = 0; i < hpars->nparams; i++)
    {fprintf(stdout," C[%d] = %ld;",i,hpars->C[i]);
     if ((++n % 4) == 0) fprintf(stdout,"\n   ");
    }
  if (n > 0) fprintf(stdout,"\n");
 return;
}
