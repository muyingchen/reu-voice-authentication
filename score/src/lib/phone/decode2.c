/* file decode2.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /********************************************************/
/*  ***##** not this way now ***@@*** */
  /*  char *sex(sinin,linin,pcstr)                        */
  /*  Returns the external ascii string of phones whose   */
  /*  internal numeric code values are in sinin[1:linin]. */
  /*  Sex is a string, external, of ASCII pcode elements. */
  /*  Sinin is an array of int internal pcode elements.   */
  /*  This version expects phone index internal codes.    */
  /*  Uses the convention that the first element of       */
  /*  *psinin is *psinin[1], not *psinin[0].              */
  /*  If there is no match, returns 0.                    */
  /*  On return, *perr indicates exception:               */
  /*    *perr  condition                                  */
  /*      0     a.o.k.                                    */
  /*     11     phone string overflow                     */
  /********************************************************/

  /******************************************************************/
  /*  void decode2(psex,lpsex,pcstr,psinin,plinin,perr)             */
  /*                                                                */
  /*  input: psinin, plinin, lpsex                                  */
  /* output: psex   (strlen(psex) !> lpsex)                         */
  /*                                                                */
  /*  Converts sinin to sex (*psinin to *psex),according            */
  /*   to pcode "pcstr".   Sex is a string, external,               */
  /*   of ASCII pcode elements;  *psinin is an array of             */
  /*   int internal pcode elements, from *psinin[1] thru            */
  /*   *psinin[plinin].                                             */
  /*  This version expects phone indexes for internal               */
  /*   (*psinin) codes, and if the pcode is "separated",            */
  /*   it pads between code words with a " " (space).               */
  /*  If there is no match, returns 0. and will work on             */
  /*   either separated or non-separated pcodes.                    */
  /*  On return, *perr indicates exception:                         */
  /*    *perr  condition                                            */
  /*      0     a.o.k.                                              */
  /*     11     on entry, *plinin > MAX_SYMBS_IN_STR                */
  /*     13     *psex not big enough to hold decoded *psinin array  */
  /*                                                                */
  /* Parameterized by MAX_SYMBS_IN_STR.                             */
  /*                                                                */
  /* Modified 1/3/91 by WMF; commented out a call to decode2 of the */
  /* first part, which was done as part of an error msg; it caused  */
  /* problems when used with VERY long strings.                     */
  /*                                                                */
  /******************************************************************/

   void decode2(char *psex, int lpsex, pcodeset *pc1, int *psinin, int *plinin, int *perr)
{
/* local data */
   char *proc = " decode2";
   char estr[16], *pestr = &estr[0];
   int prfield = 16, *pprfield = &prfield, i, pnext_length, psex_length;
   char *pnext;
 /* coding */
 db_enter_msg(proc,3); /* debug only */
   *perr = 0;
   psex = strcpy(psex,"");
   psex_length = 0;
   if (*plinin > MAX_SYMBS_IN_STR)
     {fprintf(stderr,"%s: on entry, *plinin (# of elements in\n",proc);
      fprintf(stderr," psinin array) is larger than MAX_SYMBS_IN_STR.\n");
      fprintf(stderr,"  *plinin (array size) =%d\n",*plinin);
      fprintf(stderr," (Increase MAX_SYMBS_IN_STR.)\n");
/*
      decode2(pestr,prfield,pc1,psinin,pprfield,perr);
      fprintf(stderr," First part = %s\n",pestr);
*/
      *perr = 11;
      return;};
   for (i = 1; i <= *plinin; i++)
     {if ( (pc1->separated) && (i>1) )
        {psex = strcat(psex," "); psex_length += 1;}
      pnext = pc1->pc[psinin[i]].ac;
      pnext_length = strlen(pnext);
      if ((psex_length + pnext_length - 1) < lpsex)
         {psex_length += (pnext_length - 1);
          psex = strcat(psex,pc1->pc[psinin[i]].ac);
	 }
      else
         {fprintf(stderr,"%s: *psex (char string output) overflow.\n",proc);
          fprintf(stderr," Need greater lpsex (=%d)\n",lpsex);
          decode2(pestr,prfield,pc1,psinin,pprfield,perr);
          fprintf(stderr," First part = %s\n",pestr);
          *perr = 13;
          return;
	 }
     } /*  end of "for (i = 1; i <= *plinin; i++) ..." */
 db_leave_msg(proc,3); /* debug only */
   return;
  } /* end of decode2 */
