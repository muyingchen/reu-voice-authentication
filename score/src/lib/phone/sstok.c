/* file sstok.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   SUBSTRING sstok(ssx,delimiters);                                 */
 /*   SUBSTRING *ssx; char *delimiters;                                */
 /*                                                                    */
 /*   Finds the next token in the substring *ssx and returns it as a   */
 /* SUBSTRING.  A token is a string of characters not in *delimiters.  */
 /* (Beginning and ending of ssx also count as delimiters.)            */
 /*   If there are nonesuch, returns y->start > x->end;                */
 /**********************************************************************/

 SUBSTRING sstok(SUBSTRING *ssx, char *delimiters)
{char *proc = "sstok";
  SUBSTRING ssy_data, *ssy = &ssy_data;
 /* code */
  db_enter_msg(proc,9); /* debug only */
  ssy->start = ssx->start;
  ssy->end = ssy->start - 1; /* no-find condition */
/* move ssy->start through left-most delimiter characters */
  while ((strchr(delimiters,*ssy->start) != NULL)&&
         (ssy->start <= ssx->end))   ssy->start += 1;
  if (ssy->start > ssx->end) ssy->end = ssy->start - 1;
  else
/* more ssy->end from there through non-delimiter characters */
    {ssy->end = ssy->start;
     while ((strchr(delimiters,*ssy->end) == NULL)&&
            (ssy->end <= ssx->end))   ssy->end += 1;
     ssy->end -= 1;
    }
  db_leave_msg(proc,9); /* debug only */
  return *ssy;
  } /* end of function "sstok" */
