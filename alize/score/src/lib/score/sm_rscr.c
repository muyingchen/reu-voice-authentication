
/**********************************************************************/
/*                                                                    */
/*             FILENAME:  sm_rscr.c                                   */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>


float time_wod PROTO((float t_r, float d_r, float t_h, float d_h));


void rescore_align_for_splits_and_merges(TABLE_OF_CODESETS *pcs, SYS_ALIGN *align_str, double fom_thresh, int v)
{
    int spkr, snt;

    for (spkr=0; spkr < num_spkr(align_str); spkr++){
        for (snt=0; snt < num_sents(align_str,spkr); snt++){
	    rescore_SENT_for_splits_and_merges(pcs,sent_n(align_str,spkr,snt),
					       fom_thresh, v,
			  (has_ATTRIB(sys_attrib(align_str),ATTRIB_TIMEMRK)));
	} /* end of the sentence for(;;) */
    } /* end of spkr for(;;) */
}

void rescore_SENT_for_splits_and_merges(TABLE_OF_CODESETS *pcs, SENT *sent, double fom_thresh, int v, int has_time_marks)
{
    int wrd, last_wrd=(-1), err, newind, changes, largest_fom_wrd;
    double fom, largest_fom;
    char check, lorr, sorm, largest_fom_sorm, largest_fom_lorr;

    err = FALSE;
    changes = 1;
/*     dump_SENT_wrds(pcs,sent); */
    while (changes > 0) {
	changes = 0;
	largest_fom = (-1.0);
	largest_fom_sorm = largest_fom_lorr = '\0';
	last_wrd = s_eval_wrd(sent,0);
	for (wrd=1; s_ref_wrd(sent,wrd)!=END_OF_WORDS; wrd++){
	    fom = 0.0; check = 'n';
	    /* now check for splits and merges */
	    if (is_SUB(s_eval_wrd(sent,wrd)) ||
		is_MERGE(s_eval_wrd(sent,wrd)) || 
		is_SPLIT(s_eval_wrd(sent,wrd))) {
		if (is_DEL(last_wrd)){
		    check = 'y'; lorr = 'r'; sorm = 'm';
		} else if (is_INS(last_wrd)) {
		    check = 'y'; lorr = 'r'; sorm = 's';
		}
	    }
	    else if (is_DEL(s_eval_wrd(sent,wrd))) {
		if (is_SUB(last_wrd) || is_MERGE(last_wrd) || 
		    is_SPLIT(last_wrd)) {
		    check = 'y'; lorr = 'l'; sorm = 'm';
		}
	    }
	    else if (is_INS(s_eval_wrd(sent,wrd))) {
		if (is_SUB(last_wrd) || is_MERGE(last_wrd) || 
		    is_SPLIT(last_wrd)) {
		    check = 'y'; lorr = 'l'; sorm = 's';
		}
	    }
	    if (check == 'y'){
		if (! has_time_marks){
		    fom = compute_sm_fom(pcs,lorr,sorm,
					 s_ref_wrd(sent,wrd-1),
					 s_ref_wrd(sent,wrd),
					 s_hyp_wrd(sent,wrd-1),
					 s_hyp_wrd(sent,wrd));
		} else {
		    fom = compute_time_sm_fom(pcs,lorr,sorm,sent,wrd-1,wrd);
		}
		if (v) {
		    printf("FOM Comp %5.3f (%c-%c)  %s %s ->  %s %s\n",fom,
			   lorr,sorm,
			   (s_ref_wrd(sent,wrd-1) < 0) ? "***" :
			   pcs->cd[1].pc->pc[s_ref_wrd(sent,wrd-1)].ac,
			   (s_ref_wrd(sent,wrd) < 0) ? "***" :
			   pcs->cd[1].pc->pc[s_ref_wrd(sent,wrd)].ac,
			   (s_hyp_wrd(sent,wrd-1) < 0) ? "***" :
			   pcs->cd[1].pc->pc[s_hyp_wrd(sent,wrd-1)].ac,
			   (s_hyp_wrd(sent,wrd) < 0) ? "***" :
			   pcs->cd[1].pc->pc[s_hyp_wrd(sent,wrd)].ac);
		    if (has_time_marks){
			printf("             Begin   %5.2f %5.2f ->  %5.2f %5.2f\n",
			       s_ref_beg_wrd(sent,wrd-1),
			       s_ref_beg_wrd(sent,wrd),
			       s_hyp_beg_wrd(sent,wrd-1),
			       s_hyp_beg_wrd(sent,wrd));
			printf("             Dura    %5.2f %5.2f ->  %5.2f %5.2f\n",
			       s_ref_dur_wrd(sent,wrd-1),
			       s_ref_dur_wrd(sent,wrd),
			       s_hyp_dur_wrd(sent,wrd-1),
			       s_hyp_dur_wrd(sent,wrd));
		    }
		}
		if (fom > fom_thresh){
		    if (fom > largest_fom){
			largest_fom = fom;
			largest_fom_wrd  = wrd;
			largest_fom_sorm = sorm;
			largest_fom_lorr = lorr;
		    }
		}
	    }
	    last_wrd = s_eval_wrd(sent,wrd);
	}
	/* now if we have 1 large enough fom, change it and continue */
	if (largest_fom > fom_thresh) {
	    wrd = largest_fom_wrd;
	    sorm = largest_fom_sorm;
	    lorr = largest_fom_lorr;	
	    if (v) 
		printf("Using FOM computation: wrd=%d sorr=%c lorr=%c\n",
		       wrd,sorm,lorr);
	    if (sorm == 's')
		newind = add_merge_to_lexicon(pcs,
					      s_hyp_wrd(sent,wrd-1),
					      s_hyp_wrd(sent,wrd));
	    else
		newind = add_merge_to_lexicon(pcs,
					      s_ref_wrd(sent,wrd-1),
					      s_ref_wrd(sent,wrd));
	    /* remove condense the split or merge */
	    /* first, condense the s/m */
	    if (sorm == 's'){
		if (lorr == 'r'){
		    s_ref_wrd(sent,wrd-1) = s_ref_wrd(sent,wrd);
		    s_ref_beg_wrd(sent,wrd-1) = s_ref_beg_wrd(sent,wrd);
		    s_ref_dur_wrd(sent,wrd-1) = s_ref_dur_wrd(sent,wrd);
		}
		s_hyp_wrd(sent,wrd-1) = newind;
		s_eval_wrd(sent,wrd-1) = EVAL_SPT;
		s_hyp_dur_wrd(sent,wrd-1) = (s_hyp_beg_wrd(sent,wrd) +
					     s_hyp_dur_wrd(sent,wrd)) - 
						 s_hyp_beg_wrd(sent,wrd-1);
	    } else {
		if (lorr == 'r'){
		    s_hyp_wrd(sent,wrd-1) = s_hyp_wrd(sent,wrd);
		    s_hyp_beg_wrd(sent,wrd-1) = s_hyp_beg_wrd(sent,wrd);
		    s_hyp_dur_wrd(sent,wrd-1) = s_hyp_dur_wrd(sent,wrd);
		}
		s_ref_wrd(sent,wrd-1) = newind;
		s_eval_wrd(sent,wrd-1) = EVAL_MRG;
		s_ref_dur_wrd(sent,wrd-1) = (s_ref_beg_wrd(sent,wrd) +
					     s_ref_dur_wrd(sent,wrd))- 
						 s_ref_beg_wrd(sent,wrd-1);
	    }
	    /* now slide the words down to end-of-sentence */
	    do {
		wrd++;
		s_hyp_wrd(sent,wrd-1) = s_hyp_wrd(sent,wrd);
		s_ref_wrd(sent,wrd-1) = s_ref_wrd(sent,wrd);
		s_eval_wrd(sent,wrd-1) = s_eval_wrd(sent,wrd);
		s_hyp_beg_wrd(sent,wrd-1) = s_hyp_beg_wrd(sent,wrd);
		s_hyp_dur_wrd(sent,wrd-1) = s_hyp_dur_wrd(sent,wrd);
		s_ref_beg_wrd(sent,wrd-1) = s_ref_beg_wrd(sent,wrd);
		s_ref_dur_wrd(sent,wrd-1) = s_ref_dur_wrd(sent,wrd);
	    } while (s_ref_wrd(sent,wrd)!=END_OF_WORDS);
	    changes = 1;
	}
    }
}

