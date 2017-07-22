/* file aprules1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/*****************************************************************/
/*                                                               */
/*  char *apply_rules1(pb,pa,rset,perr)                          */
/*  Applies RULESET1 *rset to string pa, producing string pb.    */
/*  Returns pb.                                                  */
/*  If rset == NULL, just copies over the input string.          */
/*                                                               */
/*****************************************************************/
char *apply_rules1(char *pb, char *pa, RULESET1 *rset, int *perr)
{char *proc = "apply_rules";
  char *pi, *pbx;
  int irule;
  boolean hit;
/* code: */
 db_enter_msg(proc,0); /* debug only */
  *perr = 0;
if (db_level > 0) printf("%s Input: '%s'\n",pdb,pa);
  if (rset == NULL)
    {pb = strcpy(pb,pa);
     goto RETURN;
    }
  pi = pa;
  while (*pi != '\0')
    {hit = F;
     for (irule = 1; (!hit) && (irule <= rset->nrules); irule++)
       {if (streq(rset->format,"NIST1"))
          {if (strncmp(pi,rset->rule[irule].sin,rset->rule[irule].sinl)==0)
             {hit = T;
              pb = strcat(pb,rset->rule[irule].sout);
              pi += rset->rule[irule].sinl;
       }  }  }
     if (!hit)
       {if (rset->copy_no_hit)
          {pbx = pb + strlen(pb);
           *pbx = *pi; /* copy char from pi to end of pb */
           *(++pbx) = '\0';
          }
        pi += 1;
    }  }
 RETURN:
if (db_level > 0) printf("%s Output:'%s'\n",pdb,pb);
 db_leave_msg(proc,0); /* debug only */
  return pb;
} /* end apply_rules1 */
