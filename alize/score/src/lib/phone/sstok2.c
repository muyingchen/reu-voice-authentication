/* file sstok2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   SUBSTRING sstok2(sx,delimiters);                                 */
 /*   char *sx, *delimiters;                                           */
 /*                                                                    */
 /*   Finds the next token in the char string *sx and returns it as a  */
 /* SUBSTRING.  A token is a string of characters bounded on the left  */
 /* and the right by a character in *delimiters.  (NOTE that the start */
 /* and end of the string do NOT count as delimiters, as in sstok(),   */
 /* and the input is a char string, not a SUBSTRING.)                  */
 /*   If delimiters aren't found, returns a substring of length < 0    */
 /**********************************************************************/

 SUBSTRING sstok2(char *sx, char *delimiters)
{char *proc = "sstok2";
  SUBSTRING ssy_data, *ssy = &ssy_data;
 /* code */
  db_enter_msg(proc,9); /* debug only */
/* move ssy->start to first delimiter character */
  ssy->start = sx;
  while ((strchr(delimiters,*ssy->start) == NULL)&&
         (*ssy->start != '\0'))   ssy->start += 1;
  if (*ssy->start == '\0') ssy->end = ssy->start - 2;
  else
    {ssy->start += 1; /* token doesn't include delimiters */
/* more ssy->end from 1 beyond there to next delimiter characters */
     ssy->end = ssy->start;
     while ((strchr(delimiters,*ssy->end) == NULL)&&
            (ssy->end != NULL))   ssy->end += 1;
     if (ssy->end == NULL) ssy->end = ssy->start - 2;
     else                  ssy->end -= 1;
    }
  db_leave_msg(proc,9); /* debug only */
  return *ssy;
  } /* end of function "sstok2" */