float time_wod(float t_r, float d_r, float t_h, float d_h){
    float T2_r, T2_h, wod;
    if (t_r < 0.0) /* it's an insertion */
	wod = d_h;
    else if (t_h < 0.0) /* it's a deletion */
	wod = d_r;
    else {
	T2_r = t_r + d_r;
	T2_h = t_h + d_h;
	wod = fabs(t_r - t_h) + fabs(T2_r - T2_h);
    }
    if (0) 
	printf("    Time_wod:  %f %f - %f %f   WOD %f\n",t_r,d_r,t_h,d_h,wod);
    return(wod);
}

double compute_time_sm_fom(TABLE_OF_CODESETS *pcs, char lorr, char sorm, SENT *sent, int ind1, int ind2){
    float ina1_t, ina2_t, inb1_t, inb2_t;
    float ina1_d, ina2_d, inb1_d, inb2_d;
    float dist_sub, dist_ins, dist_mrg;
    double fom;

    ina1_t = s_ref_beg_wrd(sent,ind1);
    ina2_t = s_ref_beg_wrd(sent,ind2);
    inb1_t = s_hyp_beg_wrd(sent,ind1);
    inb2_t = s_hyp_beg_wrd(sent,ind2);
    ina1_d = s_ref_dur_wrd(sent,ind1);
    ina2_d = s_ref_dur_wrd(sent,ind2);
    inb1_d = s_hyp_dur_wrd(sent,ind1);
    inb2_d = s_hyp_dur_wrd(sent,ind2);

/*                   sorm=s       sorm=m     */
/*   lorr==l      R: W1  **    R: W1  W2     */
/*                   H1  H2       H1  **     */
/*                                           */
/*   lorr==r      R: **  W2    R: W1  W2     */
/*                   H1  H2       **  H2     */

    if (sorm == 's'){
	if (lorr == 'l'){
	    dist_sub = time_wod(ina1_t, ina1_d, inb1_t, inb1_d);
	    dist_ins = time_wod(-1.0,   -1.0,   inb2_t, inb2_d);
	    dist_mrg = time_wod(ina1_t, ina1_d, inb1_t, (inb2_t + inb2_d)-inb1_t);
	} else {
	    dist_sub = time_wod(ina2_t, ina2_d, inb2_t, inb2_d);
	    dist_ins = time_wod(-1.0,   -1.0,   inb1_t, inb1_d);
	    dist_mrg = time_wod(ina2_t, ina2_d, inb1_t, (inb2_t + inb2_d)-inb1_t);
	}
    } else {
	if (lorr == 'l'){
	    dist_sub = time_wod(ina1_t, ina1_d,                   inb1_t, inb1_d);
	    dist_ins = time_wod(ina2_t, ina2_d,                   -1.0,    -1.0);
	    dist_mrg = time_wod(ina1_t, (ina2_t + ina2_d)-ina1_t, inb1_t, inb1_d);
	} else {
	    dist_sub = time_wod(ina2_t, ina2_d,                   inb2_t, inb2_d);
	    dist_ins = time_wod(ina1_t, ina1_d,                   -1.0,   -1.0);
	    dist_mrg = time_wod(ina1_t, (ina2_t + ina2_d)-ina1_t, inb2_t, inb2_d);
	}
    }
    fom = (dist_sub + dist_ins) / dist_mrg;
    if (0) printf("  D(Sub)=%f  D(Ins/Del)=%f  D(S/M)=%f   FOM=%f\n",
		  dist_sub,dist_ins,dist_mrg,fom);
    return(fom);
}


