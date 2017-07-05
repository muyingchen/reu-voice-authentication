
/* file gmarkf1.c                                               */
/* defines functions "get_mark_file()", "dump_word_tokens()",   */
/* "free_mark_file()", "get_next_mark_utt()", and               */
/* "fill_mark_struct()", along with functions supporting them:  */
/*  mispron(), start_overlap(), end_overlap(), start_comment(), */
/*  end_comment(), start_unsure(), end_unsure(),                */
/*  start_crosstalk(), end_crosstalk(), start_mispron(), and    */
/*  end_mispron().                                              */
/* parameterized by MAX_WORDS_IN_FILE, MAX_STR_NCHARS           */
/* Changed 2/28/95 by Jon Fiscus & Bill Fisher to accomodate    */
/* new fields in revised version of word token data structure,  */
/* "wtokstr1.h", and partial reads of mark files.               */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif



 /**********************************************************************/
 /*                                                                    */
 /*    void dump_word_tokens(word_tokens,lim);                         */
 /*    WTOKE_STR1 *word_tokens; int lim;                               */
 /*                                                                    */
 /*    Dumps up to <lim> of *word_tokens.                              */
 /*                                                                    */
 /**********************************************************************/
 void dump_word_tokens(WTOKE_STR1 *word_tokens, int lim)
  {int i;
   printf("\n");
   printf(" DUMP OF WORD TOKENS (OCCURRENCES):\n");
   printf(" (Only the first %d shown.)",lim);
   printf("\n");
   printf(" No. of word tokens = %d     Id = %s\n",
	  word_tokens->n,word_tokens->id);
   printf("                          CON-  OVER  MIS  UN- CROS COM- \n");
   printf(" TURN      T1      DUR    FID   LAPD PRON SURE TALK MENT BAD  WORD\n");
   for (i=1; (i<=word_tokens->n)&&(i<=lim); i++)
    {printf("  %2.2s  %9.2f  %6.2f  %6.2f  %s    %s    %s    %s    %s    %s    %s\n",
       word_tokens->word[i].turn,
       word_tokens->word[i].t1,
       word_tokens->word[i].dur,
       word_tokens->word[i].confidence,
       bool_print(word_tokens->word[i].overlapped),
       bool_print(word_tokens->word[i].mispronounced),
       bool_print(word_tokens->word[i].unsure),
       bool_print(word_tokens->word[i].crosstalk),
       bool_print(word_tokens->word[i].comment),
       bool_print(word_tokens->word[i].bad_marking),
       word_tokens->word[i].sp);
     }
   return;
  } /* end of function "dump_word_tokens" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean mispron(sp) char *sp;                                   */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp is mispronounced.                                        */
 /*                                                                    */
 /**********************************************************************/
 boolean mispron(char *sp)
  {char *proc = "mispron";
/* data: */
   boolean bad;
   char xspx[LINE_LENGTH], *xsp = &xspx[0];
/* code: */
db_enter_msg(proc,3); /* debug only */
   xsp = clean_up2(strcpy(xsp,sp),"-[]");
   if ((*pltrim(xsp) == '-') || (*prtrim(xsp) == '-'))
      bad = T;
   else
      bad = F;
 db_leave_msg(proc,3); /* debug only */
   return bad;
  } /* end of function "mispron" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean start_overlap(sp) char *sp;                             */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp starts a section of overlapped speech.                   */
 /*                                                                    */
 /**********************************************************************/
 boolean start_overlap(char *sp)
  {char *proc = "start_overlap";
/* data: */
   boolean flag_found;
   char *p1;
/* code: */
db_enter_msg(proc,9); /* debug only */
if (db_level > 9) printf("%ssp='%s'\n",pdb,sp);

/* '#' must be in the header, before the first alpha char or '-' */
   flag_found = F;
   p1 = sp;
   while ((!flag_found) &&
          (*p1 != '\0') &&
          (*p1 != '-')  &&
          (ispunct(*p1)) )
     {if (*p1 == '#')
        {flag_found = T;
        }
      p1 += 1;
     }
 db_leave_msg(proc,9); /* debug only */
   return flag_found;
  } /* end of function "start_overlap" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean end_overlap(sp) char *sp;                               */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp ends a section of overlapped speech.                     */
 /*                                                                    */
 /**********************************************************************/
 boolean end_overlap(char *sp)
  {char *proc = "end_overlap";
/* data: */
   boolean flag_found;
   char *p1;
/* code: */
db_enter_msg(proc,9); /* debug only */
if (db_level > 9) printf("%ssp='%s'\n",pdb,sp);

/* '#' must be in the trailer, after the last alpha char or '-' */
   flag_found = F;
   p1 = prtrim(sp);
   while ((!flag_found) &&
          (p1 >= sp)    &&
          (*p1 != '-')  &&
          (ispunct(*p1)) )
     {if (*p1 == '#') flag_found = T;
      p1 -= 1;
     }
 db_leave_msg(proc,9); /* debug only */
   return flag_found;
  } /* end of function "end_overlap" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean start_comment(sp) char *sp;                             */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp starts a comment section.                                */
 /*                                                                    */
 /**********************************************************************/
 boolean start_comment(char *sp)
  {char *proc = "start_comment";
/* data: */
   boolean found;
/* code: */
db_enter_msg(proc,9); /* debug only */
   if (strchr(sp,'{') != NULL) found = T;
   else  found = F;
db_leave_msg(proc,9); /* debug only */
   return found;
  } /* end of function "start_comment" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean end_comment(sp) char *sp;                               */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp ends a comment section.                                  */
 /*                                                                    */
 /**********************************************************************/
 boolean end_comment(char *sp)
  {char *proc = "end_comment";
/* data: */
   boolean found;
/* code: */
db_enter_msg(proc,9); /* debug only */
   if (strchr(sp,'}') != NULL) found = T;
   else  found = F;
db_leave_msg(proc,9); /* debug only */
   return found;
  } /* end of function "end_comment" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean start_unsure(sp) char *sp;                              */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp starts a section that the transcriber was unsure of.     */
 /*                                                                    */
 /**********************************************************************/
 boolean start_unsure(char *sp)
  {char *proc = "start_unsure";
/* data: */
   boolean found;
/* code: */
db_enter_msg(proc,9); /* debug only */
   if (strstr(sp,"((") != NULL) found = T;
   else  found = F;
db_leave_msg(proc,9); /* debug only */
   return found;
  } /* end of function "start_unsure" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean end_unsure(sp) char *sp;                                */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp ends a section that the transcriber was unsure of.       */
 /*                                                                    */
 /**********************************************************************/
 boolean end_unsure(char *sp)
  {char *proc = "end_unsure";
/* data: */
   boolean found;
/* code: */
db_enter_msg(proc,9); /* debug only */
   if (strstr(sp,"))") != NULL) found = T;
   else  found = F;
db_leave_msg(proc,9); /* debug only */
   return found;
  } /* end of function "end_unsure" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean start_crosstalk(sp) char *sp;                           */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp starts a section of crosstalked speech.                  */
 /*                                                                    */
 /**********************************************************************/
 boolean start_crosstalk(char *sp)
  {char *proc = "start_crosstalk";
/* data: */
   boolean flag_found;
   char *p1;
/* code: */
db_enter_msg(proc,9); /* debug only */
if (db_level > 9) printf("%ssp='%s'\n",pdb,sp);

/* '^' must be in the header, before the first alpha char or '-' */
   flag_found = F;
   p1 = sp;
   while ((!flag_found) &&
          (*p1 != '\0') &&
          (*p1 != '-')  &&
          (ispunct(*p1)) )
     {if (*p1 == '^')
        {flag_found = T;
        }
      p1 += 1;
     }
 db_leave_msg(proc,9); /* debug only */
   return flag_found;
  } /* end of function "start_crosstalk" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean end_crosstalk(sp) char *sp;                             */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp ends a section of crosstalked speech.                    */
 /*                                                                    */
 /**********************************************************************/
 boolean end_crosstalk(char *sp)
  {char *proc = "end_crosstalk";
/* data: */
   boolean flag_found;
   char *p1;
/* code: */
db_enter_msg(proc,9); /* debug only */
if (db_level > 9) printf("%ssp='%s'\n",pdb,sp);

/* '^' must be in the trailer, after the last alpha char or '-' */
   flag_found = F;
   p1 = prtrim(sp);
   while ((!flag_found) &&
          (p1 >= sp)    &&
          (*p1 != '-')  &&
          (ispunct(*p1)) )
     {if (*p1 == '^') flag_found = T;
      p1 -= 1;
     }
 db_leave_msg(proc,9); /* debug only */
   return flag_found;
  } /* end of function "end_crosstalk" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean start_mispron(sp) char *sp;                             */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp starts a section of mispronounced speech.                */
 /*                                                                    */
 /**********************************************************************/
 boolean start_mispron(char *sp)
  {char *proc = "start_mispron";
/* data: */
   boolean flag_found;
   char *p1;
/* code: */
db_enter_msg(proc,9); /* debug only */
if (db_level > 9) printf("%ssp='%s'\n",pdb,sp);

/* '%' must be in the header, before the first alpha char or '-' */
   flag_found = F;
   p1 = sp;
   while ((!flag_found) &&
          (*p1 != '\0') &&
          (*p1 != '-')  &&
          (ispunct(*p1)) )
     {if (*p1 == '%')
        {flag_found = T;
        }
      p1 += 1;
     }
 db_leave_msg(proc,9); /* debug only */
   return flag_found;
  } /* end of function "start_mispron" */


 /**********************************************************************/
 /*                                                                    */
 /*    boolean end_mispron(sp) char *sp;                               */
 /*                                                                    */
 /*    Determines by its transcription spelling whether or not the     */
 /*  word *sp ends a section of mispronounced speech.                  */
 /*                                                                    */
 /**********************************************************************/
 boolean end_mispron(char *sp)
  {char *proc = "end_mispron";
/* data: */
   boolean flag_found;
   char *p1;
/* code: */
db_enter_msg(proc,9); /* debug only */
if (db_level > 9) printf("%ssp='%s'\n",pdb,sp);

/* '%' must be in the trailer, after the last alpha char or '-' */
   flag_found = F;
   p1 = prtrim(sp);
   while ((!flag_found) &&
          (p1 >= sp)    &&
          (*p1 != '-')  &&
          (ispunct(*p1)) )
     {if (*p1 == '%') flag_found = T;
      p1 -= 1;
     }
 db_leave_msg(proc,9); /* debug only */
   return flag_found;
  } /* end of function "end_overlap" */


 /**********************************************************************/
 /*                                                                    */
 /*    void get_mark_file(fname,word_tokens,perr);                     */
 /*    char *fname; WTOKE_STR1 *word_tokens; int *perr;                 */
 /*                                                                    */
 /*    Gets a list of time-marked word tokens from *fname into the     */
 /*  structure *word_tokens.                                           */
 /*   *perr = 0 means a.o.k.                                           */
 /*   error codes:                                                     */
 /*    1: invalid mispronunciation mark at start of file.              */
 /*   11: error in opening file.                                       */
 /*   13: word_tokens array overflow.                                  */
 /*                                                                    */
 /**********************************************************************/
 void get_mark_file(char *fname, WTOKE_STR1 *word_tokens, int *perr)
  {
/* data: */
   char *proc = "get_mark_file";
   FILE *fp;
   int i, j, n, iret;
   char sxx[LINE_LENGTH], *sx    = &sxx[0];
   char xtr[LINE_LENGTH], *xturn = &xtr[0];
   char xco[LINE_LENGTH], *xconf = &xco[0];
   char xxx[LINE_LENGTH], *xsp   = &xxx[0];
   char s2x[LINE_LENGTH], *s2   = &s2x[0];
   char s3x[LINE_LENGTH], *s3   = &s3x[0];
   double xt1, xdur;
   char comment_char = ';';
   boolean in_overlap, in_comment, in_unsure, in_mispron, in_crosstalk;
/* code: */
db_enter_msg(proc,0); /* debug only */
   n = 0;
   *perr = 0;
   fp = fopen(del_eol(fname),"r");
   if (fp == NULL)
     {fprintf(stdout,"; *ERR:%s: Can't open %s\n",proc,fname);
      *perr = 11; goto RETURN;
     }
 /* and loop on contents */
   in_overlap   = F;
   in_comment   = F;
   in_unsure    = F;
   in_mispron   = F;
   in_crosstalk = F;
   while ((sx = fgets(sx,LINE_LENGTH,fp)) != NULL)
     {if (*sx != comment_char)
        {n += 1;
	 *s3 = *s3 = *xturn = *xsp = *xconf = '\0';
	 if (0)
	     iret = sscanf(sx,"%s %s %s %s",xturn,s2,s3,xsp);
	 else
	     iret = sscanf(sx,"%s %s %s %s %s",s2,s3,xsp,xturn,xconf);
	 
         if (iret != EOF)
	   {if (n >= MAX_WORDS_IN_FILE)
              {fprintf(stderr,"*ERR: too many words in file.\n");
               fprintf(stderr," increase MAX_WORDS_IN_FILE & recompile.\n");
               fprintf(stdout,"; *ERR: too many words in file.\n");
               fclose(fp);
               *perr = 13; goto RETURN;
              }
            if ((s2[0] == '&') && (s2[1] == '&')) /* bad time marks */
              {xt1 = atof(s2+2);
               word_tokens->word[n].bad_marking = T;
              }
            else
              {xt1 = atof(s2);
               word_tokens->word[n].bad_marking = F;
              }
            xdur = atof(s3);
            if (start_overlap(xsp))   in_overlap   = T;
            if (start_comment(xsp))   in_comment   = T;
            if (start_unsure(xsp))    in_unsure    = T;
            if (start_mispron(xsp))   in_mispron   = T;
            if (start_crosstalk(xsp)) in_crosstalk = T;

            word_tokens->word[n].overlapped    = in_overlap;
            word_tokens->word[n].comment       = in_comment;
            word_tokens->word[n].unsure        = in_unsure;
            word_tokens->word[n].mispronounced = in_mispron;
            word_tokens->word[n].crosstalk     = in_crosstalk;

            if (end_overlap(xsp))   in_overlap   = F;
            if (end_comment(xsp))   in_comment   = F;
            if (end_unsure(xsp))    in_unsure    = F;
            if (end_mispron(xsp))   in_mispron   = F;
            if (end_crosstalk(xsp)) in_crosstalk = F;

            if (mispron(xsp)) word_tokens->word[n].mispronounced = T;
            if (strstr(xsp,"{mispronounced}") != NULL) /* prev word flag */
              {if (n > 1) word_tokens->word[n-1].mispronounced = T;
               else
                 {fprintf(stderr,"*ERR:%s:file '%s' has\n",proc,fname);
                  fprintf(stderr,"  mispronunciation mark at start.\n");
                  *perr = 1;
	      }  }
	    word_tokens->word[n].confidence = (strcmp(xconf,"") == 0) ? 0.0 : atof(xconf);
            word_tokens->word[n].turn = strdup(xturn);
            word_tokens->word[n].t1   = xt1;
            word_tokens->word[n].dur  = xdur;
            word_tokens->word[n].sp   = strdup(xsp);
	   }
         else n -= 1;
        }
     }
   fclose(fp);
/* mark overlap indicated by following turn being starred */
   i = 2;
   while ((i <= n)&&(*(word_tokens->word[i].turn) != '*')) i++;
   if (i <= n)
     {xturn = strcpy(xturn,word_tokens->word[i-1].turn);
      for (j=i-1; ((j > 0)&&streq(word_tokens->word[j].turn,xturn)); j--);
        {word_tokens->word[j].overlapped = T;
     }  }
RETURN:
   word_tokens-> n = n;
 db_leave_msg(proc,0); /* debug only */
   return;
  } /* end of function "get_mark_file" */


 /**********************************************************************/
 /*                                                                    */
 /*    void free_mark_file(word_tokens);                               */
 /*    WTOKE_STR1 *word_tokens;                                         */
 /*                                                                    */
 /*    Frees the dynamic memory allocated to hold *word_tokens.        */
 /*                                                                    */
 /**********************************************************************/
 void free_mark_file(WTOKE_STR1 *word_tokens)
  {
/* data: */
   char *proc = "free_mark_file";
   int i;
/* code: */
db_enter_msg(proc,0); /* debug only */
   for (i=1; i <= word_tokens->n; i++)
     {/* free((void *)word_tokens->word[i].turn); */ /* K&R */
      free(word_tokens->word[i].turn);
      /* free((void *)word_tokens->word[i].conv); */ /* K&R */
      free(word_tokens->word[i].conv);
      /* free((void *)word_tokens->word[i].sp);   */ /* K&R */
      free(word_tokens->word[i].sp);
      word_tokens->word[i].overlapped = F;
      word_tokens->word[i].mispronounced = F;
      word_tokens->word[i].unsure = F;
      word_tokens->word[i].comment = F;
      word_tokens->word[i].bad_marking = F;
      word_tokens->word[i].crosstalk = F;
      word_tokens->word[i].alternate = F;
     }
   /* free((void *)word_tokens->id);   */ /* K&R */
   free(word_tokens->id);
 db_leave_msg(proc,0); /* debug only */
   return;
  } /* end of function "free_mark_file" */


 /**********************************************************************/
 /*                                                                    */
 /*   static boolean is_utt_id(char *sx, char *uttid)                  */
 /*                                                                    */
 /*   Check the string, sx, to see if it's a valid utterance           */
 /*   identifier, i.e. enclosed in parens and no whitespace in the     */
 /*   parens.  If the utt id is valid, it is returned in uttid (which  */
 /*   must be memory passed to the function and the function returns   */
 /*   T, otherwise F is returned.                                      */
 /*   **** Added by Jon Fiscus                                         */
 /**********************************************************************/
