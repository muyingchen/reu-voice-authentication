/*  file errcnt1.c  */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/****************************************************/
/* contains functions for counting errors, based on */
/* an integer alignment.                            */
/****************************************************/

int number_of_insertions(ALIGNMENT_INT *pn1)
{int n, i;
 n=0; for (i=1; i<=pn1->aint[0]; i++) {if (pn1->aint[i] == 0) n += 1;}
 return n;
}

int number_of_deletions(ALIGNMENT_INT *pn1)
{int n, i;
 n=0; for (i=1; i<=pn1->bint[0]; i++) {if (pn1->bint[i] == 0) n += 1;}
 return n;
}

int number_of_substitutions(ALIGNMENT_INT *pn1)
{int n, i;
 n=0;
 for (i=1; i<=pn1->aint[0]; i++)
 {if ((pn1->aint[i] != 0) &&
      (pn1->bint[i] != 0) &&
      (pn1->aint[i] != pn1->bint[i]))
     n += 1;}
 return n;
}

/*  end file errcnt1.c  */
