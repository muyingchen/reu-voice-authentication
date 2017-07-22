 /* file encode2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /*********************************************************/
  /*  void encode2(psex_in,pcstr,psinin,plinin,perr)       */
  /*                                                       */
  /*  input: psex_in,pcstr                                 */
  /* output:psinin,plinin,perr                             */
  /*                                                       */
  /*  Converts sex to sinin (*psex_in to *psinin), acc.    */
  /*   to pcode "pcstr".   Sex is a string, external,      */
  /*   of ASCII pcode elements;  *psinin is an array of    */
  /*   int internal pcode elements, from *psinin[1] thru   */
  /*   *psinin[plinin].                                    */
  /*  This version uses phone indexes for internal codes;  */
  /*   if there is no match, returns 0; and will work on   */
  /*   both separated and non-separated (single character) */
  /*   codes.                                              */
  /*  On return, *perr indicates exception:                */
  /*    *perr  condition                                   */
  /*      0     a.o.k.                                     */
  /*      1     phone code not found in pcode              */
  /*     11     phone string overflow                      */
  /*                                                       */
  /* Modified 7/18/90 by WMF with a quick fix to make it   */
  /* work on 1-character separated codes.                  */
  /*                                                       */
  /* Modified 6/15/92 by WMF to use sstok (instead of      */
  /* strtok) so that it can be used recursively.           */
  /*********************************************************/

  void encode2(char *psex_in, pcodeset *pcstr, int *psinin, int *plinin, int *perr)
{char *proc = " encode2";
   char psxx[LINE_LENGTH], *psymb = &psxx[0], *prem;
   SUBSTRING ssremx, *ssrem = &ssremx;
   SUBSTRING psymbss_x, *psymbss = &psymbss_x;
   int int_code;
   boolean done;
/* code: */
 db_enter_msg(proc,2); /* debug only */
 if (db_level > 2)
  {printf("*DB: entering encode2, *psex_in='%s'\n",psex_in);
   if (pcstr->separated) printf("*DB: pcode pcstr is separated.\n");
   else              printf("*DB: pcode pcstr is NOT separated.\n");
  }
   *perr = 0;
   *plinin = 0;
/* check for degenerate case */
   if (psex_in != NULL)
     {ssrem->start = psex_in;
      ssrem->end   = prtrim(psex_in);
      done = F;
      if (pcstr->separated)
        {while ((! done) && (*perr == 0))
           {*psymbss = sstok(ssrem,pcstr->sep_codes);
            if (substr_length(psymbss) < 1) break;
            psymb = substr_to_str(psymbss,psymb,LINE_LENGTH);
if (db_level > 2) printf("*DB: trying to encode '%s'\n",psymb);
            if (*plinin < MAX_SYMBS_IN_STR)
              {*plinin += 1;
               int_code = pcindex2(psymb,pcstr);
               if (int_code >= 0)  
                 psinin[*plinin] = int_code;
               else
                 {fprintf(stderr,"*ERR:%s: invalid code '%s'.\n",proc,psymb);
                  *plinin -= 1;
                  *perr=1;
              }  }
            else
              {fprintf(stderr,
                "*ERR:%s:phone string ovf; increase MAX_SYMBS_IN_STR.\n",proc);
               *perr=11;
               break;
              }
            ssrem->start = psymbss->end + 1;
	}  }
      else
        {psymb[1] = '\0';
      /* NOTE - This is a quick fix useful for 1-symbol codes only!!! */
         for (prem=psex_in; *prem != '\0'; prem++)
           {*psymb = *prem;
            if (*plinin < MAX_SYMBS_IN_STR)
              {*plinin += 1;
               int_code = pcindex2(psymb,pcstr);
               if (int_code >= 0)  
                 psinin[*plinin] = int_code;
               else
                 {fprintf(stderr,"*ERR:%s: invalid code '%s'.\n",proc,psymb);
                  *plinin -= 1;
                  *perr=1; goto RETURN;
              }  }
            else
              {fprintf(stderr,
                "*ERR:%s:phone string ovf; increase MAX_SYMBS_IN_STR.\n",proc);
               *perr=11; goto RETURN;
     }  }  }  }
 RETURN:
 db_leave_msg(proc,2); /* debug only */
   return;
  } /* end of encode2 */
