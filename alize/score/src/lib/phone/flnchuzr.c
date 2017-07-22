/* file flnchuzr.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /***********************************************************************/
 /*                                                                     */
 /*   float float_n_choose_r(n,r)                                       */
 /*                                                                     */
 /* Computes the number of ways that r urns can be filled with n balls, */
 /* sampling without replacement; a.k.a. "n choose r", n!/(n-r)!.       */
 /* This version uses floating point arithmetic for max range.          */
 /*                                                                     */
 /***********************************************************************/

 float float_n_choose_r(int n, int r)
{long int i; float x;
   if ((r > n)||(n==0)||(r==0))
     x = 0.0;
   else
     {x = 1.0; for (i=n; i>n-r;i--) x *= (float)i;
     }
   return x;
  }
