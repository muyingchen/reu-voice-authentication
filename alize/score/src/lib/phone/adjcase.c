 /* file adjcase.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /*********************************************/
 /*  adjust_type_case(s,pcode)                */
 /*  Changes the type case of *s to agree     */
 /* with the type case of pcodeset *pcode.    */
 /* Returns pointer to *s.                    */
 /*********************************************/
  char *adjust_type_case(char *s, pcodeset *pcode)
{if (streq(pcode->type_case,"upper")) s = make_upper(s);
   if (streq(pcode->type_case,"lower")) s = make_lower(s);
   return s;
  }