int add_merge_to_lexicon(TABLE_OF_CODESETS *pcs, WCHAR_T i1, WCHAR_T i2)
{
    int ind, perror=0, *perr=&perror;

    if ((ind = pcindex5(rsprintf("%s=%s",pcs->cd[1].pc->pc[i1].ac,
				pcs->cd[1].pc->pc[i2].ac),
		       pcs->cd[1].pc,0,0)) != -1){
	return(ind);
    }
    if ((ind = pcindex5(rsprintf("%s=%s",pcs->cd[1].pc->pc[i1].ac,
			    pcs->cd[1].pc->pc[i2].ac),
		   pcs->cd[1].pc,1,0)) == (-1)){
	fprintf(stderr,"Error: Unable to add merge %s into the core pcode\n",
		pcs->cd[1].pc->pc[ind].ac);
	exit(-1);
    }
/*    printf("Adding %s\n",pcs->cd[1].pc->pc[ind].ac);
    printf("New item id:%d   str:%s   lc:%s\n",
	   ind,pcs->cd[1].pc->pc[i1].ac,pcs->cd[1].pc->pc[i1].lc_ext);
    printf("New item id:%d   str:%s   lc:%s\n",
	   ind,pcs->cd[1].pc->pc[i2].ac,pcs->cd[1].pc->pc[i2].lc_ext);
    printf("New item id:%d   str:%s   lc:%s\n",
	   ind,pcs->cd[1].pc->pc[ind].ac,pcs->cd[1].pc->pc[ind].lc_ext); */
    free(pcs->cd[1].pc->pc[ind].lc_ext);
    pcs->cd[1].pc->pc[ind].lc_ext = 
	mtrf_malloc(sizeof(char) * (strlen(pcs->cd[1].pc->pc[i1].lc_ext) + 
			       strlen(pcs->cd[1].pc->pc[i2].lc_ext) + 2));
    strcpy(pcs->cd[1].pc->pc[ind].lc_ext,pcs->cd[1].pc->pc[i1].lc_ext);
    strcat(pcs->cd[1].pc->pc[ind].lc_ext," ");
    strcat(pcs->cd[1].pc->pc[ind].lc_ext,pcs->cd[1].pc->pc[i2].lc_ext);
    
    free(pcs->cd[1].pc->pc[ind].lc_int);
    pcs->cd[1].pc->pc[ind].lc_int = (WCHAR_T *)
	mtrf_malloc(sizeof(WCHAR_T) * (pcs->cd[1].pc->pc[i1].lc_int[0] + 
				  pcs->cd[1].pc->pc[i2].lc_int[0] + 2));
    
    nstrcpy(pcs->cd[1].pc->pc[ind].lc_int,pcs->cd[1].pc->pc[i1].lc_int);
    nstrcat(pcs->cd[1].pc->pc[ind].lc_int,pcs->cd[1].pc->pc[i2].lc_int);
    
/*    printf("New item id:%d   str:%s   lc:%s\n",
	   ind,pcs->cd[1].pc->pc[ind].ac,pcs->cd[1].pc->pc[ind].lc_ext);  */
    
    report_lex_add2(ind,pcs->cd[1].pc,perr);
    if (*perr != 0) {
	fprintf(stderr,"Error: Unable to add Merge %s into the .ADD file\n",
		pcs->cd[1].pc->pc[ind].ac);
	exit(-1);
    }
    return(ind);
}

/***  Returns the number of words combined into a split or merge ***/
int count_words_in_spilts_merges(char *str)
{
    char *p=str;
    int sum=1;
    /* printf("Count_sms  %s",str); */
    while ((p=(char *)strchr(p,'=')) != (char *)NULL)
        sum++,  p++;
    /* printf("  %d\n",sum); */
    return(sum);
}

