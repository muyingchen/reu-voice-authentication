/* file letfcns.c                                               */
/* functions predicates) defined over characters.               */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


   boolean isvowel1(char c)
{if (strchr("aeiouy",c)) return T; else return F;}

   boolean isvowel2(char c)
{if (strchr("aeiou",c)) return T; else return F;}

   boolean iscons1(char c) {return ((!isvowel1(c))&&(!bound(c)));}

   boolean iscons2(char c)         /* consonant letter per Japanese romanization */
   {if (strchr("bcdfghjkmnprstwyz",c)) return T; else return F; }

   boolean bound(char c)
{if ((c == '-')||(!isalnum(c))) return T; else return F;}

   boolean is_front_vowel(char c)
{if ((c=='e')||(c=='i')||(c=='y')) return T; else return F;}

   boolean velar(char c)
{if ((c=='k')||(c=='g')||(c=='q')) return T; else return F;}

   boolean coronal(char c)
{if ((c=='t')||(c=='d')) return T; else return F;}

   boolean fric1(char *p)         
/* frics that take schwa in plural & possessive in ENG (e.g. boss's)  */
   {if ((*p == 's')||(*p == 'x')) return T; /* boss,box */
    if ((*p == 'c')&&(is_front_vowel(*(p+1)))) return T; /* faces */
    if ((*p == 'g')&&
        (is_front_vowel(*(p+1)))&&
        (bound(*(p+2)))) return T; /* face, faces, rage */
    if (*p == 'h')
      {if (*(p-1) == 's') return T; /* bash */
       if (((*(p-1) == 'c'))&&(*(p-2)=='t')) return T; /* batch */
     }
    return F;
   }

   boolean voiced(char *p)         
/* letters standing for voiced sounds */
   {char *vcons = "bdgjlmnrvwz";
    if (isvowel1(*p)||(strchr(vcons,*p) != NULL)) return T; else return F;}

   boolean short_env(char *p)         
/* "short vowel" environment for ENG */
   {if (!isvowel1(*(p+1)))
      {if ((*(p+2) == 'r') && (*(p+3) == 'e') && bound(*(p+4)) ) /* sabre */
         return F;
       if (*(p+1) == 'x') return T;
       if (!isvowel1(*(p+2))) return T;
      }
    return F;
   }
/* end letfcns.c */
