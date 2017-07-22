/* file boolpr1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   char *bool_print(x)                                              */
 /*                                                                    */
 /* Returns the printing equivalent of the boolean *x.                 */
 /*                                                                    */
 /**********************************************************************/

 char *bool_print(boolean x) {if (x) return "T"; else return "F";}
