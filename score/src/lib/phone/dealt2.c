/* file dealt2.c */
/* function dealt2() and supporting functions            */
/* for removing alternations from a transcription string */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif



  char *dealt2(char *sy, char *sx,
     boolean zap_if_null, boolean zap_if_nonlex,
     boolean dont_pick_null, boolean dont_pick_nonlex,
     int *perr)

  /******************************************************************/
  /*                                                                */
  /*  Removes alternations in transcription string *sx in favor of  */
  /*  one of the alternants, putting results into *sy.              */
  /*  In place of an alternation, the first alternant is output     */
  /*  subject to constraints specified by these boolean parameters: */
  /*  if zap_if_null, outputs nothing if an alternant is null.      */
  /*  if zap_if_nonlex, outputs nothing if an alternant is "[...]"  */
  /*  if dont_pick_null, will not output a null alternant.          */
  /*  if dont_pick_nonlex, will not output a "[...]" alternant.     */
  /*   (Fatal error if memory allocation fails.)                    */
  /*  Returns pointer to sy so you can use it like s = dealt2();    */
  /*                                                                */
  /******************************************************************/

   {char *proc = "dealt2";
    char *px, *py, *pa, lbound = '{', rbound = '}';
    char *alt[MAX_ALTS];
    SUBSTRING ssa_data, *ss_alts = &ssa_data;
    int i,nalts;
    static boolean first_time = T;
  /* instructions */
    db_enter_msg(proc,0);
if (db_level > 2) printf("%s sx='%s'\n",pdb,sx); 
    if (first_time)
      {first_time = F;
       for (i=1; i <= MAX_ALTS; i++) alt[i] = NULL;
      } 
  /* px points to next sx character to process             */
  /* py points to next sy character location to write into */
    px = sx;
    py = sy;
    for (px = sx; *px != '\0'; px++)
      {if (*px != lbound) *py++ = *px;
       else /* process alternation */
         {ss_alts->start = px + 1; /* forget boundary markers */
          ss_alts->end   = strchr(px,rbound) - 1;
          if (ss_alts->end == NULL)
            {fprintf(stderr,"*ERR:%s: transcription contains left",proc);
             fprintf(stderr," alternation boundary but no right one:\n");
             fprintf(stderr," '%s'\n",sx);
             *perr = 1;
             goto RETURN;
            }
          parse_alts(ss_alts,&nalts,alt);
          i = pick_alt(nalts,alt,zap_if_null,zap_if_nonlex,
            dont_pick_null, dont_pick_nonlex);
          if (i > 0)
            {/* copy alt #i to output string */
             for (pa = alt[i]; *pa != '\0'; pa++) *py++ = *pa;
            }
          px = ss_alts->end + 1;
      }  }
RETURN:
     *py = '\0';
    db_leave_msg(proc,0);
    return sy;
   } /* end dealt2 */

  void parse_alts(SUBSTRING *ss_alts, int *nalts, char *alt[MAX_ALTS])
/*************************************************************************/
/* Parses string of alternates in *ss_alts into alternate table *alt[].  */
/* Alternate boundaries are ss_alt->start, '/', and ss_alt->end.         */
/* Beginning and ending spaces of an alternate token are stripped off.   */
/*************************************************************************/
   {char *proc = "parse_alts";
    char sxx[LONG_LINE], *sx = &sxx[0];
    char sxt[LONG_LINE], *str_token = &sxt[0];
    boolean done;
    SUBSTRING tkx_data, *token = &tkx_data;
  /* instructions */
    db_enter_msg(proc,0);
    *nalts = 0;
    done = F;
    while (!done)
      {*token = sstok(ss_alts,"/");
       if (token->start > ss_alts->end) done = T;
       else
         {if (*nalts < MAX_ALTS)
            {*nalts += 1;
             if (alt[*nalts] != NULL) free(alt[*nalts]);
             str_token = substr_to_str(token,str_token,LONG_LINE);
             alt[*nalts] = strdup_safe(pltrim(prtrim2(str_token)),proc);
	    }
          else
            {fprintf(stderr,"*ERR:%s: Too many alternants.\n",proc);
             fprintf(stderr," increase MAX_ALTS and re-compile.\n");
             sx = substr_to_str(ss_alts,sx,LONG_LINE);
             fprintf(stderr," '%s'\n",sx);
         }  }
       ss_alts->start = token->end + 1;
      }
    db_leave_msg(proc,0);
    return;
   } /* end parse_alts */


  int pick_alt(int nalts, char *alt[MAX_ALTS],
     boolean zap_if_null, boolean zap_if_nonlex,
     boolean dont_pick_null, boolean dont_pick_nonlex)
/*****************************************************************/
/* Picks one of the alternates in the alternate table alt[].     */
/*****************************************************************/
   {char *proc = "pick_alt";
    char *null_symbol = "@";
    boolean zap;
    int i, ialt;
    int idb;
  /* instructions */
 db_enter_msg(proc,0);
if (db_level > 0)
  {printf("%szap_if_null  ='%s'\n",pdb,bool_print(zap_if_null));
   printf("%szap_if_nonlex='%s'\n",pdb,bool_print(zap_if_nonlex));
   printf("%sdont_pick_null  ='%s'\n",pdb,bool_print(dont_pick_null));
   printf("%sdont_pick_nonlex='%s'\n",pdb,bool_print(dont_pick_nonlex));
  }
if (db_level > 0)
  {printf("%s DUMP OF ALT TABLE:\n",pdb);
   printf("%s nalts = %d\n",pdb,nalts);
   printf("%s   I   'ALT[I]'\n",pdb);
   for (idb = 1; idb <= nalts; idb++)
     printf("%s %3d   '%s'\n",pdb,idb,alt[idb]);
   printf("%s\n",pdb);
  }
  /* check if whole alternation should be zapped */
    zap = F;
    if (zap_if_null || zap_if_nonlex)
      {for (i = 1; (!zap) && (i <= nalts); i++)
         {if ((zap_if_null)&&(streq(alt[i],null_symbol))) zap=T;
          if ((zap_if_nonlex)&&(*alt[i] == '['))          zap=T;
      }  }   
    if (zap) ialt = 0;
    else
      {
   /* otherwise pick one of the alternants */
       ialt = 0;
       for (i = 1; (ialt == 0) && (i <= nalts); i++)
         {if (streq(alt[i],null_symbol) && dont_pick_null) continue;
          if ((*alt[i] == '[') && dont_pick_nonlex)        continue;
          ialt = i;
      }  }  
 
if (db_level > 0) printf("%sreturning ialt=%d\n",pdb,ialt);
    db_leave_msg(proc,0);
    return ialt;
   } /* end pick_alt */
