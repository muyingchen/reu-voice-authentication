/**********************************************************************/
/*                                                                    */
/*       FILENAME:  frg_rscr.c                                   */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This function re-scores a system alignment file   */
/*                  forgiving substitution errors that involve        */
/*                  a fragment and a word if their they are spelled   */
/*                  the same up to the hyphen in the fragment.        */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvf.h>
#include <score/scorelib.h>

static int is_fragment PROTO((char *str));

void rescore_align_for_fragments(TABLE_OF_CODESETS *pcs, SYS_ALIGN *align_str, int dbg)
{
    int spkr, snt, wrd;
    SENT *sent;
    char *refstr, *hypstr;
    int lenref, lenhyp;

    for (spkr=0; spkr < num_spkr(align_str); spkr++){
        for (snt=0; snt < num_sents(align_str,spkr); snt++){
            /* use the sentence address to speed things up */
            sent = sent_n(align_str,spkr,snt);
            for (wrd=0; s_ref_wrd(sent,wrd)!=END_OF_WORDS; wrd++){
		
		if (dbg) printf("  Checking words %s %s\n",
			   (s_ref_wrd(sent,wrd) < 0) ? "***" :
			       pcs->cd[1].pc->pc[s_ref_wrd(sent,wrd)].ac,
			   (s_hyp_wrd(sent,wrd) < 0) ? "***" :
                               pcs->cd[1].pc->pc[s_hyp_wrd(sent,wrd)].ac);
		if (is_SUB(s_eval_wrd(sent,wrd)) &&
		    is_fragment(pcs->cd[1].pc->pc[s_ref_wrd(sent,wrd)].ac)){
		    if (dbg) printf("     fragment\n");
		    refstr = pcs->cd[1].pc->pc[s_ref_wrd(sent,wrd)].ac;
		    hypstr = pcs->cd[1].pc->pc[s_hyp_wrd(sent,wrd)].ac;
		    lenref = strlen(refstr);
		    lenhyp = strlen(hypstr);
		    /* check the fragments that have an ending */
		    if (*refstr == HYPHEN){
			if (lenhyp >= lenref-1){
			    if (strncmp(refstr+1,hypstr + lenhyp - (lenref-1),
					lenref-1) == 0){
				if (dbg) printf("    Forgiving error\n");
				s_eval_wrd(sent,wrd) = EVAL_CORR;
			    }
			}			
		    } else { /* check fragments that have beginning */
			if (lenhyp >= lenref-1){
			    if (strncmp(refstr,hypstr,lenref-1) == 0){
				if (dbg) printf("    Forgiving error\n");
				s_eval_wrd(sent,wrd) = EVAL_CORR;
			    }
			}			
		    }
		}
	    }
	} /* end of the sentence for(;;) */
    } /* end of spkr for(;;) */
}

static int is_fragment(char *str){
    if ((*str == HYPHEN) || (*(str + strlen(str) - 1) == HYPHEN))
	return(TRUE);
    return(FALSE);
}

