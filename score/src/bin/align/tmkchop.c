#include <phone/stdcenvf.h>
#include <score/scorelib.h>

#include "alifunc.h"

void dump_word_tokens2(WTOKE_STR1 *word_tokens, int start, int lim)
  {int i;
   printf("\n");
   printf(" DUMP OF WORD TOKENS (OCCURRENCES):\n");
   printf(" (Starting at word %d going to %d)",start,lim);
   printf("\n");
   printf(" No. of word tokens = %d     Id = %s\n",
	  word_tokens->n,word_tokens->id);
   printf("                             CON-  OVER  MIS  UN- CROS COM- \n");
   printf(" CONV  TURN   T1      DUR    FID   LAPD PRON SURE TALK MENT BAD ALT WORD\n");
   for (i=start; (i<=word_tokens->n)&&(i<=lim); i++)
    {printf("%4s  %1s  %9.2f  %6.2f  %6.2f  %s    %s    %s    %s    %s    %s   %s   %s\n",
       word_tokens->word[i].conv,
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
       bool_print(word_tokens->word[i].alternate),
       word_tokens->word[i].sp);
     }
   return;
  } /* end of function "dump_word_tokens2" */

void copy_WTOKE(WTOKE_STR1 *to_seg,WTOKE_STR1 *from_seg, int from_w, int to_w){
    int w;
    to_seg->s = 1;
    to_seg->n = to_w - from_w + 1;
    to_seg->id = strdup(from_seg->id);
    for (w=from_w; w<=to_w; w++){
	/* copy the data, then strdup the pointers */
	to_seg->word[w-from_w +1] = from_seg->word[w];
	to_seg->word[w-from_w +1].conv = mtrf_strdup(from_seg->word[w].conv);
	to_seg->word[w-from_w +1].turn = mtrf_strdup(from_seg->word[w].turn);
	to_seg->word[w-from_w +1].sp = mtrf_strdup(from_seg->word[w].sp);
    }
}

void locate_boundary(WTOKE_STR1 *seg, int start, int by_conv, int by_turn, int *end){
    int w;
    int limit=0;
    int tchg, cchg;

    if (start == seg->n){
	*end = start;
	return;
    }
    for (w=start; w<=seg->n && limit == 0; w++){
	tchg = (!by_conv) ? TRUE : 
	    (strcmp(seg->word[start].conv,seg->word[w].conv) == 0);
 	cchg = (!by_turn) ? TRUE : 
	    (strcmp(seg->word[start].turn,seg->word[w].turn) == 0);
	if (!(tchg && cchg))
	    limit = w-1;
    }
    if (limit == 0)
	limit = seg->n;
    *end = limit;
}

/*******************************************************************/
/*  Compute the overlap of two segments in time and return that    */
/*  time.  Negative times indicate no overlap.  There are 6 cases  */
/*  Case 1:    S1   t1  t2                                         */
/*             S2            t1  t2                                */
/*                                                                 */
/*  Case 2:    S1            t1  t2                                */
/*             S2   t1  t2                                         */
/*                                                                 */
/*  Case 3a:   S1   t1    t2                                       */
/*             S2      t1    t2                                    */
/*                                                                 */
/*  Case 3b:   S1   t1           t2                                */
/*             S2      t1    t2                                    */
/*                                                                 */
/*  Case 4a:   S1         t1    t2                                 */
/*             S2      t1           t2                             */
/*                                                                 */
/*  Case 4b:   S1         t1        t2                             */
/*             S2      t1       t2                                 */
/*                                                                 */
double overlap(double s1_t1, double s1_t2, double s2_t1, double s2_t2)
{ 
    double rval;
    char *rule;
    int dbg=0;
    /* Case 1: */
    if (s1_t2 < s2_t1)
	rule="Case 1", rval=(s1_t2 - s2_t1);
    /* Case 2: */
    if (s1_t1 > s2_t2)
	rule="Case 2", rval=(s2_t2 - s1_t1);
    /* Case 3: */
    if (s1_t1 < s2_t1){
	/* Case 3a: */
	if (s1_t2 < s2_t2)
	    rule="Case 3a", rval=(s1_t2 - s2_t1);
	else /* Case 3b: */
	    rule="Case 3b", rval=(s2_t2 - s2_t1);
    } else {  /* Case 4: */
	/* Case 4a: */
	if (s1_t2 < s2_t2)
	    rule="Case 4a", rval=(s1_t2 - s1_t1);
	else /* Case 4b: */
	    rule="Case 4b", rval=(s2_t2 - s1_t1);
    }
    if (dbg){
	printf("Overlap: s1_t1=%2.2f s1_t2=%2.2f",s1_t1,s1_t2);
	printf(" s2_t1=%2.2f s2_t2=%2.2f   Rule:%s  Rval=%2.2f\n",
	       s2_t1,s2_t2,rule,rval);
    }
    return(rval);
}

