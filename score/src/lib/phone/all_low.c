/* file all_low.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 boolean all_lower(char *sx)          
/**************************************************************************/
/* Determines if the string *sx is all lower-case.                        */
/**************************************************************************/
 {char *proc = "all_lower";
/* code */
  char *sy; boolean upper_case_found;
 db_enter_msg(proc,2); /* debug only */
  upper_case_found = F;
  for (sy = sx; ((*sy != '\0')&&(!upper_case_found)); sy++)
    if (isupper((int)*sy)) upper_case_found = T;
 db_leave_msg(proc,2); /* debug only */
  return (!upper_case_found);
 }
