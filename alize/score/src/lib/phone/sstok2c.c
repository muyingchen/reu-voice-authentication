/* file sstok2c.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   SUBSTRING sstok2c(sx,delimiters);                                */
 /*   char *sx, *delimiters;                                           */
 /*                                                                    */
 /*   Finds the next token in the char string *sx and returns it as a  */
 /* SUBSTRING.  A token is a string of characters bounded on the left  */
 /* and the right by a character in *delimiters.  (NOTE that the input */
 /* is a char string, not a SUBSTRING.)                                */
 /* This "b" version counts a string of delimiters as the same as just */
 /* one delimiter, whereas the original version will stop and set the  */
 /* boundary when just the first delimiter character is found.  It is  */
 /* necessary to use the first, original, version, to define tokens    */
 /* like "[", where all 3 of the characters can be delimiters.         */
 /* ALSO, this version differs from sstok2b() in that the start of the */
 /* string as well as the final end-of-string character '/0' acts      */
 /* like a delimiter.                                                  */
 /*   If delimiter aren't found, returns a substring of length < 0     */
 /**********************************************************************/

 SUBSTRING sstok2c(char *sx, char *delimiters)
{char *proc = "sstok2c";
  SUBSTRING ssy_data, *ssy = &ssy_data;
 /* code */
  db_enter_msg(proc,9); /* debug only */
/* move ssy->start thru next string of delimiter characters */
/*   move to next delimiter character */
  ssy->start = sx;
/*   move thru initial contiguous delimiter characters, if any */
  while ((strchr(delimiters,*ssy->start) != NULL)&&
         (*ssy->start != '\0'))   ssy->start += 1;
  if (*ssy->start == '\0') {ssy->end = ssy->start - 2; goto RETURN;}
/* more ssy->end from there to next delimiter characters */
  ssy->end = ssy->start;
  while ((strchr(delimiters,*ssy->end) == NULL)&&
         (*ssy->end != '\0'))   ssy->end += 1;
  ssy->end -= 1;
RETURN:
  db_leave_msg(proc,9); /* debug only */
  return *ssy;
  } /* end of function "sstok2c" */