void reset_WTOKE_flag(WTOKE_STR1 *seg,char *flag_name)
{
    int w;

    if (strcmp(flag_name,"overlapped") == 0){
	for (w=1; w<seg->n; w++)
	    seg->word[w].overlapped = F;
    }
}

int chop_WTOKE_2(WTOKE_STR1 *ref, WTOKE_STR1 *hyp, int Rstart, int Hstart, int Rendconv, int Hendconv, int max_words, int *Rret_end, int *Hret_end)
{
    int Rend, Hend, Rend_sync, Hend_sync;
    char *proc="chop_WTOKE_2";
    int dbg=0, skipped_alt;
    double Rgap_t1, Rgap_t2, Hgap_t1, Hgap_t2;
    
    if (dbg)
	printf("%s:  Start Ref=%d  Start Hyp=%d  Max_words=%d\n",
	       proc,Rstart,Hstart,max_words);

    /* Find the end of this segment */
    /*     locate_boundary(ref, Rstart, TRUE, TRUE, &Rendconv);
	   locate_boundary(hyp, Hstart, TRUE, TRUE, &Hendconv); */

    if (dbg) printf("%s:  Conversation Rendconv=%d  Hendconv=%d\n",
		    proc,Rendconv,Hendconv);

    if ((Rstart > Rendconv) && (Hstart > Hendconv))
	return(0);

    /* if we've hit the end of the conversation, send everything */
    if (((Rendconv - Rstart) <max_words) && ((Hendconv - Hstart) < max_words)){
	*Rret_end = Rendconv; 
	*Hret_end = Hendconv; 
	if (dbg) printf("%s: R1 Success Chop Ref (%d-%d)  Hyp (%d-%d)\n",
			proc,Rstart,*Rret_end,Hstart,*Hret_end);
	return(1);
    }

    /* set the indexes to maximum length */
    Rend = MIN(Rstart + max_words, Rendconv);
    Hend = MIN(Hstart + max_words, Hendconv);
	
    if (dbg) printf("%s:  Limited Rend=%d  Hend=%d\n",proc,Rend,Hend);

    /* synchronize the ends to permit searching */
    while (Rend >= Rstart && ref->word[Rend].alternate)
	Rend--;
    skipped_alt = TRUE;
    while (skipped_alt){
	skipped_alt = FALSE;
	if (ref->word[Rend].t1 > hyp->word[Hend].t1)
	    while (!skipped_alt &&
		   Rend >= Rstart && 
		   ref->word[Rend].t1 > hyp->word[Hend].t1 && 
		   overlap(ref->word[Rend].t1,
			   ref->word[Rend].t1+ref->word[Rend].dur,
			   hyp->word[Hend].t1,
			   hyp->word[Hend].t1+hyp->word[Hend].dur) < 0.0)
		for (Rend--; Rend >= Rstart && ref->word[Rend].alternate; ) {
		    Rend--;
		    skipped_alt = TRUE;
		}
	else if (ref->word[Rend].t1 < hyp->word[Hend].t1)
	    while (Hend >= Hstart &&
		   ref->word[Rend].t1 < hyp->word[Hend].t1 && 
		   overlap(ref->word[Rend].t1,
			   ref->word[Rend].t1+ref->word[Rend].dur,
			   hyp->word[Hend].t1,
			   hyp->word[Hend].t1+hyp->word[Hend].dur) < 0.0)
		Hend --;
    }
    if (dbg) {
	printf("%s:  Synchronized Rend=%d  Hend=%d  ",proc,Rend,Hend);
	printf("Times: %2.2f,%2.2f   %2.2f,%2.2f\n",
	       ref->word[Rend].t1, ref->word[Rend].t1+ref->word[Rend].dur,
	       hyp->word[Hend].t1, hyp->word[Hend].t1+hyp->word[Hend].dur);
    }
    Rend_sync = Rend;
    Hend_sync = Hend;

    /* begin the backward search */
    while (Rend > Rstart && Hend > Hstart){
	/* first compute t1 and t2 for the gap in each segment */
	if (Rend >= Rstart){
	    Rgap_t1 = ref->word[Rend].t1 + ref->word[Rend].dur;
	    Rgap_t2 = (Rend == Rendconv) ? 999999.99 : ref->word[Rend+1].t1;
	} else {
	    Rgap_t1 = 0.0;
	    Rgap_t2 = ref->word[Rstart].t1;
	}
	if (Hend >= Hstart) {
	    Hgap_t1 = hyp->word[Hend].t1 + hyp->word[Hend].dur;
	    Hgap_t2 = (Hend == Hendconv) ? 999999.99 : hyp->word[Hend+1].t1;
	} else {
	    Hgap_t1 = 0.0;
	    Hgap_t2 = hyp->word[Hstart].t1;
	}

	if (dbg) {
	    printf("\n%s:  Gaps: ref %2.2f,%2.2f  hyp %2.2f,%2.2f\n",
		   proc,Rgap_t1, Rgap_t2, Hgap_t1, Hgap_t2);
	    printf("%s:  **** middle word is Rend or Hend\n",proc);
	    dump_word_tokens2(ref,Rend-1,Rend+1);
	    dump_word_tokens2(hyp,Hend-1,Hend+1);
	}

	/* check to see if there is any overlap */
	/* yes if :    S1   |   |         #          |  |    */
	/*             S2          |   |  #  |   |           */
	   
	if (overlap(Rgap_t1,Rgap_t2,Hgap_t1,Hgap_t2) < 0.0){
	    /* no overlap decide which to move */
	
	    if (dbg) printf("%s: Shifting down\n",proc);

	    skipped_alt = TRUE;
	    while (skipped_alt){
		skipped_alt = FALSE;
		if (ref->word[Rend].t1 > hyp->word[Hend].t1){
		    if (Rend > Rstart) {
			Rend --;
			while (Rend >= Rstart && ref->word[Rend].alternate) {
			    Rend--;
			    skipped_alt = TRUE;
			}
		    }
		    if (dbg) printf("%s: Moving Ref\n",proc);
		    while (!skipped_alt &&
			   Hend >= Hstart && 
			   ref->word[Rend].t1 < hyp->word[Hend].t1 &&
			   overlap(ref->word[Rend].t1,
				   ref->word[Rend].t1+ref->word[Rend].dur,
				   hyp->word[Hend].t1,
				 hyp->word[Hend].t1+hyp->word[Hend].dur)< 0.0){
			Hend --;
			if (dbg) printf("%s: Adjusting Hyp\n",proc);
		    }
		} else if (ref->word[Rend].t1 <= hyp->word[Hend].t1) {
		    if (Hend > Hstart) 
			Hend --;
		    if (dbg) printf("%s: Moving Hyp\n",proc);
		    while (!skipped_alt &&
			   Rend >= Rstart &&
			   ref->word[Rend].t1 > hyp->word[Hend].t1 && 
			   overlap(ref->word[Rend].t1,
				   ref->word[Rend].t1+ref->word[Rend].dur,
				   hyp->word[Hend].t1,
				  hyp->word[Hend].t1+ref->word[Hend].dur)<0.0){
			for (Rend--;
			     Rend >= Rstart && ref->word[Rend].alternate; ) {
			    Rend--;
			    skipped_alt = TRUE;
			}
			if (dbg) printf("%s: Adjusting Ref\n",proc);
		    }
		}
	    }
	} else { /* Overlap exists !!!!!   Return the values */
	    *Rret_end = Rend; 
	    *Hret_end = Hend; 
	    if (dbg) printf("%s: R2 Success Chop Ref (%d-%d)  Hyp (%d-%d)\n",
			    proc,Rstart,*Rret_end,Hstart,*Hret_end);
	    return(1);
	}
    }

    if (Rend > Rstart || Hend > Hstart){
	*Rret_end = Rend; 
	*Hret_end = Hend; 
	if (dbg) printf("%s: R3 Success Chop Ref (%d-%d)  Hyp (%d-%d)\n",
			proc,Rstart,*Rret_end,Hstart,*Hret_end);
	return(1);
    }
    /* fprintf(stdout,"%s: Failure to find chop'd segment, returning 0\n",proc); */
    return(0);
}
