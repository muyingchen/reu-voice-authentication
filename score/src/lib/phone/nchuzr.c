/* file nchuzr.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /***********************************************************************/
 /*                                                                     */
 /*   long int n_choose_r(n,r)                                          */
 /*                                                                     */
 /* Computes the number of ways that r urns can be filled with n balls, */
 /* sampling without replacement; a.k.a. "n choose r", n!/(n-r)!.       */
 /*                                                                     */
 /***********************************************************************/

 long int n_choose_r(int n, int r)
{long int i,x;
   if ((r > n)||(n==0)||(r==0))
     x = 0;
   else
     {x = 1; for (i=n; i>n-r;i--) x *= i;
     }
   return x;
  }
