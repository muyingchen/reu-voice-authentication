/* file trans.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /***********************************************************/
 /*  trans(s2,s1,outmap,inmap)                              */
 /*                                                         */
 /*    Copies character string *s1 to *s2 with substitution */
 /*  or deletion of selected characters.  The arguments     */
 /*  *inmap and *outmap are considered sets of characters.  */
 /*  Any input character found in *inmap is mapped into the */
 /*  character in the corresponding position within *outmap.*/
 /*  When *outmap is short, it is padded to the length of   */
 /*  *inmap by duplicating its last character.              */
 /*  This is a portable C-language version of Unix "tr".    */
 /*                                                         */
 /*  Returns s2, so that it can be called in the form of    */
 /*  an assignment rule or equation:                        */
 /*    s2 = trans(s2,s1,X,Y);                               */
 /***********************************************************/
  char *trans(char *s2, char *s1, char *outmap, char *inmap)
{char *pi, *pj, *pk, key_chr;
   int len_out, len_in;
 /* pad outmap if necessary per Unix tr */
   if ((len_out = strlen(outmap)) < (len_in = strlen(inmap)))
     {key_chr = outmap[len_out-2];
      pj = outmap+len_out-1;
      for (pi = inmap+len_out; *pi != '\0'; pi++) *(pj++) = key_chr;
      *pj = '\0';
     }
 /* do mapping */
   pj = s2;
   for (pi = s1; *pi != '\0'; pi++)
     {if ((pk = strchr(inmap,*pi)) != NULL)
        *(pj++) = *(outmap + (pk - inmap));
      else
        *(pj++) = *pi;
     }
   *pj = '\0';
   return s2;
  }
