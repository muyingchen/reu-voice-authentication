/* file normpr1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /************************************************/
 /*  double normprob(z)                          */
 /*  Returns an approximation to the cumulative  */
 /* standard normal probability of z. The value  */
 /* is found by table look-up;  when z lies      */
 /* between two table values, the lower one is   */
 /* returned, as a conservative approximation.   */
 /* If z is higher than any value in the table,  */
 /* the highest tabulated value is returned.     */
 /*                                              */
 /************************************************/
  double normprob(double z)
{int i, ix; double zx, p; boolean found;

#include <phone/normpr1.h>

    if (z < 0.0) zx = -z; else zx = z;
    found = F;
    for (i=1; (i < n_normp)&&(!found); i++)
      {if (zx < znorm[i]) {ix = i - 1; found = T;}
       if (zx == znorm[i]){ix = i;     found = T;}
      }
    if (!found) p = Fnorm[n_normp-1];
    else p = Fnorm[ix];
    return p;
   }
