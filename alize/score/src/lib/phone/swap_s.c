/* file swap_s.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/*******************************************/
/*   short swap_short(s)                   */
/*   short s;                              */
/* Function to swap bytes in a short int.  */
/*******************************************/

short int swap_short(short int s)
 {char *p = (char *) &s, temp;
  temp = *p;
  *p = *(p + 1);
  *(p + 1) = temp;
  return s;
 }
