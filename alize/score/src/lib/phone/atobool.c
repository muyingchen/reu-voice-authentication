 /* file atobool.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 boolean atobool(char *s)
         
 /*********************************************/
 /*  Returns T if string s is "T" or "TRUE"   */
 /* in upper or lower case, F otherwise.      */
 /*********************************************/
 {boolean ignore_case = T, ans;
  if ((string_equal(s,"T",ignore_case)) ||
      (string_equal(s,"YES",ignore_case)) ||
      (string_equal(s,"TRUE",ignore_case))) ans = T;
  else ans = F;
  return ans;
 }
