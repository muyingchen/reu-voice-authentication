/* file sstok3.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   SUBSTRING sstok3(ssx,delimiters)                                 */
 /*   SUBSTRING *ssx; char *delimiters;                                */
 /*                                                                    */
 /*   Finds the next token in the SUBSTRING *ssx and returns it as a   */
 /* SUBSTRING.  A token is a string of characters bounded on the left  */
 /* and the right by a character in *delimiters.  (NOTE that the start */
 /* and end of the string do NOT count as delimiters, as in sstok(),   */
 /* and the input is a SUBSTRING.)                                     */
 /*   If delimiter aren't found, returns ssy-> start > ssx->end.       */
 /**********************************************************************/

 SUBSTRING sstok3(SUBSTRING *ssx, char *delimiters)
{char *proc = "sstok3";
  SUBSTRING ssy_data, *ssy = &ssy_data;
 /* code */
  db_enter_msg(proc,9); /* debug only */
/* move ssy->start to first delimiter character */
  ssy->start = ssx->start;
  while ((ssy->start <= ssx->end) &&
         (strchr(delimiters,*ssy->start) == NULL)) ssy->start += 1;
  if (ssy->start <= ssx->end)
    {ssy->start += 1; /* token doesn't include delimiters */
/* move ssy->end from 1 beyond there to next delimiter character */
     ssy->end = ssy->start;
     while ((ssy->end <= ssx->end) &&
         (strchr(delimiters,*ssy->end) == NULL)) ssy->end += 1;
     if (ssy->end > ssx->end) ssy->start = ssx->end + 1;
     else ssy->end -= 1; /* token doesn't include delimiters */
    }
  db_leave_msg(proc,9); /* debug only */
  return *ssy;
  } /* end of function "sstok3" */
