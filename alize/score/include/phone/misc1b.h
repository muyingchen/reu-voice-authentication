/*	misc1b.h	89/09/28
 *
 * Miscellaneous external function prototypes
 * Required for BCD 4.2 only.
 */

#ifndef MISC1B_HEADER
#define MISC1B_HEADER

/* acc. to K&R, 2nd ed., these should be in <stdlib.h>: */
double atof();
int    atoi();
long   atol();
double strtod();
long   strtol();
unsigned long strtoul();
int rand();
/* note - in standard C, malloc is "void *"; cf. K&R 2nd Ed. p. 252,
   Turbo C Ref. Guide Ver. 2.0 p. 235; but BCD lint wants it like this:
*/
char *malloc();
char *calloc();
/* this should be in <string.h>: */
char *strtok();
/* this should be in <stdio.h> : */
char   *gets();

#endif
/* end of misc1b.h */
