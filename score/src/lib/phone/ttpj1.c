/* file ttpj1.c                                                       */
/* function ttpj1(stx,sp,perr) for converting Japanese text to phones */
/* Uses character functions defined in letfcns1.c                     */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  /*********************************************/
  /*  char *ttpj1(stx,sp,perr)                 */
  /*  Converts text to phones                  */
  /*  Input: *stx is a text string;            */
  /*  Output: *sp is a phone string;           */
  /*  Process: elementary.                     */
  /*  Returns *perr > 0 iff error.             */
  /*********************************************/
  char *ttpj1(char *stx, char *sp, int *perr)
{char *proc = "ttpj1";
    char *p,*q;
    char st_data[MAX_REC_LENGTH], *st = &st_data[0];
/* code */
 db_enter_msg(proc,1); /* debug only */
if (db_level > 1) printf("*DB: stx=[%s]\n",stx);
    *perr = 0;
/* pre-process */
    q = st;
    if (*stx != ' ') *q++ = ' ';
    for (p = stx; *p != '\0'; p++)
      {if ( (!ispunct(*p)) || (*p == '\''))  /* ignore punctuation */
        {*q++ = tolower(*p);
      } }
    if (*(p-1) != ' ')  *q++ = ' ';
    *q = '\0';
/* convert letter to phone */
    q = sp;
    for (p = stx; *p != '\0'; p++)
      {*q++ = ' ';       
       if (isvowel2(*p))
         {*q++ = *p;
          if ((*(p+1) == ':') ||
              ((*p == 'e')&&(*(p+1) == 'i')) ||
              ((*p == 'i')&&(*(p+1) == 'i')) )
            {*q++ = ':'; p++;
         }  }
       if (iscons2(*p))
         {if ((*p == 'g') && (!bound(*(p-1)))) {*q++ = 'n'; continue;}
          if ((*p == 'n') && (*(p+1) == '\''))
            {*q++ = 'e'; *q++ = 'n'; p++; continue;}
          if (*p == 'j') {*q++ = 'j'; *q++ = 'h'; continue;}
          if (*p == 'h') {*q++ = 'h'; *q++ = 'h'; continue;}
          if ((*p == 'c') && (*(p+1) == 'h'))
            {*q++ = 'c'; *q++ = 'h'; p++; continue;}
          if ((*p == 's') && (*(p+1) == 'h'))
            {*q++ = 's'; *q++ = 'h'; p++; continue;}
          if ((*p == 't') && (*(p+1) == 's'))
            {*q++ = 't'; *q++ = 's'; p++; continue;}
          *q++ = *p;
         }
      }
    *q = '\0';
 db_leave_msg(proc,1); /* debug only */
    return sp;
   }
/* end file ttpj1.c */
