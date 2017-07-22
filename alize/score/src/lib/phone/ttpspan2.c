/* file ttpspan2.c                                                    */
/* function ttpspan2(stx,sp,path,perr) for converting Spanish text to phones */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /*************************************************/
  /*  char *ttpspan2(stx,sp,path,perr)             */
  /*  Converts Spanish text to phonemes.           */
  /*  Input: *stx is a text string;                */
  /*  Output: *sp is a phone string;               */
  /*  Process:                                     */
  /*    1. apply text-to-phoneme rules.            */
  /*    2. add stress to words not already marked. */
  /*  Note: uses rules in <path>/span_ttp.rls;      */
  /*  these can be either Castilian or New World.  */
  /*  Returns *perr > 0 iff error.                 */
  /*************************************************/
  char *ttpspan2(char *stx, char *sp, char *path, int *perr)
{char *proc = "ttpspan2";
    char st_data[MAX_REC_LENGTH], *st = &st_data[0];
    char sp1_data[MAX_REC_LENGTH], *sp1 = &sp1_data[0];
    char sx_data[LINE_LENGTH], *sx = &sx_data[0];
    char dbsx_data[LINE_LENGTH], *dbsx = &dbsx_data[0];
    char *fname_rules = "span_ttp.rls";
    char *p,*q;
    char *vowels="aeioiu";
    char *n_or_s="ns";
    SUBSTRING sswx, *ssword = &sswx;
    SUBSTRING ssxx, *ssx = &ssxx;
    boolean already_stressed;
    int nsylbs;
    static RULESET1 rset_data, *rset = &rset_data;
    static boolean rules_read_in;
/* code */
 db_enter_msg(proc,1); /* debug only */
if (db_level > 2) printf("*DB: stx=[%s]\n",stx);
    *perr = 0;
/* get rules if we haven't already */
    if (!rules_read_in)
      {get_rules1(rset, path, fname_rules, perr);
       if (*perr > 0)
         {fprintf(stderr,"*ERR:%s: Get_rules1 returned err code =%d\n",
            proc,*perr);
          goto RETURN;
         }
       rules_read_in = T;
if (db_level > 2) printf("%s%d Spanish rules read in.\n",pdb,rset->nrules);
      }
/* pre-process */
    q = st;
    if (*stx != ' ') *q++ = ' '; /* insert leading space */
    for (p = stx; *p != '\0'; p++)
      {if ( (!ispunct(*p)) || (*p == '\''))  /* ignore punctuation */
        {*q++ = tolower(*p);     /* lower-case it */
      } }
    if (*(p-1) != ' ')  *q++ = ' '; /* insert trailing space */
    *q = '\0';
/* convert letter to phone */
     sp1 = strcpy(sp1,"");
     sp1 = apply_rules1(sp1,st,rset,perr);
     if (*perr > 0)
       {fprintf(stderr,"*ERR:%s: apply_rules returns perr=%d\n",proc,*perr);
       }
/* add stress marks */
     ssword->end = pltrim(sp1);
     sp = strcpy(sp,"");
     do
       {/* find next word and whether or not it's been stressed */
        already_stressed = F;
        ssword->start = ssword->end;
        ssword->end   = ssword->start+1;
        while ((*(ssword->end) != '\0')&&(*(ssword->end) != '#'))
          {if (*(ssword->end) == '1') already_stressed = T;
           ssword->end += 1;
          }
        if (*(ssword->end) == '\0') continue;
if (db_level > 2)
  {substr_to_str(ssword,dbsx,LINE_LENGTH);
   printf("%snext word (in brackets): [%s]\n",pdb,dbsx);
  }
        if (*(ssword->end) == '#')
          {if (already_stressed)
             {/* copy over as is */
              ssword->end -= 1;
              substr_to_str(ssword,sx,LINE_LENGTH);
              sp = strcat(sp,sx);
              ssword->end += 1;
             }
           else
             {/* copy adding stress marks */

if (db_level > 2)
 {printf("%sstrchr(vowels...)='%c'\n",pdb,*strchr(vowels,*(ssword->end-2)));
  printf("%sstrchr(n_or_s...)='%c'\n",pdb,*strchr(n_or_s,*(ssword->end-2)));
 }
              nsylbs = 0;
              for(p=ssword->start; p != ssword->end; p++)
                if (strchr(vowels,*p)) nsylbs += 1;
if (db_level > 2) printf("%snsylbs=%d\n",pdb,nsylbs);
              if ((nsylbs > 1) &&
                  ((strchr(vowels,*(ssword->end-2)) != NULL) ||
                   (strchr(n_or_s,*(ssword->end-2)) != NULL)))
                {/* stress penult */
if (db_level > 2) printf("%sstressing penultimate\n",pdb);
                 p = ssword->end;
                 while (!strchr(vowels,*p)) p -= 1;
                 p -= 1;
                 while (!strchr(vowels,*p)) p -= 1;
		}
              else
		{/* stress ultimate */
if (db_level > 2) printf("%sstressing ultimate\n",pdb);
                 p = ssword->end;
                 while (!strchr(vowels,*p)) p -= 1;
                }
              /* p points to V to be stressed */
              ssx->start = ssword->start;
              ssx->end   = p-1;
              substr_to_str(ssx,sx,LINE_LENGTH);
if (db_level > 2) printf("%s1st part of word=[%s]\n",pdb,sx);
              sp = strcat(sp,sx);
              sp = strcat(sp,"'1 ");
              ssx->start = p;
              ssx->end   = ssword->end - 1;
              substr_to_str(ssx,sx,LINE_LENGTH);
if (db_level > 2) printf("%s2nd part of word=[%s]\n",pdb,sx);
              sp = strcat(sp,sx);
       }  }  }
     while (*(ssword->end) != '\0');
     sp = strcat(sp,"#");
RETURN:
 db_leave_msg(proc,1); /* debug only */
    return sp;
   }
/* end file ttpspan2.c */
