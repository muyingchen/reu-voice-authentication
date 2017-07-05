 /* File remzeros.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   void remove_zeros(s)                                             */
 /*   WCHAR_T s[MAX_SYMBS_IN_STR];                                     */
 /*                                                                    */
 /*   Remove zeros (NULL elements) from the integer string s[1]-s[n].  */
 /*   Adjusts s[0], the number of elements in the string.              */
 /**********************************************************************/
 void remove_zeros(WCHAR_T *s)
{
/* local data: */
  char *proc = "remove_zeros";
  int i,j;
/* code: */
 db_enter_msg(proc,3); /* debug only */
  j=1;
  for (i = 1; i <= s[0]; i++) {if (s[i] != 0) {s[j]=s[i]; j++;}}
  s[0] = j - 1;
 db_leave_msg(proc,3); /* debug only */
  } /* end of function "remove_zeros" */
/* end of file remzeros.c */
