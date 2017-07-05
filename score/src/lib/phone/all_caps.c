/* file all_caps.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 boolean all_caps(char *sx)          
/**************************************************************************/
/* Determines if the string *sx is all caps.                              */
/**************************************************************************/
 {char *proc = "all_caps";
/* code */
  char *sy; boolean lower_case_found;
 db_enter_msg(proc,2); /* debug only */
  lower_case_found = F;
  for (sy = sx; ((*sy != '\0')&&(!lower_case_found)); sy++)
    if (islower((int)*sy)) lower_case_found = T;
 db_leave_msg(proc,2); /* debug only */
  return (!lower_case_found);
 }
