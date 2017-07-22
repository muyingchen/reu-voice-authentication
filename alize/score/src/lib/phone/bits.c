 /* file bits.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


 /**********************************************************************/
 /*                                                                    */
 /*   char *bits(x,nbits,sx)                                           */
 /*   long int x; int nbits; char *sx                                  */
 /*                                                                    */
 /*   Puts bit-wise representation of x, up to <nbits> bits, into *sx. */
 /*   Returns *sx.                                                     */
 /**********************************************************************/

 char *bits(long int x, int nbits, char *sx)
{int i;
   if ((nbits < 1)||(nbits > 8*sizeof(x))) sx = strcpy(sx,"********");
   else
     {sx[nbits] = '\0';
      for (i=0; i <= nbits-1; i++)
        {if ((x & (1 << i)) != 0) sx[nbits-1-i] = '1';
         else                     sx[nbits-1-i] = '0';
     }  }
   return sx;
  } /* end of function "bits" */
