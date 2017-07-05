/* file ttp1.c                                              */
/* function ttp1(stx,sp,perr) for converting text to phones */
/* and its sub-functions.                                   */
/* Uses character functions defined in letfcns1.c           */
/* Marks NO stress.                                         */
/* Output phones are from standard phone set "phon1ax.pcd"  */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif



  /*********************************************/
  /*  char *ttp1(stx,sp,perr)                 */
  /*  Converts text to phones                  */
  /*  Input: *stx is a text string;            */
  /*  Output: *sp is a phone string;           */
  /*  Process: elementary.                     */
  /*  Returns *perr > 0 iff error.             */
  /*********************************************/
  char *ttp1(char *stx, char *sp, int *perr)
{char *proc = "ttp1";
    char *p,*q;
    char st_data[MAX_REC_LENGTH], *st = &st_data[0];
/* code */
 db_enter_msg(proc,1); /* debug only */
    *perr = 0;
    *sp = '\0'; /* empty output string */
/* pre-process */
    q = st;
    if (*stx != ' ') *q++ = ' ';
    for (p = stx; *p != '\0'; p++)
      {if (p > stx)
         {if (isalpha(*(p-1)) && isalpha(*(p+1)) && (*p == '+'))
            {*q++ = '\'';  /* "kiska+s" --> kiska's */
             continue;
	 }  }
       if ( (!ispunct(*p)) || (*p == '\'')) *q++ = tolower(*p);
       else if (ispunct(*p)) *q++ = ' ';
      }
    if (*(p-1) != ' ')  *q++ = ' ';
    *q = '\0';
if (db_level > 1) printf("*DB: st=[%s]\n",st);
/* convert letter to phone */
    for (p = st; *p != '\0'; p++)
      {
/* digits */
/* NOTE: these can't be treated lexically because they may be */
/* embedded in alphanumeric strings, like "m1".               */
       if (isdigit(*p))
         {switch (*p)
            {case '0': sp = strcat(sp," z iy r ow");      break;
             case '1': sp = strcat(sp," w ax n");         break;
             case '2': sp = strcat(sp," t uw");           break;
             case '3': sp = strcat(sp," th r iy");        break;
             case '4': sp = strcat(sp," f ow r");         break;
             case '5': sp = strcat(sp," f ay v");         break;
             case '6': sp = strcat(sp," s ih k s");       break;
             case '7': sp = strcat(sp," s eh v ax n"); break;
             case '8': sp = strcat(sp," ey t");           break;
             case '9': sp = strcat(sp," n ay n");         break;
            }
          continue;
         }
/* separate letters */
       if (bound(*(p-1)) && bound(*(p+1)))
         {switch (*p)
            {case 'a': sp = strcat(sp," ey");                     break;
             case 'b': sp = strcat(sp," b iy");                   break;
             case 'c': sp = strcat(sp," s iy");                   break;
             case 'd': sp = strcat(sp," d iy");                   break;
             case 'e': sp = strcat(sp," iy");                     break;
             case 'f': sp = strcat(sp," eh f");                   break;
             case 'g': sp = strcat(sp," jh iy");                  break;
             case 'h': sp = strcat(sp," ey ch");                  break;
             case 'i': sp = strcat(sp," ay");                     break;
             case 'j': sp = strcat(sp," jh ey");                  break;
             case 'k': sp = strcat(sp," k ey");                   break;
             case 'l': sp = strcat(sp," eh l");                   break;
             case 'm': sp = strcat(sp," eh m");                   break;
             case 'n': sp = strcat(sp," eh n");                   break;
             case 'o': sp = strcat(sp," ow");                     break;
             case 'p': sp = strcat(sp," p iy");                   break;
             case 'q': sp = strcat(sp," k y uw");                 break;
             case 'r': sp = strcat(sp," aa r");                   break;
             case 's': sp = strcat(sp," eh s");                   break;
             case 't': sp = strcat(sp," t iy");                   break;
             case 'u': sp = strcat(sp," y uw");                   break;
             case 'v': sp = strcat(sp," v iy");                   break;
             case 'w': sp = strcat(sp," d ax b ax l y uw"); break;
             case 'x': sp = strcat(sp," eh k s");                 break;
             case 'y': sp = strcat(sp," w ay");                   break;
             case 'z': sp = strcat(sp," z iy");                   break;
            }
          continue;
	 }
/* geminate consonants */
       if (iscons1(*p)&&(*p == *(p+1))) continue;  /* geminate cons */
/* "'" */
       if (*p == '\'') /* apostrophe */
         {if (bound(*(p+1))) continue; /* warriors' */
          if (bound(*(p-1))) continue; /* 'taters   */
          if (*(p+1) == 's')
            {if ((fric1(p-1))||((*(p-1) == 'e')&& fric1(p-2)))
               {sp=strcat(sp," ax z"); p++; continue;} /* boss's, face's, rage's" */
              if (voiced(p-1))
               {sp=strcat(sp," z"); p++; continue;} /* Joe's,bag's */
              else
               {sp=strcat(sp," s"); p++; continue;} /* bat's */
            }
          if ((*(p+1) == 'l')&&(*(p+2) == 'l')&&bound(*(p+3)))
	    {sp = strcat(sp," el"); p++; p++; continue;} /* john'll, they'll */                   
          continue;
         }
/* "a" */
       if (*p == 'a')
         {if (bound(*(p+1))) {sp = strcat(sp," ax");      continue;}/* a */
          if (*(p+1) == 'i') {sp = strcat(sp," ey"); p++; continue;}
          if (*(p+1) == 'y') {sp = strcat(sp," ey"); p++; continue;}
          if (*(p+1) == 'u') {sp = strcat(sp," ao"); p++; continue;}/*haul*/
          if (*(p+1) == 'w') {sp = strcat(sp," ao"); p++; continue;}/*bawl*/
          if ((*(p+1) == 'l')&&(*(p+2) == 'l'))
            {sp = strcat(sp," ao"); /* all, ball, call, fall */
             continue;
            }
          if (strncmp(p+1,"ste ",4) == 0)  /* paste (long env?) */
            {sp = strcat(sp," ey"); continue;}
          if (short_env(p)) sp = strcat(sp," ae");
          else              sp = strcat(sp," ey");
         }
/* "b" */
       if (*p == 'b') sp = strcat(sp," b");
/* "c" */
       if (*p == 'c')
         {if (*(p+1) == 'h') {sp = strcat(sp," ch"); p++; continue;}
          if (is_front_vowel(*(p+1))) sp = strcat(sp," s");
          else                        sp = strcat(sp," k");
          if (*(p+1) == 'k') {p++; continue;}
         }
/* "d" */
       if (*p == 'd') sp = strcat(sp," d");
/* "e" */
       if (*p == 'e')
         {if  (*(p+1) == 'w') {sp = strcat(sp," y uw"); p++; continue;}/*few*/
          if ((*(p+1) == 'l') && (*(p+2) == ' ')) /* final "el" */
	      {sp = strcat(sp," el"); p++; continue;} /* nickel */
          if ((*(p+1) == 'y')&&(iscons1(*(p-1)))&&(bound(*(p-2))))
            {sp = strcat(sp," ey"); p++; continue;} /* hey */
          if ((*(p+1) == 'y')&&(iscons1(*(p-1)))&&(iscons1(*(p-2)))&&(bound(*(p-3))))
            {sp = strcat(sp," ey"); p++; continue;} /* they */
          if ((*(p+1) == 'y')&&(*(p+2) == ' ')) /* final "ey" */
            {sp = strcat(sp," iy"); p++; continue;
            }
          if (bound(*(p+1))) /* final "e" */
            {if (iscons1(*(p-1)))
               {if (bound(*(p-2))) {sp = strcat(sp," iy"); continue;}
                if ((iscons1(*(p-2))) &&(bound(*(p-3))) )
                    {sp=strcat(sp," iy"); continue;}  /* me, the */
               } 
             if (bound(*(p-1))) {sp = strcat(sp," iy"); continue;}
             continue;
            }

          if ((*(p+1) == 'd')&&(bound(*(p+2)))) /* final "ed" */
            {if ((*(p-1) == 't')||(*(p-1) == 'd'))
               {sp=strcat(sp," ax d"); p++; continue;} /* batted */
             if ((*(p-1) == 'r')&&(*(p-2) == 'd'))
               {sp=strcat(sp," ax d"); p++; continue;} /* hundred */
             if (voiced(p-1)) sp=strcat(sp," d");       /* bagged */
             else             sp=strcat(sp," t");       /* backed */
             p++;
             continue;
	   }
          if ((*(p+1) == 's')&&(*(p+2) == ' ')) /* final "es" */
            {if (fric1(p-1))
               {sp=strcat(sp," ax z"); p++; continue;} /* bosses */
              if (voiced(p-1)) sp=strcat(sp," z");      /* aides */
              else             sp=strcat(sp," s");      /* crates */
              p++; continue;
            }

          if ((*(p+1) == 'r'))
            {if (*(p+2) == 'r')
               sp = strcat(sp," eh"); /* terror */
             else
               {if (!isvowel1(*(p+2)))
                  {sp = strcat(sp," ax r"); p++;} /* herd */
		else
                  {sp = strcat(sp," eh");}      /* heretic */
	       }
             continue;
	    }
          if (*(p+1) == 'a')
            {if (*(p+2) == 'd') sp = strcat(sp," eh"); /* head, instead */
             else               sp = strcat(sp," iy");
             p++; continue;
            }
          if (*(p+1) == 'e') {sp = strcat(sp," iy"); p++; continue;}
          if (*(p+1) == 'i') {sp = strcat(sp," ey"); p++; continue;}
          if (short_env(p)) sp = strcat(sp," eh");
          else              sp = strcat(sp," iy");
         }
/* "f" */
       if (*p == 'f') sp = strcat(sp," f");
/* "g" */
       if (*p == 'g')
         {if (*(p+1) == 'h')
            {if (*(p-1) == ' ')
               {sp = strcat(sp," g"); p++; continue;} /* ghost */
             else
               {p++; continue;} /* might flight higher */
            }
          if ((*(p+1) == 'n')&&(!isvowel1(*(p+2)))) continue; /* align */
          if ((*(p-1) == 'n')&&(*(p+1) == ' ')) continue; /* -ing */
          if (*(p-1) == 'g') {sp=strcat(sp," g"); continue;} /* bagged */
          if (is_front_vowel(*(p+1))) sp = strcat(sp," jh");
          else                        sp = strcat(sp," g");
         }
/* "h" */
       if (*p == 'h') sp = strcat(sp," hh");
/* "i" */
       if (*p == 'i')
         {if ((*(p+1) == 'g')&&(*(p+2) == 'h'))  /* igh... */
            {sp = strcat(sp," ay"); continue;}
          if (*(p+1) == 'e') {sp = strcat(sp," iy"); p++; continue;}
          if ((*(p+1) == 'o')&&(*(p+2) == 'n')) /* -ion */
            {if (bound(*(p-2))) /* #Cion */
               {sp = strcat(sp," ay ax"); p++; continue;} /* lion */
             else
               {if (*(p-1) == 'l')
                  {sp = strcat(sp," y ax"); p++; continue;}/* million */
                else
                  {sp = strcat(sp," ax"); p++; continue;} /* diction */
	    }  }
          if (short_env(p)) sp = strcat(sp," ih");
          else              sp = strcat(sp," ay");
         }
/* "j" */
       if (*p == 'j') sp = strcat(sp," jh");
/* "k" */
       if (*p == 'k')
         {if ((*(p-1) == ' ')&&(*(p+1) == 'n')) continue;/* knee */
          sp = strcat(sp," k");
         }
/* "l" */
       if (*p == 'l')
         {if ((*(p+1) == 'e') && (*(p+2) == ' ') &&(iscons1(*(p-1))))
	      {sp = strcat(sp," el"); p++; continue;} /* rifle */         
          sp = strcat(sp," l");
 	 }
/* "m" */
       if (*p == 'm')
         {if ((*(p-1) == ' ')&&(*(p+1) == 'c')) /* mcvey,mcintosh */
            {sp = strcat(sp," m ae k"); p++;}
         else sp = strcat(sp," m");
         }
/* "n" */
       if (*p == 'n')
         {if (velar(*(p+1))) sp = strcat(sp," nx");
          else sp = strcat(sp," n");
         }
/* "o" */
       if (*p == 'o')
         {if (*(p+1) == 'a') {sp = strcat(sp," ow"); p++; continue;}
          if (*(p+1) == 'u') {sp = strcat(sp," aw"); p++; continue;}
          if (*(p+1) == 'i') {sp = strcat(sp," oy"); p++; continue;}
          if (*(p+1) == 'y') {sp = strcat(sp," oy"); p++; continue;}
          if (*(p+1) == 'o') {sp = strcat(sp," uw"); p++; continue;}
          if (*(p+1) == 'r') {sp = strcat(sp," ow");      continue;}/* for */
          if ((*(p+1) == 'w') && (bound(*(p+2)))) /* sorrow, arrow, bow */
	    {sp = strcat(sp," ow"); p++; continue;}
          if (bound(*(p+1))) {sp = strcat(sp," ow");      continue;}
          if ((*(p+1) == 's')&&(*(p+2) == ' '))
             {sp = strcat(sp," ow"); continue;} /* tomatos */
          if (short_env(p)) sp = strcat(sp," aa");
          else              sp = strcat(sp," ow");
         }
/* "p" */
       if (*p == 'p')
         {if (*(p+1) == 'h') {sp = strcat(sp," f"); p++; continue;}
          if ((*(p+1) == 'n')&&(*(p-1) == ' ')) continue;
          if ((*(p+1) == 's')&&(*(p-1) == ' ')) continue;
          sp = strcat(sp," p");
         }
/* "q" */
       if (*p == 'q')
         {sp = strcat(sp," k w"); if (*(p+1) == 'u') p++; continue;}
/* "r" */
       if (*p == 'r')
         {if (*(p-1) == 'r')
            {sp=strcat(sp," r"); continue;} /* bizarre, pierre */
          if ((iscons1(*(p-1)))&&(*(p+1) == 'e')&&bound(*(p+2)))
            {sp=strcat(sp," ax r"); p++; continue;} /* acre,centre,sabre */
          sp = strcat(sp," r");
          if ((*(p-1) == ' ')&&(*(p+1) == 'h')) p++; /* Rhode */
          continue;
         }
/* "s" */
       if (*p == 's')
         {if (*(p+1) == 'h') {sp = strcat(sp," sh"); p++; continue;}
          if ((*(p+1) == 'i')&&(*(p+2) == 'o'))
            {if (*(p-1) == 's') sp = strcat(sp," sh");
             else               sp = strcat(sp," zh");
             continue;
            }
          if ((*(p+1) == ' ')&& voiced(p-1))  /* bags */
            {sp=strcat(sp," z"); continue;}
          sp = strcat(sp," s");
         }
/* "t" */
       if (*p == 't')
         {if (*(p+1) == 'h') {sp = strcat(sp," th"); p++; continue;}
          if ((*(p+1) == 'i')&&(*(p+2) == 'o'))
            {sp = strcat(sp," sh"); continue;}
          if ((*(p+1) == 'c')&&(*(p+2) == 'h')) continue; /* batch */
          sp = strcat(sp," t");
         }
/* "u" */
       if (*p == 'u')
         {if (*(p+1) == 'y')
            {sp = strcat(sp," ay"); p++; continue;} /* buy, guy */
          if (short_env(p)) sp = strcat(sp," ax");
          else
            {if ((*(p-1) == 'r'))
               {sp = strcat(sp," uw"); continue;} /* rune */
    /* American rule, not British or stage English: */
             if (coronal(*(p-1)))
               {sp = strcat(sp," uw"); continue;} /* tune */
             sp = strcat(sp," y uw");
         }  }
/* "v" */
       if (*p == 'v') sp = strcat(sp," v");
/* "w" */
       if (*p == 'w')
         {if (*(p+1) == 'h') {sp = strcat(sp," wh"); p++; continue;}
          if (*(p+1) == 'r') continue; /* wrist */
          sp = strcat(sp," w");
         }
/* "x" */
       if (*p == 'x') sp = strcat(sp," k s");
/* "y" */
       if (*p == 'y')
         {if (isvowel1(*(p+1))) sp = strcat(sp," y"); /* yes */
          else
            {if (bound(*(p+1)))  /* final y */
               {if ((iscons1(*(p-1)))&&(bound(*(p-2))))
                  {sp = strcat(sp," ay"); continue;} /*  my */
                if ((iscons1(*(p-1)))&&(iscons1(*(p-2)))&&(bound(*(p-3))))
                  {sp = strcat(sp," ay"); continue;} /* fly */
                sp = strcat(sp," iy");               /* envy */
               }
             else                 sp = strcat(sp," ih");
         }  }
/* "z" */
       if (*p == 'z') sp = strcat(sp," z");
/*       if (*p == ' ') sp = strcat(sp," #"); */
      }
 db_leave_msg(proc,1); /* debug only */
    return sp;
   }
/* end file ttp1.c */