static boolean is_utt_id(char *sx, char *uttid){
    char *s, *e, *t;
    *uttid = '\0';
    if ((s=strchr(sx,'(')) == NULL)
	return(F);
    if ((e=strrchr(sx,')')) == NULL)
	return(F);
    /* printf("is_utt_id: %s\n",sx); */
    /* make sure there are no blanks or junk between the parens */
    for (t=s; t <= e; t++)
	if (isspace(*t))
	    return(F);
    strncpy(uttid, s, (e-s)+1);
    uttid[(e-s)+1] = '\0';
    /* printf("is_utt_id: found %s\n",uttid); */
    return(T);
}

 /**********************************************************************/
 /*                                                                    */
 /*   static boolean is_end_utt_id(char *sx, char *uttid)              */
 /*                                                                    */
 /*   Check the string, sx, to see if it's an end of utterance label,  */
 /*   i.e. enclosed in parens with uttid the first token in the paren, */
 /*   followed by a space, followed by the string "END".  If the end   */
 /*   of utterancs is valid the function returns T, otherwise F is     */
 /*   returned.                                                        */
 /*   **** Added by Jon Fiscus                                         */
 /**********************************************************************/
static boolean is_end_utt_id(char *sx, char *uttid){
    char *s, *e;
    int idlen;
    if ((s=strchr(sx,'(')) == NULL)
	return(F);
    if ((e=strrchr(sx,')')) == NULL)
	return(F);

    /* printf("is_next_utt_id: %s - %s\n",sx,uttid); */
    /* check to make sure utterance id's are the same */
    idlen = strlen(uttid);

    /* printf("is_next_utt_id: %s %s %d\n",uttid,s,idlen-1); */
    if (strncasecmp(uttid,s,idlen-1) != 0)
	return(F);
    /* printf("is_next_utt_id: check 1\n"); */
    /* make sure there's a space after the id */
    if (*(s+idlen-1) != ' ')
	return(F);
    /* printf("is_next_utt_id: check 2\n"); */
    if (strncasecmp((s+idlen),"END",3) != 0)
	return(F);
    /* printf("is_next_utt_id: HIT\n"); */
    return(T);
}

 /**********************************************************************/
 /*                                                                    */
 /*    void get_next_mark_utt(fp,word_tokens,fname,end_of_file,perr);  */
 /*      FILE *fp, WTOKE_STR1 *word_tokens, char *fname,               */
 /*      boolean *end_of_file, int *perr                               */
 /*                                                                    */
 /*    Gets a list of time-marked word tokens from opened file fp,     */
 /*    called fname, and load them into the structure *word_tokens.    */
 /*    The function loads words until the end of the utterance is      */
 /*    found.  To mark an utterance, the utterance id is enclosed in   */
 /*    parens on a single line.  The utt is terminated by the utt id   */
 /*    and the string "END" enclosed in parens on a single line.       */
 /*   *perr = 0 means a.o.k.                                           */
 /*   error codes:                                                     */
 /*    1: invalid mispronunciation mark at start of file.              */
 /*   11: error in opening file.                                       */
 /*   13: word_tokens array overflow.                                  */
 /*                                                                    */
 /*   **** Added by Jon Fiscus                                         */
 /**********************************************************************/
 void get_next_mark_utt(FILE *fp, WTOKE_STR1 *word_tokens, char *fname, boolean *end_of_file, int *perr)
  {
/* data: */
   char *proc = "get_next_mark_utt";
   int i, j, n, iret;
   char sxx[LINE_LENGTH], *sx    = &sxx[0];
   char xtr[LINE_LENGTH], *xconv = &xtr[0];
   char xco[LINE_LENGTH], *xconf = &xco[0];
   char xxx[LINE_LENGTH], *xsp   = &xxx[0];
   char sidx[LINE_LENGTH],*side = &sidx[0];
   char s2x[LINE_LENGTH], *s2    = &s2x[0];   
   char s3x[LINE_LENGTH], *s3    = &s3x[0];
   char sut[LINE_LENGTH], *uttid = &sut[0];
   double xt1, xdur, end_found;
   char comment_char = ';';
   boolean in_overlap, in_comment, in_unsure, in_mispron, in_crosstalk;
   boolean in_alternate;
/* code: */
db_enter_msg(proc,0); /* debug only */
   n = 0;
   *perr = 0;

 /* and loop on contents */
   in_overlap   = F;
   in_comment   = F;
   in_unsure    = F;
   in_mispron   = F;
   in_crosstalk = F;
   in_alternate = F;
   end_found    = F;
   while (!end_found && (sx = fgets(sx,LINE_LENGTH,fp)) != NULL){
       if (*sx != comment_char){
	   if (is_utt_id(sx,uttid)){  /* this is an utterance identifier */
	       word_tokens->id = strdup(uttid);	  
           } else if (is_end_utt_id(sx,word_tokens->id)){  /* this is an utterance identifier */
	       end_found = T;
           } else {
	       n += 1;
	       *side = *xconv = *s2 = *s3 = *xsp = *xconf = '\0';
	       if ((iret = sscanf(sx,"%s %s %s %s %s %s",
				  xconv,side,s2,s3,xsp,xconf)) > 0 && iret <6){
		   if (iret == 4) 
		       iret = sscanf(sx,"%s %s %s %s",s2,s3,xsp,xconf);
		   else if (iret == 3)
		       iret = sscanf(sx,"%s %s %s",s2,s3,xsp);
	       }

	       if (iret != EOF)
		   {if (n >= MAX_WORDS_IN_FILE)
			{fprintf(stderr,"*ERR: too many words in file.\n");
			 fprintf(stderr," increase MAX_WORDS_IN_FILE & recompile.\n");
			 fprintf(stdout,"; *ERR: too many words in file.\n");
			 *perr = 13; goto RETURN;
		     }
			
		    if ((s2[0] == '&') && (s2[1] == '&')) /* bad time marks */
			{xt1 = atof(s2+2);
			 word_tokens->word[n].bad_marking = T;
		     }
		    else
			{xt1 = atof(s2);
			 word_tokens->word[n].bad_marking = F;
		     }
		    xdur = atof(s3);
		    if (start_overlap(xsp))   in_overlap   = T;
		    if (start_comment(xsp))   in_comment   = T;
		    if (start_unsure(xsp))    in_unsure    = T;
		    if (start_mispron(xsp))   in_mispron   = T;
		    if (start_crosstalk(xsp)) in_crosstalk = T;

		    word_tokens->word[n].turn          = strdup(side);
		    word_tokens->word[n].overlapped    = in_overlap;
		    word_tokens->word[n].comment       = in_comment;
		    word_tokens->word[n].unsure        = in_unsure;
		    word_tokens->word[n].mispronounced = in_mispron;
		    word_tokens->word[n].crosstalk     = in_crosstalk;

		    if (end_overlap(xsp))   in_overlap   = F;
		    if (end_comment(xsp))   in_comment   = F;
		    if (end_unsure(xsp))    in_unsure    = F;
		    if (end_mispron(xsp))   in_mispron   = F;
		    if (end_crosstalk(xsp)) in_crosstalk = F;
		    
		    if (mispron(xsp)) word_tokens->word[n].mispronounced = T;
		    if (strstr(xsp,"{mispronounced}") != NULL) /* prev word flag */
			{if (n > 1) word_tokens->word[n-1].mispronounced = T;
			else
			    {fprintf(stderr,"*ERR:%s:file '%s' has\n",proc,fname);
			     fprintf(stderr,"  mispronunciation mark at start.\n");
			     *perr = 1;
			 }  }
		    word_tokens->word[n].conv = strdup(xconv);

		    /* confidence is optional, therefore check the validity of */
		    /* turn and confidence */
		    word_tokens->word[n].confidence = (strcmp(xconf,"") == 0) ? 0.0 : atof(xconf);
		    word_tokens->word[n].t1   = xt1;
		    word_tokens->word[n].dur  = xdur;
		    word_tokens->word[n].sp   = strdup(xsp);
		    in_alternate = F;
		    if (n > 1){
			if (word_tokens->word[n-1].alternate == T &&
			    strcmp(word_tokens->word[n-1].sp,"<ALT_END>")!=0)
			    in_alternate = T;
		    }
		    if (strncmp(word_tokens->word[n].sp,"<ALT",4) == 0)
			in_alternate = T;
		    word_tokens->word[n].alternate = in_alternate;
		}
	       else n -= 1;
	   }
       }
   }
   if (sx == NULL)
       *end_of_file = T;

/* mark overlap indicated by following turn being starred */
   i = 2;
   while ((i <= n)&&(*(word_tokens->word[i].turn) != '*')) i++;
   if (i <= n)
     {xconv = strcpy(xconv,word_tokens->word[i-1].turn);
      for (j=i-1; ((j > 0)&&streq(word_tokens->word[j].turn,xconv)); j--);
        {word_tokens->word[j].overlapped = T;
     }  }
RETURN:
   word_tokens-> n = n;
 db_leave_msg(proc,0); /* debug only */
   return;
  } /* end of function "get_next_mark_utt" */


 /**********************************************************************/
 /*                                                                    */
 /*    void fill_mark_struct(fp,word_tokens,fname,end_of_file,perr);   */
 /*      FILE *fp, WTOKE_STR1 *word_tokens, char *fname,               */
 /*      boolean *end_of_file, int *perr                               */
 /*                                                                    */
 /*    Reads a list of time-marked word tokens from opened file fp,    */
 /*    called fname, and load them into the structure *word_tokens.    */
 /*    The function loads words until the word tokens structure is     */
 /*    filled.  If there is data already in the word tokens structure  */
 /*    it is first copied to the beginning of the structure before     */
 /*    data is read.                                                   */
 /*                                                                    */
 /*   *perr = 0 means a.o.k.                                           */
 /*   error codes:                                                     */
 /*    1: invalid mispronunciation mark at start of file.              */
 /*   11: error in opening file.                                       */
 /*   13: word_tokens array overflow.                                  */
 /*                                                                    */
 /*   **** Added by Jon Fiscus                                         */
 /**********************************************************************/
 void fill_mark_struct(FILE *fp, WTOKE_STR1 *word_tokens, char *fname, boolean *end_of_file, int *perr)
  {
/* data: */
   char *proc = "fill_mark_struct";
   int i, j, n=0, iret;
   char sxx[LINE_LENGTH], *sx    = &sxx[0];
   char xtr[LINE_LENGTH], *xconv = &xtr[0];
   char xco[LINE_LENGTH], *xconf = &xco[0];
   char xxx[LINE_LENGTH], *xsp   = &xxx[0];
   char sidx[LINE_LENGTH],*side = &sidx[0];
   char s2x[LINE_LENGTH], *s2    = &s2x[0];   
   char s3x[LINE_LENGTH], *s3    = &s3x[0];
   double xt1, xdur, end_found;
   char comment_char = ';';
   boolean in_overlap, in_comment, in_unsure, in_mispron, in_crosstalk;
   boolean in_alternate;
/* code: */
db_enter_msg(proc,0); /* debug only */


   /* if the data doesn't begin at 1, copy it down and go from there */
   if (word_tokens->s > 1){
       /* first free the already alloc'd data */
       for (i=1; i<word_tokens->s; i++){
	   free(word_tokens->word[i].turn);
	   free(word_tokens->word[i].conv);
	   free(word_tokens->word[i].sp);       
       }
       
       /* then copy down the residual data */
       /*  dump_word_tokens2(word_tokens,word_tokens->s,word_tokens->n); */
       for (i=word_tokens->s; i<=word_tokens->n; i++){
	   word_tokens->word[i-word_tokens->s + 1] = word_tokens->word[i];
       }
       word_tokens->n = word_tokens->n - word_tokens->s + 1;
       word_tokens->s=1;
   }

   n = word_tokens->n;
   *perr = 0;

 /* and loop on contents */
   in_overlap   = F;
   in_comment   = F;
   in_unsure    = F;
   in_mispron   = F;
   in_crosstalk = F;
   in_alternate = F;
   end_found    = F;
   while (n < MAX_WORDS_IN_FILE-1 && !end_found &&
	  (sx = fgets(sx,LINE_LENGTH,fp)) != NULL){
       if (*sx != comment_char){
	   n += 1;
	   *s3 = *s3 = *xconv = *xsp = *xconf = '\0';
	   if (0)
	       iret = sscanf(sx,"%s %s %s %s",xconv,s2,s3,xsp );
	   else
	       iret = sscanf(sx,"%s %s %s %s %s %s",xconv,side,s2,s3,xsp,xconf);
	   /*iret = sscanf(sx,"%s %s %s %s %s %s",s2,s3,xsp,xconv,xconf);*/
	   
	   if (iret != EOF)
	       {if (n >= MAX_WORDS_IN_FILE)
		    {fprintf(stderr,"*ERR: too many words in file.\n");
		     fprintf(stderr," increase MAX_WORDS_IN_FILE & recompile.\n");
		     fprintf(stdout,"; *ERR: too many words in file.\n");
		     *perr = 13; goto RETURN;
		 }
		if (strcmp(xconv,"") == 0) {
		    fprintf(stdout,"; *ERR: Conversation is empty '%s'.\n",sx);
		    *perr = 14; goto RETURN;
		}
		if (strcmp(side,"") == 0) {
		   fprintf(stdout,"; *ERR: Conversation side is empty '%s'.\n",
			   sx);
		   *perr = 15; goto RETURN;
	        }
		if (strcmp(s2,"") == 0) {
		    fprintf(stdout,"; *ERR: Start time is empty '%s'.\n",
			    sx);
		    *perr = 16; goto RETURN;
	        }
		if (strcmp(s3,"") == 0) {
		    fprintf(stdout,"; *ERR: Duration time is empty '%s'.\n",
			    sx);
		    *perr = 17; goto RETURN;
	        }
		if (strcmp(xsp,"") == 0) {
		    fprintf(stdout,"; *ERR: Word string is empty '%s'.\n",
			    sx);
		    *perr = 18; goto RETURN;
	        }

		if ((s2[0] == '&') && (s2[1] == '&')) /* bad time marks */
		    {xt1 = atof(s2+2);
		     word_tokens->word[n].bad_marking = T;
		 }
		else
		    {xt1 = atof(s2);
		     word_tokens->word[n].bad_marking = F;
		 }
		
		xdur = atof(s3);
		word_tokens->word[n].turn          = strdup(side);
		word_tokens->word[n].confidence    =
		    (strcmp(xconf,"") == 0) ? 0.0 : atof(xconf);
		word_tokens->word[n].t1   = xt1;
		word_tokens->word[n].dur  = xdur;
		word_tokens->word[n].sp   = strdup(xsp);
		word_tokens->word[n].conv = strdup(xconv);

#ifdef stuff		
		if (start_overlap(xsp))   in_overlap   = T;
		if (start_comment(xsp))   in_comment   = T;
		if (start_unsure(xsp))    in_unsure    = T;
		if (start_mispron(xsp))   in_mispron   = T;
		if (start_crosstalk(xsp)) in_crosstalk = T;

		word_tokens->word[n].overlapped    = in_overlap;
		word_tokens->word[n].comment       = in_comment;
		word_tokens->word[n].unsure        = in_unsure;
		word_tokens->word[n].mispronounced = in_mispron;
		word_tokens->word[n].crosstalk     = in_crosstalk;
		
		if (end_overlap(xsp))   in_overlap   = F;
		if (end_comment(xsp))   in_comment   = F;
		if (end_unsure(xsp))    in_unsure    = F;
		if (end_mispron(xsp))   in_mispron   = F;
		if (end_crosstalk(xsp)) in_crosstalk = F;
		
		if (mispron(xsp)) word_tokens->word[n].mispronounced = T;
		if (strstr(xsp,"{mispronounced}") != NULL) /* prev word flag */
		    {if (n > 1) word_tokens->word[n-1].mispronounced = T;
		    else
			{fprintf(stderr,"*ERR:%s:file '%s' has\n",proc,fname);
			 fprintf(stderr,"  mispronunciation mark at start.\n");
			 *perr = 1;
		     }  }
#else

		word_tokens->word[n].overlapped    = F;
		word_tokens->word[n].comment       = F;
		word_tokens->word[n].unsure        = F;
		word_tokens->word[n].mispronounced = F;
		word_tokens->word[n].crosstalk     = F;
		in_alternate = F;
		if (n > 1){
		    if (word_tokens->word[n-1].alternate == T &&
			strcmp(word_tokens->word[n-1].sp,"<ALT_END>")!=0)
			in_alternate = T;
		}
		if (strncmp(word_tokens->word[n].sp,"<ALT",4) == 0)
		    in_alternate = T;
		word_tokens->word[n].alternate = in_alternate;
#endif		
	    }
	   else n -= 1;
       }
   }
   if (sx == NULL)
       *end_of_file = T;
   
   /* mark overlap indicated by following turn being starred */
   i = 2;
   while ((i <= n)&&(*(word_tokens->word[i].turn) != '*')) i++;
   if (i <= n)
       {xconv = strcpy(xconv,word_tokens->word[i-1].turn);
	for (j=i-1; ((j > 0)&&streq(word_tokens->word[j].turn,xconv)); j--);
        {word_tokens->word[j].overlapped = T;
     }  }
 RETURN:
   word_tokens-> s = 1;
   word_tokens-> n = n;
   if (word_tokens->id == (char *)0)
       word_tokens->id = strdup("");
   db_leave_msg(proc,0); /* debug only */
   return;
} /* end of function "fill_mark_struct" */

