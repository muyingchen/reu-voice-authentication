/* file sstok4.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   SUBSTRING sstok4(x); char *x;                                    */
 /*                                                                    */
 /*   Finds the next token in the string *x and returns it as a        */
 /* SUBSTRING. A token is a string of characters bounded by            */
 /*     1. " and "; (may not contain ")                                */
 /*     2. ' and '; (may not contain ')                                */
 /*     3. whitespace and whitespace; (may not contain whitespace)     */
 /*   The start and end of x also may be token boundaries.             */
 /*   If there are nonesuch, returns y->end < y->start (length < 0).   */
 /**********************************************************************/

 SUBSTRING sstok4(char *x)
{char *proc = "sstok4";
  SUBSTRING ssy_data, *ssy = &ssy_data;
 /* code */
  db_enter_msg(proc,1); /* debug only */

if (db_level > 1) printf("%sinput string=[%s]\n",pdb,x);

/* find first possible start of token */
  ssy->start = pltrimf(x);
/* check for null string */
  if (*(ssy->start) == '\0')
    {ssy->end = ssy->start - 2;
     goto RETURN;
    }
/* is it a token bounded by double quotes? */
  if (*(ssy->start) == '\"')
    {ssy->start += 1;
     ssy->end = ssy->start;
     while ((*ssy->end != '\0')&&(*ssy->end != '\"')) ssy->end += 1;
     ssy->end -= 1;
     goto RETURN;
    }
/* is it a token bounded by single quotes? */
  if (*(ssy->start) == '\'')
    {ssy->start += 1;
     ssy->end = ssy->start;
     while ((*ssy->end != '\0')&&(*ssy->end != '\'')) ssy->end += 1;
     ssy->end -= 1;
     goto RETURN;
    }
/* default - find end as last non-whitespace */
  ssy->end = ssy->start;
  while ((*ssy->end != '\0')&&(!isspace(*(ssy->end)))) ssy->end += 1;
  ssy->end -= 1;
RETURN:
  db_leave_msg(proc,1); /* debug only */
  return *ssy;
  } /* end of function "sstok4" */
