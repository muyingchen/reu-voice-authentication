/* file drules1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/*****************************************************************/
/*                                                               */
/*  dump_rules1(rset)                                             */
/*  dumps a ruleset to stdout.                                   */
/*                                                               */
/*****************************************************************/
void dump_rules1(RULESET1 *rset)
{
  int i;
  fprintf(stdout,"\n");
  fprintf(stdout," RULESET1 DUMP\n\n");
  fprintf(stdout,"  name = '%s'\n",rset->name);
  fprintf(stdout,"  directory = '%s'\n",rset->directory);
  fprintf(stdout,"  desc = '%s'\n",rset->desc);
  fprintf(stdout,"  format = '%s'\n",rset->format);
  fprintf(stdout,"  copy_no_hit = '%s'\n",bool_print(rset->copy_no_hit));
  fprintf(stdout,"  max_nrules = %ld\n",rset->max_nrules);
  fprintf(stdout,"  nrules = %d\n",rset->nrules);
  fprintf(stdout,"  rtab table length = %ld\n",rset->rtab_length);
  if (rset->nrules > 0 )
    {fprintf(stdout,"\n");
     fprintf(stdout,"     l.h.s.(length)    r.h.s.(length)\n");
     for (i = 1; i <= rset->nrules; i++)
       {fprintf(stdout," %s (%d) => %s (%d)\n",
          rset->rule[i].sin,
          rset->rule[i].sinl,
          rset->rule[i].sout,
          rset->rule[i].soutl);
    }  }
  fprintf(stdout,"\n");
  return;
} /* end dump_rules */
