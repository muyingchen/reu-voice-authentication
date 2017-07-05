 /* File remsymb.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


 /**********************************************************************/
 /*                                                                    */
 /*   void remove_symbol(s,symb,pcode)                                 */
 /*   WCHAR_T s[MAX_SYMBS_IN_STR];                                     */
 /*                                                                    */
 /*   Remove all elements with pcode external representation <symb>    */
 /* from the integer string s[1]-s[n].                                 */
 /*   Adjusts s[0], the number of elements in the string.              */
 /**********************************************************************/
 void remove_symb(int *s, char *symb, pcodeset *pcode)
{
/* local data: */
  char *proc = "remove_symb";
  int i,j;
/* code: */
 db_enter_msg(proc,2); /* debug only */
if (db_level > 3) {printf("%son entry, s=",pdb); dump_nstr(s);} 
  j=1;
  for (i = 1; i <= s[0]; i++)
    {if ( !streq(symb,pcode->pc[s[i]].ac) )
       {s[j]=s[i]; j++;
    }  }
  s[0] = j - 1;
if (db_level > 3) {printf("%son exit, s=",pdb); dump_nstr(s);} 
 db_leave_msg(proc,2); /* debug only */
  } /* end of function "remove_symb" */
/* end of file remsymb.c */
