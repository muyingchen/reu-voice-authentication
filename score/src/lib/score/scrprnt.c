/**********************************************************************/
/*                                                                    */
/*             filename:  scrprnt.c                                   */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This is procedure scores an ALIGNMENT STRUCTURE   */
/*                  in order to do so, there are TONS of counters     */
/*                  necessary to keep track of all the statistics     */
/*                  for scoring.                                      */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/*******************************************************************/
/* define the threshold for the minimum number of occurences       */
/* to print for lists of del, ins, subs and confusion pairs        */
#define SPKR_THRESH   1
#define DTL_THRESH    2

/* added by Brett to compute mean, variance, and standard dev */
#define NUM_SPKRS       100
/* defines the initial size of the word list arrarys for each speaker */
#define WLISTN		200

int spont_ver = TRUE;

/* scrprnt.c */		FILE *make_filename PROTO((char *dir, char *name, char *ext));
/* scrprnt.c */		SCORE *alloc_SCORE PROTO((int n_spkr, int lex_size));
/* scrprnt.c */		void free_SCORE PROTO((SCORE *score));
/* scrprnt.c */		void cp_spkrs_to_ovr_dtl PROTO((SCORE *score, int n_spkr, int ovr_dtl, int lex_size));
/* scrprnt.c */		void make_sys_root_name PROTO((char **sys_root_name, char *ali_fname));
/* scrprnt.c */		void print_conf_pairs PROTO((SCORE *score, int spkr, TABLE_OF_CODESETS *pcs, FILE *fp, char *label, int thresh, int num_elem, PCIND_T *elem_cnt, PCIND_T **elem_list));
/* scrprnt.c */		void print_overall_desc PROTO((SCORE *score, SYS_ALIGN *align_str, char *output_dir, char *sys_root_name));
/* scrprnt.c */		void print_overall_desc PROTO((SCORE *score, SYS_ALIGN *align_str, char *output_dir, char *sys_root_name));
/* scrprnt.c */		void print_overall_raw_desc PROTO((SCORE *score, SYS_ALIGN *align_str, char *output_dir, char *sys_root_name));
/* scrprnt.c */		void print_overall_raw_desc PROTO((SCORE *score, SYS_ALIGN *align_str, char *output_dir, char *sys_root_name));
/* scrprnt.c */		void print_sent_desc PROTO((SCORE *score, SENT *sent, SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, char *spkr_str, int spkr, FILE *fp));
/* scrprnt.c */		void print_spkr_desc PROTO((SCORE *score, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, SM_CAND *sm_cand, char *spkr_str, int spkr, char *output_dir, char *ext, char *label, int thresh, char *s_desc, int spkr_report));
/* scrprnt.c */		void print_system_header PROTO((FILE *fp, char *name, char *desc));
/* scrprnt.c */		void print_wrd_cnt_arr_of_lex PROTO((PCIND_T *ins_del, TABLE_OF_CODESETS *pcs, FILE *fp, char *label, int thresh));
/* scrprnt.c */		void report_lexicon_subsets_percents PROTO((SCORE *score, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int spkr, FILE *fp, char *title));
/* scrprnt.c */		void report_lexicon_subsets_raw PROTO((SCORE *score, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int spkr, FILE *fp));
/* scrprnt.c */		void store_sub_in_list PROTO((SCORE *score, int spkr, PCIND_T ref_ind, PCIND_T hyp_ind, int max_subs, PCIND_T increment, int *max_elem, int *num_elem, PCIND_T **elem_cnt_arr, PCIND_T ***elem_list_arr));
/* scrprnt.c */		void update_subset_counts PROTO((int l_set, int *arr));
/* scrprnt.c */		void add_to_wordlist PROTO((int *n, int *max, PCIND_T ***arr, PCIND_T ind, int count));
/* scrprnt.c */		void print_wrd_cnt_arr_of_lex2 PROTO((int n, int max, PCIND_T **arr, TABLE_OF_CODESETS *pcs, FILE *fp, char *label, int thresh));

/**********************************************************************/
/*  this function allocate memory for a SCORE structure, and          */
/*  initializes all the variables to zero.                            */
/**********************************************************************/
SCORE *alloc_SCORE(int n_spkr, int lex_size)
{
    SCORE *score;
    register int _i, _j; 
    alloc_singarr(score,1,SCORE); 
    score->n_of_spk = n_spkr; 
    alloc_singarr(score->spk,n_spkr,SPKR_SCORE *); 
    for (_i=0; _i < score->n_of_spk; _i++){ 
	alloc_singarr(score->spk[_i],1,SPKR_SCORE); 
	score->spk[_i]->n_ref = 0; 
	score->spk[_i]->n_hyp = 0; 
	for (_j=0; _j<20; _j++) 
	    score->spk[_i]->n_hyp_lsub[_j] = 0;  
	score->spk[_i]->n_corr = 0; 
	for (_j=0; _j<20; _j++) 
	    score->spk[_i]->n_corr_lsub[_j] = 0;  
	score->spk[_i]->n_subs = 0; 
	score->spk[_i]->n_subs_alphnum = 0; 
	score->spk[_i]->n_subs_mono = 0; 
	for (_j=0; _j<20; _j++) 
	    score->spk[_i]->n_subs_lsub[_j] = 0; 
	score->spk[_i]->n_ins = 0; 
	score->spk[_i]->n_ins_alphnum = 0; 
	score->spk[_i]->n_ins_mono = 0; 
	for (_j=0; _j<20; _j++) 
	    score->spk[_i]->n_ins_lsub[_j] = 0; 
	score->spk[_i]->n_del = 0; 
	score->spk[_i]->n_del_THE = 0; 
	score->spk[_i]->n_del_alphnum = 0; 
	score->spk[_i]->n_del_mono = 0; 
	for (_j=0; _j<20; _j++) 
	    score->spk[_i]->n_del_lsub[_j] = 0; 
	score->spk[_i]->n_align_words = 0; 
	score->spk[_i]->n_ref_mono_syl = 0; 
	score->spk[_i]->n_accept_merge = 0; 
	score->spk[_i]->n_merge = 0; 
	score->spk[_i]->n_accept_split = 0; 
	score->spk[_i]->n_split = 0; 
	score->spk[_i]->n_sent = 0; 
	score->spk[_i]->n_sent_errors = 0; 
	score->spk[_i]->n_sent_sub = 0; 
	score->spk[_i]->n_sent_sub_alpha = 0; 
	score->spk[_i]->n_sent_sub_mono = 0; 
	score->spk[_i]->n_sent_del = 0; 
	score->spk[_i]->n_sent_del_THE = 0; 
	score->spk[_i]->n_sent_del_THE_only = 0; 
	score->spk[_i]->n_sent_del_alpha = 0; 
	score->spk[_i]->n_sent_del_mono = 0; 
	score->spk[_i]->n_sent_ins = 0; 
	score->spk[_i]->n_sent_ins_alpha = 0; 
	score->spk[_i]->n_sent_ins_mono = 0; 
	score->spk[_i]->n_sent_spl = 0; 
	score->spk[_i]->n_sent_mrg = 0; 
	for (_j=0; _j<20; _j++) {
	    score->spk[_i]->n_sent_sub_lsub[_j] = 0; 
	    score->spk[_i]->n_sent_del_lsub[_j] = 0; 
	    score->spk[_i]->n_sent_ins_lsub[_j] = 0; 
	} 
	score->spk[_i]->n_in_sub_arr = 0; 
	score->spk[_i]->conf_max_pairs = 500; 
#ifdef NEW_SM_METHOD
	sp_n_split_arr(_i) = 0; 
	sp_n_merge_arr(_i) = 0; 
	sp_max_split_pr(_i) = 200; 
	sp_max_merge_pr(_i) = 200; 
	alloc_2dimZ(sp_split_arr(_i),200,2,PCIND_T,0); 
	alloc_singZ(sp_split_cnt_arr(_i),200,PCIND_T,0); 
	alloc_2dimZ(sp_merge_arr(_i),200,2,PCIND_T,0); 
	alloc_singZ(sp_merge_cnt_arr(_i),200,PCIND_T,0); 
#endif
#ifdef OLD
	alloc_singZ(sp_del_arr(_i),lex_size,PCIND_T,0); 
	alloc_singZ(sp_ins_arr(_i),lex_size,PCIND_T,0); 
	alloc_singZ(sp_mis_rec_arr(_i),lex_size,PCIND_T,0); 
	alloc_singZ(sp_sub_arr(_i),lex_size,PCIND_T,0); 
#endif
	alloc_2dimZ(score->spk[_i]->conf_word_arr,500,2,PCIND_T,0); 
	alloc_singZ(score->spk[_i]->conf_count_arr,500,PCIND_T,0); 


	score->spk[_i]->n_del_word = 0;
	score->spk[_i]->max_del_word = WLISTN;
	alloc_2dimZ(score->spk[_i]->del_word_arr,2,WLISTN,PCIND_T,0); 

	score->spk[_i]->n_ins_word = 0;
	score->spk[_i]->max_ins_word = WLISTN;
	alloc_2dimZ(score->spk[_i]->ins_word_arr,2,WLISTN,PCIND_T,0); 

	score->spk[_i]->n_misr_word = 0;
	score->spk[_i]->max_misr_word = WLISTN;
	alloc_2dimZ(score->spk[_i]->misr_word_arr,2,WLISTN,PCIND_T,0); 

	score->spk[_i]->n_sub_word = 0;
	score->spk[_i]->max_sub_word = WLISTN;
	alloc_2dimZ(score->spk[_i]->sub_word_arr,2,WLISTN,PCIND_T,0); 
    } 
    return(score);
}

/**********************************************************************/
/*  this function free's memory for a SCORE structure.                */
/**********************************************************************/
void free_SCORE(SCORE *score)
{
    register int _i; 

    for (_i=0; _i < score->n_of_spk; _i++){ 
	free_2dimarr(score->spk[_i]->conf_word_arr,500,PCIND_T); 
	free_singarr(score->spk[_i]->conf_count_arr,PCIND_T); 
#ifdef NEW_SM_METHOD
	free_2dimarr(sp_split_arr(_i),200,PCIND_T); 
	free_singarr(sp_split_cnt_arr(_i),PCIND_T); 
	free_2dimarr(sp_merge_arr(_i),200,PCIND_T); 
	free_singarr(sp_merge_cnt_arr(_i),PCIND_T); 
#endif

	free_2dimarr(score->spk[_i]->del_word_arr,2,PCIND_T); 
	free_2dimarr(score->spk[_i]->ins_word_arr,2,PCIND_T); 
	free_2dimarr(score->spk[_i]->misr_word_arr,2,PCIND_T); 
	free_2dimarr(score->spk[_i]->sub_word_arr,2,PCIND_T); 
	free_singarr(score->spk[_i],SPKR_SCORE); 
    } 

    free_singarr(score->spk,SPKR_SCORE *); 
    free_singarr(score,SCORE); 
}


/**********************************************************************/
/*  this function scores and count all the errors for every sentence  */
/*  int a SYS_ALIGN structure, plus it has the ability to print       */
/*  reports for the sentence, speaker and system levels               */
/*  Fix: 921211:                                                      */
/*       the memory for the string "THE" must be on the stack, so it  */ 
/*       can be modified by Bill's codeset code                       */
/**********************************************************************/
void print_score_SYS_ALIGN(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, ALPHA_NUM *alpha_num, MONO_SYL *mono_syl, SPLT_MRG *sp_mrg, int overall_dtl, int overall, int overall_raw, int spkr_lvl, int sent_lvl, char *output_dir, char *ali_fname)
/* flags for report levels */
{
    SCORE *score;
    int spkr, snt, wrd, THE_index=0, err, last_wrd = EVAL_CORR, ovr_dtl;
    int beg_spkr_sm=0, l_set, set;
    SENT *sent;
    char *spkr_str, *sys_root_name;
    char the_str[10];
    SM_CAND sm_cand;
    FILE *fp_sent, *make_filename(char *dir, char *name, char *ext);


    /*  the last position in the speaker list "ovr_dtl" will hold the */
    /*  overall statistics for the entire system                      */
    ovr_dtl = num_spkr(align_str);
    score = alloc_SCORE(num_spkr(align_str)+1,lex_num(pcs));

    strcpy(the_str,"THE");
    THE_index = is_LEXICON(pcs,the_str); /* find the lex index for "THE"*/

    /* if it's possible open the file */
    make_sys_root_name(&sys_root_name,ali_fname);

    /***** reset the splt and merge list */
    init_SM_CAND((&sm_cand));
    for (spkr=0; spkr < num_spkr(align_str); spkr++){
        spkr_str = spkr_name(align_str,spkr);
        /***** if sent level, try to open it's file if requested */
        if (sent_lvl){
            fp_sent = make_filename(output_dir,spkr_str,"snt");
            print_system_header(fp_sent,sys_name(align_str),
                                        sys_desc(align_str));
	}

        base_cand((&sm_cand)) = beg_spkr_sm = num_cand((&sm_cand));
        for (snt=0; snt < num_sents(align_str,spkr); snt++){
            /* reset the sentence counter variales */
            score->snt.s_ref     = score->snt.s_hyp     = score->snt.s_corr = 0; 
            score->snt.s_ins     = score->snt.s_ins_alphnum  = score->snt.s_ins_mono = 0; 
            score->snt.s_sub     = score->snt.s_sub_alphnum  = score->snt.s_sub_mono = 0; 
            score->snt.s_del     = score->snt.s_del_alphnum  = score->snt.s_del_mono = 0; 
            score->snt.s_accept_merge = score->snt.s_merge     = 0; 
            score->snt.s_accept_split = score->snt.s_split     = 0; 
            score->snt.s_align_words = score->snt.s_ref_mono_syl = 0; 
            score->snt.s_del_THE = 0;
            for (set=0; set<20; set++)
                score->snt.s_hyp_lsub[set] = score->snt.s_corr_lsub[set] = score->snt.s_ins_lsub[set] =
                score->snt.s_sub_lsub[set] = score->snt.s_del_lsub[set] = 0;

            /* the candidate list is kept for the all sentences of a speaker*/
            /* so mark the beginning of this sentences splits and merges    */
            base_cand((&sm_cand)) = num_cand((&sm_cand));
            err = FALSE;
            /* use the sentence address to speed things up */
            sent = sent_n(align_str,spkr,snt);
            for (wrd=0; s_ref_wrd(sent,wrd)!=END_OF_WORDS; wrd++){
                l_set = eval_set(s_eval_wrd(sent,wrd)); /* what is the set flag*/
                if (is_CORR(s_eval_wrd(sent,wrd))) {
                    /*  if the word is correct */
                    score->snt.s_corr++, score->snt.s_ref++, score->snt.s_hyp++;
                    if (l_set >= 0) { /* it's part of a set */
                        update_subset_counts(l_set,score->snt.s_corr_lsub);
                        update_subset_counts(l_set,score->snt.s_hyp_lsub);
                    }
                }
                else if (is_INS(s_eval_wrd(sent,wrd))) {
                    /*  if the hyp word was inserted */
                    score->snt.s_hyp++, score->snt.s_ins++, err = TRUE;
		    add_to_wordlist(&(score->spk[spkr]->n_ins_word),
				    &(score->spk[spkr]->max_ins_word),
				    &(score->spk[spkr]->ins_word_arr),
				    s_hyp_wrd(sent,wrd),1);
                    if (is_ALPHA_NUM(alpha_num,s_hyp_wrd(sent,wrd)))
                        score->snt.s_ins_alphnum++;
                    if (is_MONO_SYL(mono_syl,s_hyp_wrd(sent,wrd)))
                        score->snt.s_ins_mono++;
                    if (l_set >= 0){  /* it's part of a set */
                        update_subset_counts(l_set,score->snt.s_ins_lsub);
                        update_subset_counts(l_set,score->snt.s_hyp_lsub);
                    }
                }
                else if (is_DEL(s_eval_wrd(sent,wrd))) {
                    /*  if the hyp word was deleted */
                    score->snt.s_ref++, score->snt.s_del++, err = TRUE;
		    add_to_wordlist(&(score->spk[spkr]->n_del_word),
				    &(score->spk[spkr]->max_del_word),
				    &(score->spk[spkr]->del_word_arr),
				    s_ref_wrd(sent,wrd),1);

                    if (s_ref_wrd(sent,wrd) == THE_index)
                         score->snt.s_del_THE++;
                    if (is_ALPHA_NUM(alpha_num,s_ref_wrd(sent,wrd)))
                        score->snt.s_del_alphnum++;
                    if (is_MONO_SYL(mono_syl,s_ref_wrd(sent,wrd)))
                        score->snt.s_del_mono++;
                    if (l_set >= 0)  /* it's part of a set */
                        update_subset_counts(l_set,score->snt.s_del_lsub);
                }
                else if (is_SUB(s_eval_wrd(sent,wrd))) {
                    /*  the words could be substituted */
                    store_sub_in_list(score,spkr,
                                      s_ref_wrd(sent,wrd),
                                      s_hyp_wrd(sent,wrd),
				      score->spk[spkr]->conf_max_pairs, 1,
				      &(score->spk[spkr]->conf_max_pairs), 
				      &(score->spk[spkr]->n_in_sub_arr),
				      &(score->spk[spkr]->conf_count_arr),
				      &(score->spk[spkr]->conf_word_arr));
                    score->snt.s_sub++, score->snt.s_ref++;
                    score->snt.s_hyp++, err = TRUE;
		    add_to_wordlist(&(score->spk[spkr]->n_misr_word),
				    &(score->spk[spkr]->max_misr_word),
				    &(score->spk[spkr]->misr_word_arr),
				    s_ref_wrd(sent,wrd),1);
		    add_to_wordlist(&(score->spk[spkr]->n_sub_word),
				    &(score->spk[spkr]->max_sub_word),
				    &(score->spk[spkr]->sub_word_arr),
				    s_hyp_wrd(sent,wrd),1);
		    
                    if (is_ALPHA_NUM(alpha_num,s_ref_wrd(sent,wrd)))
                        score->snt.s_sub_alphnum++;
                    if (is_MONO_SYL(mono_syl,s_ref_wrd(sent,wrd)))
                        score->snt.s_sub_mono++;
                    if (l_set >= 0)  /* it's part of a set */
                        update_subset_counts(l_set,score->snt.s_sub_lsub);
                    { int tmp; /* check to see if the substituted word is*/
                               /* part of a subset */
                      if ((tmp = is_LEX_SUBSET(lsub,s_hyp_wrd(sent,wrd))) !=
                           NOT_IN_SUBSET)
                         update_subset_counts(tmp,score->snt.s_hyp_lsub);
                    }
		}
#ifdef NEW_SM_METHOD
                else if (is_SPLIT(s_eval_wrd(sent,wrd))) {
		    int nsw =  count_words_in_spilts_merges(
  			          pcs->cd[1].pc->pc[s_hyp_wrd(sent,wrd)].ac);
		    int ns=0;
		    PCIND_T sl[10];
		    char *str, *pstr;
		    float fom1, fom2;
                    st_spl()++, st_ref()++; st_hyp()+=nsw; err = TRUE;
		    /* find the pcindexes that make up the split */
		    str=(char *)mtrf_strdup(pcs->cd[1].pc->pc[s_hyp_wrd(sent,wrd)].ac);
		    if (0) printf("Split %s -> %s \n",str,
			   pcs->cd[1].pc->pc[s_ref_wrd(sent,wrd)].ac);
		    pstr = strtok(str,"=");
		    while (pstr != NULL){
			sl[ns++] = pcindex2(pstr,pcs->cd[1].pc);
			if (0) printf("Split found %s = %d\n",pstr,sl[ns-1]);
			pstr = strtok(NULL,"=");
		    }
		    mtrf_free(str);
		    fom1 = compute_sm_fom(pcs,'l','m', sl[0], sl[1],
					  s_ref_wrd(sent,wrd),0);
		    fom2 = compute_sm_fom(pcs,'r','m', sl[0], sl[1],
					  0,s_ref_wrd(sent,wrd));
		    if (0) printf("Fig of Merit: %f %f\n",fom1,fom2);
		    is_SPLT(sp_mrg,&sm_cand,pcs,
			    s_ref_wrd(sent,wrd), sl[0], sl[1],
			    (fom1 > fom2) ? 'l' : 'r');

                    store_sub_in_list(score,spkr,
                                      s_ref_wrd(sent,wrd),
                                      s_hyp_wrd(sent,wrd),
				      sp_max_split_pr(spkr), 1,
				      &(sp_max_split_pr(spkr)), 
				      &(sp_n_split_arr(spkr)),
				      &(sp_split_cnt_arr(spkr)),
				      &(sp_split_arr(spkr)));
		}
                else if (is_MERGE(s_eval_wrd(sent,wrd))) {
		    int nmw = count_words_in_spilts_merges(
			     pcs->cd[1].pc->pc[s_ref_wrd(sent,wrd)].ac);
		    int nm=0;
		    PCIND_T ml[10];
		    char *str, *pstr;
		    float fom1, fom2;
                    st_mrg()++; st_ref()+=nmw; st_hyp()++, err = TRUE;
		    /* find the pcindexes that make up the merges */
		    str=(char *)mtrf_strdup(pcs->cd[1].pc->pc[s_ref_wrd(sent,wrd)].ac);
		    if (0) printf("Merge %s -> %s \n",str,
			   pcs->cd[1].pc->pc[s_hyp_wrd(sent,wrd)].ac);
		    pstr = strtok(str,"=");
		    while (pstr != NULL){
			ml[nm++] = pcindex2(pstr,pcs->cd[1].pc);
			if (0) printf("Merge found %s = %d\n",pstr,ml[nm-1]);
			pstr = strtok(NULL,"=");
		    }
		    mtrf_free(str);
		    fom1 = compute_sm_fom(pcs,'l','m', ml[0], ml[1],
					  s_hyp_wrd(sent,wrd),0);
		    fom2 = compute_sm_fom(pcs,'r','m', ml[0], ml[1],
					  0,s_hyp_wrd(sent,wrd));
		    if (0) printf("Fig of Merit: %f %f\n",fom1,fom2);
		    is_MRG(sp_mrg,&sm_cand,pcs,
			   s_hyp_wrd(sent,wrd), ml[0], ml[1],
			   (fom1 > fom2) ? 'l' : 'r');
                    store_sub_in_list(score,spkr,
                                      s_ref_wrd(sent,wrd),
                                      s_hyp_wrd(sent,wrd),
				      sp_max_merge_pr(spkr), 1,
				      &(sp_max_merge_pr(spkr)), 
				      &(sp_n_merge_arr(spkr)),
				      &(sp_merge_cnt_arr(spkr)),
				      &(sp_merge_arr(spkr)));
		}
#endif

                else {
                    fprintf(stderr,"Error: unknown evaluation flag %x\n",
                            s_eval_wrd(sent,wrd));
                    exit(-1);
                }

#ifdef OLD_SM_METHOD
                /* now check for splits and merges */
                if (wrd > 0){
                    if (is_SUB(s_eval_wrd(sent,wrd))) {
                        if (is_DEL(last_wrd)){
			    if (0) dump_SENT_wrds(pcs,sent);
                            if (is_MRG(sp_mrg,&sm_cand,pcs,
				       s_hyp_wrd(sent,wrd),
				       s_ref_wrd(sent,wrd-1),
				       s_ref_wrd(sent,wrd), 'r'))
                               score->snt.s_accept_merge++;
                            else 
                               score->snt.s_merge++;
                        } else if (is_INS(last_wrd)) {
			    if (0) dump_SENT_wrds(pcs,sent);
			    if (is_SPLT(sp_mrg,&sm_cand,pcs,
					s_ref_wrd(sent,wrd),
					s_hyp_wrd(sent,wrd-1),
					s_hyp_wrd(sent,wrd),'r'))
				score->snt.s_accept_split++;
			    else 
				score->snt.s_split++;
			}
                    }
                    else if (is_DEL(s_eval_wrd(sent,wrd))) {
                        if (is_SUB(last_wrd)){
			    if (0) dump_SENT_wrds(pcs,sent);
                            if (is_MRG(sp_mrg,&sm_cand,pcs,
				       s_hyp_wrd(sent,wrd-1),
				       s_ref_wrd(sent,wrd-1),
				       s_ref_wrd(sent,wrd),'l'))
                                score->snt.s_accept_merge++;
                            else
                                score->snt.s_merge++;
			}
                    }
                    else if (is_INS(s_eval_wrd(sent,wrd))) {
                        if (is_SUB(last_wrd)){
			    if (0) dump_SENT_wrds(pcs,sent);
                            if (is_SPLT(sp_mrg,&sm_cand,pcs,
					s_ref_wrd(sent,wrd-1),
					s_hyp_wrd(sent,wrd-1),
					s_hyp_wrd(sent,wrd),'l'))
                                score->snt.s_accept_split++;
                            else
                                score->snt.s_split++;
			}
		    }
		}
#endif
                last_wrd = s_eval_wrd(sent,wrd);
                score->snt.s_align_words++;
                if (is_MONO_SYL(mono_syl,s_ref_wrd(sent,wrd)) == IS_MONO_SYL)
                     score->snt.s_ref_mono_syl++;
                
	    }
            /* increment all speaker word counters for this sentence */
            score->spk[spkr]->n_ref     += score->snt.s_ref;
            score->spk[spkr]->n_hyp     += score->snt.s_hyp;
            score->spk[spkr]->n_corr    += score->snt.s_corr;
            score->spk[spkr]->n_subs     += score->snt.s_sub;
            score->spk[spkr]->n_subs_alphnum  += score->snt.s_sub_alphnum;
            score->spk[spkr]->n_subs_mono  += score->snt.s_sub_mono;
            score->spk[spkr]->n_ins     += score->snt.s_ins;
            score->spk[spkr]->n_ins_alphnum  += score->snt.s_ins_alphnum;
            score->spk[spkr]->n_ins_mono  += score->snt.s_ins_mono;
            score->spk[spkr]->n_del     += score->snt.s_del;
            score->spk[spkr]->n_del_THE += score->snt.s_del_THE;
            score->spk[spkr]->n_del_alphnum  += score->snt.s_del_alphnum;
            score->spk[spkr]->n_del_mono  += score->snt.s_del_mono;
            for (set=0; set<20; set++){
                score->spk[spkr]->n_hyp_lsub[set]    += score->snt.s_hyp_lsub[set];
                score->spk[spkr]->n_corr_lsub[set]   += score->snt.s_corr_lsub[set];
                score->spk[spkr]->n_subs_lsub[set]    += score->snt.s_sub_lsub[set];
                score->spk[spkr]->n_ins_lsub[set]    += score->snt.s_ins_lsub[set];
                score->spk[spkr]->n_del_lsub[set]    += score->snt.s_del_lsub[set];
            }
            score->spk[spkr]->n_align_words += score->snt.s_align_words;
            score->spk[spkr]->n_ref_mono_syl += score->snt.s_ref_mono_syl;
            score->spk[spkr]->n_accept_merge += score->snt.s_accept_merge;
            score->spk[spkr]->n_merge     += score->snt.s_merge;
            score->spk[spkr]->n_accept_split += score->snt.s_accept_split;
            score->spk[spkr]->n_split     += score->snt.s_split;
            score->spk[spkr]->n_sent++;
            /* increment all speaker sentence counters for this sentence */
            if (err)			score->spk[spkr]->n_sent_errors++;
            if (score->snt.s_sub    > 0)	score->spk[spkr]->n_sent_sub++;
            if (score->snt.s_sub_alphnum > 0)	score->spk[spkr]->n_sent_sub_alpha++;
            if (score->snt.s_sub_mono > 0)	score->spk[spkr]->n_sent_sub_mono++;
            if (score->snt.s_del    > 0)	score->spk[spkr]->n_sent_del++;
            if (score->snt.s_del_THE> 0)	score->spk[spkr]->n_sent_del_THE++;
            if (score->snt.s_del_alphnum > 0)	score->spk[spkr]->n_sent_del_alpha++;
            if (score->snt.s_del_mono > 0)	score->spk[spkr]->n_sent_del_mono++;
            if (score->snt.s_ins    > 0)	score->spk[spkr]->n_sent_ins++;
            if (score->snt.s_ins_alphnum > 0)	score->spk[spkr]->n_sent_ins_alpha++;
            if (score->snt.s_ins_mono > 0)	score->spk[spkr]->n_sent_ins_mono++;
            if (score->snt.s_merge    > 0)	score->spk[spkr]->n_sent_mrg++;
            if (score->snt.s_split    > 0)	score->spk[spkr]->n_sent_spl++;
            for (set=0; set<20; set++){
                if (score->snt.s_sub_lsub[set] > 0)	score->spk[spkr]->n_sent_sub_lsub[set]++;
                if (score->snt.s_del_lsub[set] > 0)	score->spk[spkr]->n_sent_del_lsub[set]++;
                if (score->snt.s_ins_lsub[set] > 0)	score->spk[spkr]->n_sent_ins_lsub[set]++;
            }
            if ((score->snt.s_del_THE > 0) && 
                ((score->snt.s_sub + score->snt.s_del + score->snt.s_ins == 0)))
                 score->spk[spkr]->n_sent_del_THE_only++;
            /* should i print the sentence level report */
            if (sent_lvl)
               print_sent_desc(score,sent,&sm_cand,pcs,lsub,spkr_str,spkr,
                               fp_sent);
	} /* end of the sentence for(;;) */
        if (sent_lvl)
            if (fp_sent != stdout)
                fclose(fp_sent);
        if (spkr_lvl) {
             base_cand((&sm_cand)) = beg_spkr_sm;
             print_spkr_desc(score,pcs,lsub,&sm_cand,spkr_str,spkr,output_dir,
                             SPEAKER_SUMMARY_EXT,"SCORING FOR SPEAKER:",
                             SPKR_THRESH,sys_desc(align_str),TRUE);
	}
    } /* end of spkr for(;;) */
    if (overall) print_overall_desc(score,align_str,output_dir,sys_root_name);

    if (overall_raw) print_overall_raw_desc(score,align_str,output_dir,
                                            sys_root_name);

    if (overall_dtl){
        /* copy all the speaker counts to the overall detailed report */
        cp_spkrs_to_ovr_dtl(score,num_spkr(align_str),ovr_dtl,lex_num(pcs));
        base_cand((&sm_cand)) = 0;
        sort_SM_CAND(&sm_cand);
        print_spkr_desc(score,pcs,lsub,&sm_cand,sys_root_name,ovr_dtl,
             output_dir,"sys_dtl",
             "DETAILED OVERALL REPORT FOR THE SYSTEM",2,"",FALSE);
    }
    free_SCORE(score);
    mtrf_free(sys_root_name);
    free_SM_CAND((&sm_cand));
} /*** end of print_score_SYS_ALIGN() ***/

void update_subset_counts(int l_set, int *arr)
{
    int count=0;
    do{ if ((l_set & 0x0001) > 0)
            arr[count]++;
        l_set >>= 1;
        count++;
    } while (l_set > 0);
}

/************************************************************************/
/*   in order to print a detailed summary report for the entire system  */
/*   copy the current speakers                                          */
/************************************************************************/
void cp_spkrs_to_ovr_dtl(SCORE *score, int n_spkr, int ovr_dtl, int lex_size)
{
    int c_spk, cf_pr, set, wl;

    for (c_spk=0; c_spk<n_spkr; c_spk++){
	score->spk[ovr_dtl]->n_ref          += score->spk[c_spk]->n_ref; 
	score->spk[ovr_dtl]->n_hyp          += score->spk[c_spk]->n_hyp; 
	score->spk[ovr_dtl]->n_corr         += score->spk[c_spk]->n_corr; 
	score->spk[ovr_dtl]->n_subs          += score->spk[c_spk]->n_subs; 
	score->spk[ovr_dtl]->n_subs_alphnum       += score->spk[c_spk]->n_subs_alphnum; 
	score->spk[ovr_dtl]->n_subs_mono       += score->spk[c_spk]->n_subs_mono; 
	score->spk[ovr_dtl]->n_ins          += score->spk[c_spk]->n_ins; 
	score->spk[ovr_dtl]->n_ins_alphnum       += score->spk[c_spk]->n_ins_alphnum; 
	score->spk[ovr_dtl]->n_ins_mono       += score->spk[c_spk]->n_ins_mono; 
	score->spk[ovr_dtl]->n_del          += score->spk[c_spk]->n_del; 
	score->spk[ovr_dtl]->n_del_THE      += score->spk[c_spk]->n_del_THE; 
	score->spk[ovr_dtl]->n_del_alphnum       += score->spk[c_spk]->n_del_alphnum; 
	score->spk[ovr_dtl]->n_del_mono       += score->spk[c_spk]->n_del_mono; 
	score->spk[ovr_dtl]->n_align_words      += score->spk[c_spk]->n_align_words; 
	score->spk[ovr_dtl]->n_ref_mono_syl      += score->spk[c_spk]->n_ref_mono_syl; 
	score->spk[ovr_dtl]->n_accept_merge      += score->spk[c_spk]->n_accept_merge; 
	score->spk[ovr_dtl]->n_merge          += score->spk[c_spk]->n_merge; 
	score->spk[ovr_dtl]->n_accept_split      += score->spk[c_spk]->n_accept_split; 
	score->spk[ovr_dtl]->n_split          += score->spk[c_spk]->n_split; 
	score->spk[ovr_dtl]->n_sent           += score->spk[c_spk]->n_sent; 
	score->spk[ovr_dtl]->n_sent_errors        += score->spk[c_spk]->n_sent_errors; 
	score->spk[ovr_dtl]->n_sent_sub       += score->spk[c_spk]->n_sent_sub; 
	score->spk[ovr_dtl]->n_sent_sub_alpha    += score->spk[c_spk]->n_sent_sub_alpha; 
	score->spk[ovr_dtl]->n_sent_sub_mono    += score->spk[c_spk]->n_sent_sub_mono; 
	score->spk[ovr_dtl]->n_sent_del       += score->spk[c_spk]->n_sent_del; 
	score->spk[ovr_dtl]->n_sent_del_THE   += score->spk[c_spk]->n_sent_del_THE; 
	score->spk[ovr_dtl]->n_sent_del_THE_only += score->spk[c_spk]->n_sent_del_THE_only; 
	score->spk[ovr_dtl]->n_sent_del_alpha    += score->spk[c_spk]->n_sent_del_alpha; 
	score->spk[ovr_dtl]->n_sent_del_mono    += score->spk[c_spk]->n_sent_del_mono; 
	score->spk[ovr_dtl]->n_sent_ins       += score->spk[c_spk]->n_sent_ins; 
	score->spk[ovr_dtl]->n_sent_ins_alpha    += score->spk[c_spk]->n_sent_ins_alpha; 
	score->spk[ovr_dtl]->n_sent_spl       += score->spk[c_spk]->n_sent_spl; 
	score->spk[ovr_dtl]->n_sent_mrg       += score->spk[c_spk]->n_sent_mrg; 
	for (set=0; set<20; set++){
	    score->spk[ovr_dtl]->n_hyp_lsub[set]    += score->spk[c_spk]->n_hyp_lsub[set];
	    score->spk[ovr_dtl]->n_corr_lsub[set]   += score->spk[c_spk]->n_corr_lsub[set];
	    score->spk[ovr_dtl]->n_subs_lsub[set]    += score->spk[c_spk]->n_subs_lsub[set];
	    score->spk[ovr_dtl]->n_ins_lsub[set]    += score->spk[c_spk]->n_ins_lsub[set];
	    score->spk[ovr_dtl]->n_del_lsub[set]    += score->spk[c_spk]->n_del_lsub[set];
	    /* sentence level stats */
	    score->spk[ovr_dtl]->n_sent_sub_lsub[set]    += score->spk[c_spk]->n_sent_sub_lsub[set];
	    score->spk[ovr_dtl]->n_sent_ins_lsub[set]    += score->spk[c_spk]->n_sent_ins_lsub[set];
	    score->spk[ovr_dtl]->n_sent_del_lsub[set]    += score->spk[c_spk]->n_sent_del_lsub[set];
	}
	
	for (cf_pr=0; cf_pr<score->spk[c_spk]->n_in_sub_arr; cf_pr++)
	    store_sub_in_list(score,ovr_dtl,score->spk[c_spk]->conf_word_arr[cf_pr][0],
			      score->spk[c_spk]->conf_word_arr[cf_pr][1],
			      score->spk[c_spk]->conf_max_pairs,
			      score->spk[c_spk]->conf_count_arr[cf_pr],
			      &(score->spk[ovr_dtl]->conf_max_pairs), 
			      &(score->spk[ovr_dtl]->n_in_sub_arr),
			      &(score->spk[ovr_dtl]->conf_count_arr), 
			      &(score->spk[ovr_dtl]->conf_word_arr));
	

#ifdef NEW_SM_METHOD
	for (cf_pr=0; cf_pr<sp_n_split_arr(c_spk); cf_pr++)
	    store_sub_in_list(score,ovr_dtl,sp_split_ref(c_spk,cf_pr),
			      sp_split_hyp(c_spk,cf_pr),
			      sp_max_split_pr(c_spk),
			      sp_split_cnt(c_spk,cf_pr),
			      &(sp_max_split_pr(ovr_dtl)), 
			      &(sp_n_split_arr(ovr_dtl)),
			      &(sp_split_cnt_arr(ovr_dtl)), 
			      &(sp_split_arr(ovr_dtl)));
	
	for (cf_pr=0; cf_pr<sp_n_merge_arr(c_spk); cf_pr++)
	    store_sub_in_list(score,ovr_dtl,sp_merge_ref(c_spk,cf_pr),
			      sp_merge_hyp(c_spk,cf_pr),
			      sp_max_split_pr(c_spk),
			      sp_merge_cnt(c_spk,cf_pr),
			      &(sp_max_merge_pr(ovr_dtl)), 
			      &(sp_n_merge_arr(ovr_dtl)),
			      &(sp_merge_cnt_arr(ovr_dtl)), 
			      &(sp_merge_arr(ovr_dtl)));
#endif
	
	for (wl=0; wl<score->spk[c_spk]->n_ins_word; wl++)
	    add_to_wordlist(&(score->spk[ovr_dtl]->n_ins_word),
			    &(score->spk[ovr_dtl]->max_ins_word),
			    &(score->spk[ovr_dtl]->ins_word_arr),
			    score->spk[c_spk]->ins_word_arr[0][wl],
			    score->spk[c_spk]->ins_word_arr[1][wl]);
	for (wl=0; wl<score->spk[c_spk]->n_del_word; wl++)
	    add_to_wordlist(&(score->spk[ovr_dtl]->n_del_word),
			    &(score->spk[ovr_dtl]->max_del_word),
			    &(score->spk[ovr_dtl]->del_word_arr),
			    score->spk[c_spk]->del_word_arr[0][wl],
			    score->spk[c_spk]->del_word_arr[1][wl]);
	for (wl=0; wl<score->spk[c_spk]->n_misr_word; wl++)
	    add_to_wordlist(&(score->spk[ovr_dtl]->n_misr_word),
			    &(score->spk[ovr_dtl]->max_misr_word),
			    &(score->spk[ovr_dtl]->misr_word_arr),
			    score->spk[c_spk]->misr_word_arr[0][wl],
			    score->spk[c_spk]->misr_word_arr[1][wl]);
	for (wl=0; wl<score->spk[c_spk]->n_sub_word; wl++)
	    add_to_wordlist(&(score->spk[ovr_dtl]->n_sub_word),
			    &(score->spk[ovr_dtl]->max_sub_word),
			    &(score->spk[ovr_dtl]->sub_word_arr),
			    score->spk[c_spk]->sub_word_arr[0][wl],
			    score->spk[c_spk]->sub_word_arr[1][wl]);
    }
}

/************************************************************************/
/*   This routine takes a ref and a substituted hypothesis word and     */
/*   inserts them  into the list marking out all of the confusion pairs */
/************************************************************************/
void store_sub_in_list(SCORE *score, int spkr, PCIND_T ref_ind, PCIND_T hyp_ind, int max_subs, PCIND_T increment, int *max_elem, int *num_elem, PCIND_T **elem_cnt_arr, PCIND_T ***elem_list_arr)
{
    int i;
    PCIND_T *elem_cnt = *elem_cnt_arr, **elem_list = *elem_list_arr;

    for (i=0;((i < *max_elem) && (i< *num_elem)); i++)
        /* if the two words where confused before, only increment the count*/
        if ((elem_list[i][0] == ref_ind) &&
            (elem_list[i][1] == hyp_ind)){
            elem_cnt[i] += increment;
            return;
	}

    /* check the bounds of the arr */
    if (i == *max_elem){
         PCIND_T **tconf_words, *tconf_count;
         int new_max, index;
         new_max = (int)(i*1.5);
         /* fprintf(stderr,"Warning: Confusion array too small,");
	    fprintf(stderr," expanding to %d\n",new_max); */

         /* alloc the new space */
         alloc_2dimZ(tconf_words,new_max,2,PCIND_T,0);
         alloc_singZ(tconf_count,new_max,PCIND_T,0);

         /* load in the previously recorded information */
         for (index=0; index<*num_elem; index++){
             tconf_words[index][0] = elem_list[index][0];
             tconf_words[index][1] = elem_list[index][1];
             tconf_count[index] = elem_cnt[index];
	 }

         /* free the old memory and update the new */
         free_2dimarr((*elem_list_arr),*max_elem,PCIND_T);
         free_singarr((*elem_cnt_arr),PCIND_T);
         *elem_list_arr = elem_list = tconf_words;
         *elem_cnt_arr = elem_cnt = tconf_count;
         *max_elem = new_max;
    }

    /* otherwise, insert the words into the confusion pair list */
    elem_list[i][0] = ref_ind;
    elem_list[i][1] = hyp_ind;
    elem_cnt[i] = increment;
    (*num_elem)++;
}


#ifdef OLD_SM_METHOD
/************************************************************************/
/*   print the report for the entire system.                            */
/************************************************************************/
void print_overall_desc(SCORE *score, SYS_ALIGN *align_str, char *output_dir, char *sys_root_name)
{
    char pad[SCREEN_WIDTH],temp_char;
    int spkr, pl = 47, tot_corr=0, tot_sub=0, tot_del=0;
    int tot_ins=0, tot_ref =0, tot_st=0, tot_st_er=0;
    int max_spkr_len=7, tmp;
    int sent_count_len=8, total_sents=0;
    FILE *fp, *make_filename(char *dir, char *name, char *ext);
    char spkr_format[20], sent_count_format[20];

    /* added by Brett to compute mean, variance, and standard dev */
    float corr_arr[200], sub_arr[200], del_arr[200];
    float ins_arr[200], err_arr[200], serr_arr[200];
    float mean_corr, mean_del, mean_ins, mean_sub, mean_err, mean_serr;
    float var_corr, var_del, var_ins, var_sub, var_err, var_serr;
    float sd_corr, sd_del, sd_ins, sd_sub, sd_err, sd_serr;
    int sent_num_arr[200];
    float mean_sent_num, var_sent_num, sd_sent_num, Z_stat;

    /* add the sentence count into the overall width */
    pl+=sent_count_len;
    fp = make_filename(output_dir,sys_root_name,"sys");
    for (spkr=0; spkr<num_spkr(align_str); spkr++)
        if ((tmp=strlen(spkr_name(align_str,spkr))) > max_spkr_len)
            max_spkr_len = tmp;
    max_spkr_len++;
    sprintf(spkr_format,"%%-%1ds",max_spkr_len);
    sprintf(sent_count_format,"|%%5d  ");

    /* set the pad to center the table */
    pl += max_spkr_len;
    set_pad_cent_n(pad,pl);

    /* print the report */
   fprintf(fp,"\n\n\n%s%s\n\n",pad,center("SYSTEM SUMMARY PERCENTAGES by SPEAKER",pl));
   fprintf(fp,"%s,---------------------------------------------",pad);
   for (tmp=0;tmp<sent_count_len;tmp++)
       fprintf(fp,"-");
   for (tmp=0; tmp<max_spkr_len; tmp++)
       fprintf(fp,"-");
   fprintf(fp,".\n");
   { /* special provisions for titles longer than the size of the table */
     char *middle, *begin;
     int i;
     begin = sys_desc(align_str);
     while (*begin != NULL_CHAR){
         middle = begin;
         for (i=0; (*middle!=NULL_CHAR) && (i<(pl-4)); i++) 
             middle++;            /*skip to the maximum characters allowed*/
         while ((*middle != NULL_CHAR) && (*middle != SPACE) && (*middle!=TAB))
             middle--;            /*find backwards the first space */
         if (*middle != NULL_CHAR)
             while ((*middle == SPACE) || (*middle == TAB))
                 middle--;        /* point to the last character */
         temp_char = *(middle+1);
         *(middle+1) = NULL_CHAR;
         fprintf(fp,"%s|%s|\n",pad,center(begin,pl-2));
         *(middle+1) = temp_char;
         if (*middle != NULL_CHAR){
             find_next_word(&middle);
             begin = middle;
         }
         else
             begin = middle;
      }
   } /* end of the special provisions for long titles */
   fprintf(fp,"%s|---------------------------------------------",pad);
   for (tmp=0;tmp<sent_count_len;tmp++)
       fprintf(fp,"-");
   for (tmp=0; tmp<max_spkr_len; tmp++)
       fprintf(fp,"-");
   fprintf(fp,"|\n");

    fprintf(fp,"%s| SPKR",pad);
    for (tmp=0; tmp<max_spkr_len-4; tmp++)
       fprintf(fp," ");
    fprintf(fp,"|%s",center("# Snt",sent_count_len-1));
    fprintf(fp,"|  Corr    Sub    Del    Ins    Err  S. Err |\n");
    for (spkr=0; spkr<num_spkr(align_str); spkr++){
     fprintf(fp,"%s|-",pad); 
     for (tmp=0; tmp<max_spkr_len; tmp++)
         fprintf(fp,"-");
     fprintf(fp,"+");
     for (tmp=0;tmp<sent_count_len-1;tmp++)
         fprintf(fp,"-");
     fprintf(fp,"+-------------------------------------------|\n");

     fprintf(fp,"%s| ",pad);
     fprintf(fp,spkr_format,spkr_name(align_str,spkr));
     fprintf(fp,sent_count_format,num_sents(align_str,spkr));
     total_sents+=num_sents(align_str,spkr);

     /* added by Brett to compute mean, variance, and standard dev */
     if (num_spkr(align_str) > NUM_SPKRS)
       {
	 fprintf(stderr,"Error: scrprnt.c: Too many speakers\n");
	 fprintf(stderr,"Increase \'NUM_SPKRS\' and re-compile\n");
	 exit(-1);
       }
     sent_num_arr[spkr]=num_sents(align_str,spkr);
     corr_arr[spkr]=pct(score->spk[spkr]->n_corr,score->spk[spkr]->n_ref);
     sub_arr[spkr]=pct(score->spk[spkr]->n_subs,score->spk[spkr]->n_ref);
     del_arr[spkr]=pct(score->spk[spkr]->n_del,score->spk[spkr]->n_ref);
     ins_arr[spkr]=pct(score->spk[spkr]->n_ins,score->spk[spkr]->n_ref);
     err_arr[spkr]=pct((score->spk[spkr]->n_subs + score->spk[spkr]->n_ins +
                        score->spk[spkr]->n_del),score->spk[spkr]->n_ref);
     serr_arr[spkr]=pct(score->spk[spkr]->n_sent_errors,score->spk[spkr]->n_sent);

     fprintf(fp,"| %5.1f  %5.1f  %5.1f",
              pct(score->spk[spkr]->n_corr,score->spk[spkr]->n_ref),
              pct(score->spk[spkr]->n_subs,score->spk[spkr]->n_ref),
              pct(score->spk[spkr]->n_del,score->spk[spkr]->n_ref));
     fprintf(fp,"  %5.1f  %5.1f   %5.1f |",
              pct(score->spk[spkr]->n_ins,score->spk[spkr]->n_ref),
              pct((score->spk[spkr]->n_subs + score->spk[spkr]->n_ins +
                   score->spk[spkr]->n_del),score->spk[spkr]->n_ref),
              pct(score->spk[spkr]->n_sent_errors,score->spk[spkr]->n_sent));

     fprintf(fp,"\n");
     tot_corr+=  score->spk[spkr]->n_corr;
     tot_sub+=   score->spk[spkr]->n_subs;
     tot_del+=   score->spk[spkr]->n_del;
     tot_ins+=   score->spk[spkr]->n_ins;
     tot_ref+=   score->spk[spkr]->n_ref;
     tot_st_er+= score->spk[spkr]->n_sent_errors;
     tot_st+=    score->spk[spkr]->n_sent;
    }
   fprintf(fp,"%s|=============================================",pad);
   for (tmp=0;tmp<sent_count_len;tmp++)
       fprintf(fp,"=");
   for (tmp=0; tmp<max_spkr_len; tmp++)
       fprintf(fp,"=");
   fprintf(fp,"|\n");

   fprintf(fp,"%s| ",pad);
   fprintf(fp,spkr_format,"Sum/Avg");
   fprintf(fp,sent_count_format,total_sents);
   fprintf(fp,"|  %4.1f   %4.1f   %4.1f",
              pct(tot_corr,tot_ref),
              pct(tot_sub,tot_ref),
              pct(tot_del,tot_ref));
   fprintf(fp,"   %4.1f   %4.1f   %5.1f |",
              pct(tot_ins,tot_ref),
              pct((tot_sub + tot_ins + tot_del),tot_ref),
              pct(tot_st_er,tot_st));
   fprintf(fp,"\n"); 

   fprintf(fp,"%s|=============================================",pad);
   for (tmp=0;tmp<sent_count_len;tmp++)
       fprintf(fp,"=");
   for (tmp=0; tmp<max_spkr_len; tmp++)
       fprintf(fp,"=");
   fprintf(fp,"|\n");


    /* added by Brett to compute mean, variance, and standard dev */
    calc_mean_var_std_dev_Zstat(sent_num_arr,num_spkr(align_str),&mean_sent_num,&var_sent_num,&sd_sent_num,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(corr_arr,num_spkr(align_str),&mean_corr,&var_corr,&sd_corr,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(sub_arr,num_spkr(align_str),&mean_sub,&var_sub,&sd_sub,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(ins_arr,num_spkr(align_str),&mean_ins,&var_ins,&sd_ins,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(del_arr,num_spkr(align_str),&mean_del,&var_del,&sd_del,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(err_arr,num_spkr(align_str),&mean_err,&var_err,&sd_err,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(serr_arr,num_spkr(align_str),&mean_serr,&var_serr,&sd_serr,&Z_stat);
    /* if scores are needed 
    sprintf(scorefile,"%s.scr",sys_root_name);
    sf=fopen(scorefile,"w");
    if (sf == NULL)
      fprintf(fp,"Error: Could not open scorefile \'%s\'\n",scorefile);
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      fprintf(sf,"%.1f  %.1f\n",err_arr[tmp],serr_arr[tmp]);
    fclose(sf);*/

    /* print MEAN line */
    /* fprintf(fp,"%s|-",pad); 
    for (tmp=0; tmp<max_spkr_len; tmp++)
      fprintf(fp,"-");
    fprintf(fp,"+");
    for (tmp=0;tmp<sent_count_len-1;tmp++)
      fprintf(fp,"-");
    fprintf(fp,"+-------------------------------------------|\n");*/
    fprintf(fp,"%s| ",pad);
    fprintf(fp,spkr_format,"Mean");
    fprintf(fp,sent_count_format,(int)mean_sent_num);
    fprintf(fp,"|  %4.1f   %4.1f   %4.1f", mean_corr, mean_sub, mean_del);
    fprintf(fp,"   %4.1f   %4.1f   %5.1f |", mean_ins, mean_err, mean_serr);
    fprintf(fp,"\n");

    /* print VARIANCE line */
/*    fprintf(fp,"%s|-",pad); 
    for (tmp=0; tmp<max_spkr_len; tmp++)
      fprintf(fp,"-");
    fprintf(fp,"+");
    for (tmp=0;tmp<sent_count_len-1;tmp++)
      fprintf(fp,"-");
    fprintf(fp,"+-------------------------------------------|\n");
    fprintf(fp,"%s| ",pad);
    fprintf(fp,spkr_format,"Variance");
    fprintf(fp,sent_count_format,(int)var_sent_num);
    fprintf(fp,"|  %4.1f   %4.1f   %4.1f", var_corr, var_sub, var_del);
    fprintf(fp,"   %4.1f   %4.1f   %5.1f |", var_ins, var_err, var_serr);
    fprintf(fp,"\n");*/

    /* print STAND_DEV line */
/*    fprintf(fp,"%s|-",pad); 
    for (tmp=0; tmp<max_spkr_len; tmp++)
      fprintf(fp,"-");
    fprintf(fp,"+");
    for (tmp=0;tmp<sent_count_len-1;tmp++)
      fprintf(fp,"-");
    fprintf(fp,"+-------------------------------------------|\n"); */
    fprintf(fp,"%s| ",pad);
    fprintf(fp,spkr_format,"S.D.");
    fprintf(fp,sent_count_format,(int)sd_sent_num);
    fprintf(fp,"|  %4.1f   %4.1f   %4.1f", sd_corr, sd_sub, sd_del);
    fprintf(fp,"   %4.1f   %4.1f   %5.1f |", sd_ins, sd_err, sd_serr);
    fprintf(fp,"\n");

    fprintf(fp,"%s`---------------------------------------------",pad);
    for (tmp=0;tmp<sent_count_len;tmp++)
     fprintf(fp,"-");
    for (tmp=0; tmp<max_spkr_len; tmp++)
     fprintf(fp,"-");
    fprintf(fp,"'\n\n");
    /* calc_two_sample_z_test_float(serr_arr,err_arr,num_spkr(align_str),num_spkr(align_str),&Z_stat);*/
    fprintf(fp,"For this set of %d speakers and %d sentences, the mean\n",num_spkr(align_str),total_sents);
    fprintf(fp,"number of sentences per speaker was %d with a standard\n",(int)mean_sent_num);
    fprintf(fp,"uncertainty of %d.  The \'%s\' system had a mean word\n",(int)sd_sent_num,sys_root_name);
    fprintf(fp,"error of %.1f%% with a standard uncertainty of %.1f%%,\n",mean_err,sd_err);
    fprintf(fp,"and a mean sentence error of %.1f%% with a standard\n",mean_serr);
    fprintf(fp,"uncertainty of %.1f%%.  A list of speakers out of the\n",sd_serr);
    fprintf(fp,"expanded uncertainty range is given below.\n");
    fprintf(fp,"\n");

    fprintf(fp,"Speakers with an unusually LOW number of sentences:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (num_sents(align_str,tmp) < (int)(mean_sent_num-(2*sd_sent_num)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");
    fprintf(fp,"Speakers with an unusually HIGH number of sentences:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (num_sents(align_str,tmp) > (int)(mean_sent_num+(2*sd_sent_num)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n\n");

    fprintf(fp,"Speakers with an unusually LOW word error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (err_arr[tmp] < (mean_err-(2*sd_err)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");
    fprintf(fp,"Speakers with an unusually HIGH word error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (err_arr[tmp] > (mean_err+(2*sd_err)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n\n");

    fprintf(fp,"Speakers with an unusually LOW sentence error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (serr_arr[tmp] < (mean_serr-(2*sd_serr)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");
    fprintf(fp,"Speakers with an unusually HIGH sentence error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (serr_arr[tmp] > (mean_serr+(2*sd_serr)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");

    if (fp != stdout)
     fclose(fp);
}
#endif

#ifdef OLD_SM_METHOD
/************************************************************************/
/*   print the report for the entire system.                            */
/************************************************************************/
void print_overall_raw_desc(SCORE *score, SYS_ALIGN *align_str, char *output_dir, char *sys_root_name)
{
    char pad[SCREEN_WIDTH],temp_char;
    int spkr, pl = 47, tot_corr=0, tot_sub=0, tot_del=0;
    int tot_ins=0, tot_ref =0, tot_st=0, tot_st_er=0;
    int max_spkr_len=7, tmp;
    int sent_count_len=8, total_sents=0;
    FILE *fp, *make_filename(char *dir, char *name, char *ext);
    char spkr_format[20], sent_count_format[20];

    /* add the sentence count into the overall width */
    pl+=sent_count_len;
    fp = make_filename(output_dir,sys_root_name,"sys_raw");
    for (spkr=0; spkr<num_spkr(align_str); spkr++)
        if ((tmp=strlen(spkr_name(align_str,spkr))) > max_spkr_len)
            max_spkr_len = tmp;
    max_spkr_len++;
    sprintf(spkr_format,"%%-%1ds",max_spkr_len);
    sprintf(sent_count_format,"|%%5d  ");

    /* set the pad to center the table */
    pl += max_spkr_len;
    set_pad_cent_n(pad,pl);

    /* print the report */
   fprintf(fp,"\n\n\n%s%s\n\n",pad,center("SYSTEM SUMMARY of RAW SCORES by SPEAKER",pl));
   fprintf(fp,"%s,---------------------------------------------",pad);
   for (tmp=0;tmp<sent_count_len;tmp++)
       fprintf(fp,"-");
   for (tmp=0; tmp<max_spkr_len; tmp++)
       fprintf(fp,"-");
   fprintf(fp,".\n");
   { /* special provisions for titles longer than the size of the table */
     char *middle, *begin;
     int i;
     begin = sys_desc(align_str);
     while (*begin != NULL_CHAR){
         middle = begin;
         for (i=0; (*middle!=NULL_CHAR) && (i<(pl-4)); i++) 
             middle++;            /*skip to the maximum characters allowed*/
         while ((*middle != NULL_CHAR) && (*middle != SPACE) && (*middle!=TAB))
             middle--;            /*find backwards the first space */
         if (*middle != NULL_CHAR)
             while ((*middle == SPACE) || (*middle == TAB))
                 middle--;        /* point to the last character */
         temp_char = *(middle+1);
         *(middle+1) = NULL_CHAR;
         fprintf(fp,"%s|%s|\n",pad,center(begin,pl-2));
         *(middle+1) = temp_char;
         if (*middle != NULL_CHAR){
             find_next_word(&middle);
             begin = middle;
         }
         else
             begin = middle;
      }
   } /* end of the special provisions for long titles */
   fprintf(fp,"%s|---------------------------------------------",pad);
   for (tmp=0;tmp<sent_count_len;tmp++)
       fprintf(fp,"-");
   for (tmp=0; tmp<max_spkr_len; tmp++)
       fprintf(fp,"-");
   fprintf(fp,"|\n");

    fprintf(fp,"%s| SPKR",pad);
    for (tmp=0; tmp<max_spkr_len-4; tmp++)
       fprintf(fp," ");
    fprintf(fp,"|%s",center("# Snt",sent_count_len-1));
    fprintf(fp,"| #Corr   #Sub   #Del   #Ins   #Err  #S.Err |\n");
    for (spkr=0; spkr<num_spkr(align_str); spkr++){
     fprintf(fp,"%s|-",pad); 
     for (tmp=0; tmp<max_spkr_len; tmp++)
         fprintf(fp,"-");
     fprintf(fp,"+");
     for (tmp=0;tmp<sent_count_len-1;tmp++)
         fprintf(fp,"-");
     fprintf(fp,"+-------------------------------------------|\n");

     fprintf(fp,"%s| ",pad);
     fprintf(fp,spkr_format,spkr_name(align_str,spkr));
     fprintf(fp,sent_count_format,num_sents(align_str,spkr));
     total_sents+=num_sents(align_str,spkr);
     fprintf(fp,"| %5d  %5d  %5d",
              score->spk[spkr]->n_corr,
              score->spk[spkr]->n_subs,
              score->spk[spkr]->n_del);
     fprintf(fp,"  %5d  %5d   %5d |",
              score->spk[spkr]->n_ins,
              (score->spk[spkr]->n_subs + score->spk[spkr]->n_ins +
                   score->spk[spkr]->n_del),
              score->spk[spkr]->n_sent_errors);

     fprintf(fp,"\n");
     tot_corr+=  score->spk[spkr]->n_corr;
     tot_sub+=   score->spk[spkr]->n_subs;
     tot_del+=   score->spk[spkr]->n_del;
     tot_ins+=   score->spk[spkr]->n_ins;
     tot_ref+=   score->spk[spkr]->n_ref;
     tot_st_er+= score->spk[spkr]->n_sent_errors;
     tot_st+=    score->spk[spkr]->n_sent;
    }
   fprintf(fp,"%s|=============================================",pad);
   for (tmp=0;tmp<sent_count_len;tmp++)
       fprintf(fp,"=");
   for (tmp=0; tmp<max_spkr_len; tmp++)
       fprintf(fp,"=");
   fprintf(fp,"|\n");

  fprintf(fp,"%s| ",pad);
  fprintf(fp,spkr_format,"Sums");
  fprintf(fp,sent_count_format,total_sents);
  fprintf(fp,"| %5d  %5d  %5d",
              tot_corr,
              tot_sub,
              tot_del);
  fprintf(fp,"  %5d  %5d   %5d |",
              tot_ins,
              (tot_sub + tot_ins + tot_del),
              tot_st_er);

   fprintf(fp,"\n");
   fprintf(fp,"%s`---------------------------------------------",pad);
   for (tmp=0;tmp<sent_count_len;tmp++)
       fprintf(fp,"-");
   for (tmp=0; tmp<max_spkr_len; tmp++)
       fprintf(fp,"-");
   fprintf(fp,"'\n");
    if (fp != stdout)
       fclose(fp);
}
#endif


#ifdef NEW_SM_METHOD
/************************************************************************/
/*   print the report for the entire system.                            */
/************************************************************************/
print_overall_desc(score,align_str,output_dir,sys_root_name)
SYS_ALIGN *align_str;
SCORE *score;
char *output_dir, *sys_root_name;
{
    char pad[SCREEN_WIDTH],pad1[SCREEN_WIDTH],pad2[SCREEN_WIDTH],temp_char;
    int spkr, len, l, pl = 47, tot_corr=0, tot_sub=0, tot_del=0;
    int tot_ins=0, tot_ref =0, tot_st=0, tot_st_er=0;
    int tot_spl=0, tot_mrg=0;
    int max_spkr_len=7, tmp;
    int sent_count_len=8, total_sents=0;
    FILE *fp, *make_filename();
    char spkr_format[20], sent_count_format[20];

    /* added by Brett to compute mean, variance, and standard dev */
    float corr_arr[NUM_SPKRS], sub_arr[NUM_SPKRS], del_arr[NUM_SPKRS];
    float ins_arr[NUM_SPKRS], err_arr[NUM_SPKRS], serr_arr[NUM_SPKRS];
    float spl_arr[NUM_SPKRS], mrg_arr[NUM_SPKRS];
    float mean_corr, mean_del, mean_ins, mean_sub, mean_err, mean_serr;
    float mean_spl, mean_mrg;
    float var_corr, var_del, var_ins, var_sub, var_err, var_serr;
    float var_spl, var_mrg;
    float sd_corr, sd_del, sd_ins, sd_sub, sd_err, sd_serr;
    float sd_spl, sd_mrg;
    int sent_num_arr[NUM_SPKRS];
    char scorefile[40];
    FILE *sf;
    float mean_sent_num, var_sent_num, sd_sent_num, Z_stat;


    char *pct_fmt="%5.1f ", *spkr_fmt=" %s ", *sent_fmt="%5d";

    fp = make_filename(output_dir,sys_root_name,OVERALL_EXT);
    fprintf(fp,"\n\n\n%s%s\n\n",pad,
	    center("SYSTEM SUMMARY PERCENTAGES by SPEAKER",SCREEN_WIDTH));

    Desc_erase();
    Desc_set_page_center(SCREEN_WIDTH);
    Desc_add_row_values("c",sys_desc(align_str));
    Desc_add_row_separation('-',BEFORE_ROW);
    Desc_add_row_values("l|c|cccccccc"," SPKR"," # Snt","Corr",
			" Sub"," Del"," Ins"," Mrg"," Spl"," Err","S.Err");

    for (spkr=0; spkr<num_spkr(align_str); spkr++){
	Desc_add_row_separation('-',BEFORE_ROW);

	sent_num_arr[spkr]=num_sents(align_str,spkr);
	corr_arr[spkr]=pct(sp_n_corr(spkr),sp_n_ref(spkr));
	sub_arr[spkr]=pct(sp_n_sub(spkr),sp_n_ref(spkr));
	del_arr[spkr]=pct(sp_n_del(spkr),sp_n_ref(spkr));
	ins_arr[spkr]=pct(sp_n_ins(spkr),sp_n_ref(spkr));
	spl_arr[spkr]=pct(sp_n_spl(spkr),sp_n_ref(spkr));
	mrg_arr[spkr]=pct(sp_n_mrg(spkr),sp_n_ref(spkr));
	err_arr[spkr]=pct((sp_n_sub(spkr) + sp_n_ins(spkr) +
			   sp_n_del(spkr) + sp_n_spl(spkr) +
			   sp_n_mrg(spkr)),sp_n_ref(spkr));
	serr_arr[spkr]=pct(sp_n_st_er(spkr),sp_n_st(spkr));

	Desc_set_iterated_format("l|c|cccccccc");
	Desc_set_iterated_value(rsprintf(spkr_fmt,spkr_name(align_str,spkr)));
	Desc_set_iterated_value(rsprintf(sent_fmt,sent_num_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,corr_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,sub_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,del_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,ins_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,mrg_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,spl_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,err_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,serr_arr[spkr]));
	Desc_flush_iterated_row();
	tot_corr+=  sp_n_corr(spkr);
	tot_sub+=   sp_n_sub(spkr);
	tot_del+=   sp_n_del(spkr);
	tot_ins+=   sp_n_ins(spkr);
	tot_ref+=   sp_n_ref(spkr);
	tot_spl+=   sp_n_spl(spkr);
	tot_mrg+=   sp_n_mrg(spkr);
	tot_st_er+= sp_n_st_er(spkr);
	tot_st+=    sp_n_st(spkr);
    }
    Desc_add_row_separation('=',BEFORE_ROW); 
    Desc_set_iterated_format("l|c|cccccccc");
    Desc_set_iterated_value(" Sum/Avg");
    Desc_set_iterated_value(rsprintf(sent_fmt,tot_st));
    Desc_set_iterated_value(rsprintf(pct_fmt,pct(tot_corr,tot_ref)));
    Desc_set_iterated_value(rsprintf(pct_fmt,pct(tot_sub,tot_ref)));
    Desc_set_iterated_value(rsprintf(pct_fmt,pct(tot_ins,tot_ref)));
    Desc_set_iterated_value(rsprintf(pct_fmt,pct(tot_del,tot_ref)));
    Desc_set_iterated_value(rsprintf(pct_fmt,pct(tot_mrg,tot_ref)));
    Desc_set_iterated_value(rsprintf(pct_fmt,pct(tot_spl,tot_ref)));
    Desc_set_iterated_value(rsprintf(pct_fmt,pct(tot_sub + tot_ins + tot_del +
				          tot_spl + tot_mrg,tot_ref)));
    Desc_set_iterated_value(rsprintf(pct_fmt,pct(tot_st_er,tot_st)));
    Desc_flush_iterated_row();

    Desc_add_row_separation('=',BEFORE_ROW); 
    /* added by Brett to compute mean, variance, and standard dev */
    calc_mean_var_std_dev_Zstat(sent_num_arr,num_spkr(align_str),
			&mean_sent_num,&var_sent_num,&sd_sent_num,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(corr_arr,num_spkr(align_str),
 		        &mean_corr,&var_corr,&sd_corr,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(sub_arr,num_spkr(align_str),
			&mean_sub,&var_sub,&sd_sub,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(ins_arr,num_spkr(align_str),
		        &mean_ins,&var_ins,&sd_ins,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(del_arr,num_spkr(align_str),
			&mean_del,&var_del,&sd_del,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(spl_arr,num_spkr(align_str),
			&mean_spl,&var_spl,&sd_spl,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(mrg_arr,num_spkr(align_str),
			&mean_mrg,&var_mrg,&sd_mrg,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(err_arr,num_spkr(align_str),
			&mean_err,&var_err,&sd_err,&Z_stat);
    calc_mean_var_std_dev_Zstat_float(serr_arr,num_spkr(align_str),
		        &mean_serr,&var_serr,&sd_serr,&Z_stat);

    Desc_set_iterated_format("l|c|cccccccc");
    Desc_set_iterated_value(" Mean");
    Desc_set_iterated_value(rsprintf(pct_fmt,mean_sent_num));
    Desc_set_iterated_value(rsprintf(pct_fmt,mean_corr));
    Desc_set_iterated_value(rsprintf(pct_fmt,mean_sub));
    Desc_set_iterated_value(rsprintf(pct_fmt,mean_del));
    Desc_set_iterated_value(rsprintf(pct_fmt,mean_ins));
    Desc_set_iterated_value(rsprintf(pct_fmt,mean_spl));
    Desc_set_iterated_value(rsprintf(pct_fmt,mean_mrg));
    Desc_set_iterated_value(rsprintf(pct_fmt,mean_err));
    Desc_set_iterated_value(rsprintf(pct_fmt,mean_serr));
    Desc_flush_iterated_row();

    Desc_set_iterated_format("l|c|cccccccc");
    Desc_set_iterated_value(" S.D.");
    Desc_set_iterated_value(rsprintf(pct_fmt,sd_sent_num));
    Desc_set_iterated_value(rsprintf(pct_fmt,sd_corr));
    Desc_set_iterated_value(rsprintf(pct_fmt,sd_sub));
    Desc_set_iterated_value(rsprintf(pct_fmt,sd_del));
    Desc_set_iterated_value(rsprintf(pct_fmt,sd_ins));
    Desc_set_iterated_value(rsprintf(pct_fmt,sd_spl));
    Desc_set_iterated_value(rsprintf(pct_fmt,sd_mrg));
    Desc_set_iterated_value(rsprintf(pct_fmt,sd_err));
    Desc_set_iterated_value(rsprintf(pct_fmt,sd_serr));
    Desc_flush_iterated_row();
#ifdef AT_NIST
    Desc_dump_ascii_report("score.pct.arpg");
#endif
    Desc_dump_report(0,fp);

    fprintf(fp,"'\n\n");
    /* calc_two_sample_z_test_float(serr_arr,err_arr,num_spkr(align_str),
       num_spkr(align_str),&Z_stat);*/
    fprintf(fp,"For this set of %d speakers and %d sentences, the mean\n",
	    num_spkr(align_str),total_sents);
    fprintf(fp,"number of sentences per speaker was %d with a standard\n",
	    (int)mean_sent_num);
    fprintf(fp,"uncertainty of %d.  The \'%s\' system had a mean word\n",
	    (int)sd_sent_num,sys_root_name);
    fprintf(fp,"error of %.1f%% with a standard uncertainty of %.1f%%,\n",
	    mean_err,sd_err);
    fprintf(fp,"and a mean sentence error of %.1f%% with a standard\n",
	    mean_serr);
    fprintf(fp,"uncertainty of %.1f%%.  A list of speakers out of the\n",
	    sd_serr);
    fprintf(fp,"expanded uncertainty range is given below.\n");
    fprintf(fp,"\n");

    fprintf(fp,"Speakers with an unusually LOW number of sentences:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (num_sents(align_str,tmp) < (int)(mean_sent_num-(2*sd_sent_num)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");
    fprintf(fp,"Speakers with an unusually HIGH number of sentences:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (num_sents(align_str,tmp) > (int)(mean_sent_num+(2*sd_sent_num)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n\n");

    fprintf(fp,"Speakers with an unusually LOW word error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (err_arr[tmp] < (mean_err-(2*sd_err)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");
    fprintf(fp,"Speakers with an unusually HIGH word error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (err_arr[tmp] > (mean_err+(2*sd_err)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n\n");

    fprintf(fp,"Speakers with an unusually LOW sentence error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (serr_arr[tmp] < (mean_serr-(2*sd_serr)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");
    fprintf(fp,"Speakers with an unusually HIGH sentence error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (serr_arr[tmp] > (mean_serr+(2*sd_serr)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");


    if (fp != stdout)
       fclose(fp);
}
#endif 

#ifdef NEW_SM_METHOD
/************************************************************************/
/*   print the report for the entire system.                            */
/************************************************************************/
print_overall_raw_desc(score,align_str,output_dir,sys_root_name)
SYS_ALIGN *align_str;
SCORE *score;
char *output_dir, *sys_root_name;
{
    FILE *fp, *make_filename();
    char num_format[10];
    int spkr, tot_corr=0, tot_sub=0, tot_del=0;
    int tot_ins=0, tot_ref =0, tot_st=0, tot_st_er=0;
    int tot_spl=0, tot_mrg=0;
    char pad[SCREEN_WIDTH];

    fp = make_filename(output_dir,sys_root_name,OVERALL_RAW_EXT);
    fprintf(fp,"\n\n\n%s%s\n\n",pad,
	    center("SYSTEM SUMMARY of RAW SCORES by SPEAKER",SCREEN_WIDTH));

    Desc_erase();
    Desc_set_page_center(SCREEN_WIDTH);
    Desc_add_row_values("c",sys_desc(align_str));
    Desc_add_row_separation('-',BEFORE_ROW);
    Desc_add_row_values("l|c|cccccccc","SPKR","# Snt","#Corr",
			"#Sub","#Del","#Ins","#Mrg","#Spl","#Err","#S.Err");
    Desc_add_row_separation('-',BEFORE_ROW);

    for (spkr=0; spkr<num_spkr(align_str); spkr++){
	Desc_set_iterated_format("l|r|rrrrrrrr");
	Desc_set_iterated_value(spkr_name(align_str,spkr));
	Desc_set_iterated_value(rsprintf("%d",num_sents(align_str,spkr)));
	Desc_set_iterated_value(rsprintf("%d",sp_n_corr(spkr)));
	Desc_set_iterated_value(rsprintf("%d",sp_n_sub(spkr)));
	Desc_set_iterated_value(rsprintf("%d",sp_n_del(spkr)));
	Desc_set_iterated_value(rsprintf("%d",sp_n_ins(spkr)));
	Desc_set_iterated_value(rsprintf("%d",sp_n_mrg(spkr)));
	Desc_set_iterated_value(rsprintf("%d",sp_n_spl(spkr)));
	Desc_set_iterated_value(rsprintf("%d",sp_n_corr(spkr) + 
					 sp_n_sub(spkr) + 
					 sp_n_del(spkr) + 
					 sp_n_ins(spkr) + 
					 sp_n_mrg(spkr) + 
					 sp_n_spl(spkr)));
	Desc_set_iterated_value(rsprintf("%d",sp_n_st_er(spkr)));
	Desc_flush_iterated_row();
	tot_corr+=  sp_n_corr(spkr);
	tot_sub+=   sp_n_sub(spkr);
	tot_del+=   sp_n_del(spkr);
	tot_ins+=   sp_n_ins(spkr);
	tot_ref+=   sp_n_ref(spkr);
	tot_spl+=   sp_n_spl(spkr);
	tot_mrg+=   sp_n_mrg(spkr);
	tot_st_er+= sp_n_st_er(spkr);
	tot_st+=    sp_n_st(spkr);
    }
    Desc_add_row_separation('=',BEFORE_ROW); 
    Desc_set_iterated_format("l|r|rrrrrrrr");
    Desc_set_iterated_value("Sum");
    Desc_set_iterated_value(rsprintf("%d",tot_st));
    Desc_set_iterated_value(rsprintf("%d",tot_corr));
    Desc_set_iterated_value(rsprintf("%d",tot_sub));
    Desc_set_iterated_value(rsprintf("%d",tot_ins));
    Desc_set_iterated_value(rsprintf("%d",tot_del));
    Desc_set_iterated_value(rsprintf("%d",tot_mrg));
    Desc_set_iterated_value(rsprintf("%d",tot_spl));
    Desc_set_iterated_value(rsprintf("%d",tot_sub + tot_ins + tot_del +
				          tot_spl + tot_mrg));
    Desc_set_iterated_value(rsprintf("%d",tot_st_er));
    Desc_flush_iterated_row();

#ifdef AT_NIST
    Desc_dump_ascii_report("score.raw.arpg");
#endif

    Desc_dump_report(1,fp);
    if (fp != stdout)
       fclose(fp);
}
#endif


/*********************************************************************/
/*   given the alignment file filename, extract the root system name */
/*********************************************************************/
void make_sys_root_name(char **sys_root_name, char *ali_fname)
{
    char *tname;
    int per, beg, a_ind;

    a_ind = strlen(ali_fname) - 1;
    /* find the last period */
    while ((a_ind >= 0) && (ali_fname[a_ind] != PERIOD))
        a_ind--;
    per = a_ind;
    /* find the beginning of the system root name */
    while ((a_ind >= 0) && (ali_fname[a_ind] != SLASH))
        a_ind--;
    beg=a_ind;

    /* copy the system root name into the variable */
    if ((per == 0) || (beg == 0)){
	alloc_singarr(tname,2,char);
	*tname = NULL_CHAR;
    } else {
	alloc_singarr(tname,per - (beg+1) + 2,char);
	for (a_ind = beg+1; a_ind < per; a_ind++)
	    tname[a_ind-(beg+1)] = ali_fname[a_ind];
	tname[a_ind-(beg+1)] = NULL_CHAR;
    }
    *sys_root_name = tname;
}

/**********************************************************************/
/*  print the report to summarize the speaker                         */
/**********************************************************************/
void print_spkr_desc(SCORE *score, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, SM_CAND *sm_cand, char *spkr_str, int spkr, char *output_dir, char *ext, char *label, int thresh, char *s_desc, int spkr_report)
{
     int t_err;
     FILE *fp, *make_filename(char *dir, char *name, char *ext);
     int subset_flag = TRUE, set;

     /* if it's possible open the file */
     fp = make_filename(output_dir,spkr_str,ext);
     if (spkr_report)
         fprintf(fp,"%s %s\n     of %s\n\n",label,spkr_str,s_desc);
     else
         fprintf(fp,"%s %s\n\n",label,spkr_str);

     fprintf(fp,"SENTENCE RECOGNITION PERFORMANCE\n\n");
     fprintf(fp," sentences                                        %4d\n",
                                                score->spk[spkr]->n_sent);
     fprintf(fp," with errors                            %5.1f%%   (%4d)\n\n",
              pct(score->spk[spkr]->n_sent_errors,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_errors);
     fprintf(fp,"   with substitions                     %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_sub,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_sub);
     fprintf(fp,"   with substitions of alpha numeric    %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_sub_alpha,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_sub_alpha);
     fprintf(fp,"   with substitions of mono-syllable    %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_sub_mono,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_sub_mono);
     if (subset_flag == TRUE)
        for (set=0; set<lsub_num_set(lsub); set++){
            fprintf(fp,"   with subs of %-24s",lsub_set_desc(lsub,set));
            fprintf(fp,"%5.1f%%   (%4d)\n",
                     pct(score->spk[spkr]->n_sent_sub_lsub[set],score->spk[spkr]->n_sent),
                         score->spk[spkr]->n_sent_sub_lsub[set]);
        }
     fprintf(fp,"\n");

     fprintf(fp,"   with deletions                       %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_del,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_del);
     fprintf(fp,"   with deletions of THE                %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_del_THE,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_del_THE);
     fprintf(fp,"   with deletions of THE only           %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_del_THE_only,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_del_THE_only);
     fprintf(fp,"   with deletions of alpha numeric      %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_del_alpha,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_del_alpha);
     fprintf(fp,"   with deletions of mono-syllable      %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_del_mono,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_del_mono);
     if (subset_flag == TRUE)
        for (set=0; set<lsub_num_set(lsub); set++){
            fprintf(fp,"   with dels of %-24s",lsub_set_desc(lsub,set));
            fprintf(fp,"%5.1f%%   (%4d)\n",
                     pct(score->spk[spkr]->n_sent_del_lsub[set],score->spk[spkr]->n_sent),
                         score->spk[spkr]->n_sent_del_lsub[set]);
        }
     fprintf(fp,"\n");

     fprintf(fp,"   with insertions                      %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_ins,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_ins);
     fprintf(fp,"   with insertions of alpha numeric     %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_ins_alpha,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_ins_alpha);
     fprintf(fp,"   with insertions of mono-syllable     %5.1f%%   (%4d)\n",
              pct(score->spk[spkr]->n_sent_ins_mono,score->spk[spkr]->n_sent),
              score->spk[spkr]->n_sent_ins_mono);
     if (subset_flag == TRUE)
        for (set=0; set<lsub_num_set(lsub); set++){
            fprintf(fp,"   with ins. of %-24s",lsub_set_desc(lsub,set));
            fprintf(fp,"%5.1f%%   (%4d)\n",
                     pct(score->spk[spkr]->n_sent_ins_lsub[set],score->spk[spkr]->n_sent),
                         score->spk[spkr]->n_sent_ins_lsub[set]);
        }
#ifdef NEW_SM_METHOD
     fprintf(fp,"\n");

     fprintf(fp,"   with merges                          %5.1f%%   (%4d)\n",
              pct(sp_n_st_mrg(spkr),sp_n_st(spkr)),
              sp_n_st_mrg(spkr));
     fprintf(fp,"   with splits                          %5.1f%%   (%4d)\n",
              pct(sp_n_st_spl(spkr),sp_n_st(spkr)),
              sp_n_st_spl(spkr));
#endif


     fprintf(fp,"\n\n");


     fprintf(fp,"WORD RECOGNITION PERFORMANCE\n\n");
     t_err = score->spk[spkr]->n_subs + score->spk[spkr]->n_ins +
             score->spk[spkr]->n_del;
     fprintf(fp,"Percent Total Error       =  %5.1f%%   (%4d)\n",
        pct(t_err,score->spk[spkr]->n_ref), t_err);
     fprintf(fp,"\n");

     fprintf(fp,"Percent Correct           =  %5.1f%%   (%4d)\n",
        pct(score->spk[spkr]->n_corr,score->spk[spkr]->n_ref),
        score->spk[spkr]->n_corr);
     if (!spont_ver)
      if (subset_flag == TRUE)
        for (set=0; set<lsub_num_set(lsub); set++){
            fprintf(fp,"  %-24s=",lsub_set_desc(lsub,set));
            fprintf(fp,"  %5.1f%%   (%4d)\n",
                     pct(score->spk[spkr]->n_corr_lsub[set],score->spk[spkr]->n_ref),
                         score->spk[spkr]->n_corr_lsub[set]);
            fprintf(fp,"  Frac %-19s=",lsub_set_desc(lsub,set));
            fprintf(fp,"  %5.1f%%\n",
                     pct(score->spk[spkr]->n_corr_lsub[set],score->spk[spkr]->n_corr));
        }
     fprintf(fp,"Percent Substitution      =  %5.1f%%   (%4d)\n",
        pct(score->spk[spkr]->n_subs,score->spk[spkr]->n_ref),
        score->spk[spkr]->n_subs);
     if (!spont_ver){
      fprintf(fp,"Substitution Alpha/Num    =  %5.1f%%   (%4d)\n",
         pct(score->spk[spkr]->n_subs_alphnum,score->spk[spkr]->n_ref),
         score->spk[spkr]->n_subs_alphnum);
      fprintf(fp,"Fractional Alpha/Num Subs =  %5.1f%%\n",
         pct(score->spk[spkr]->n_subs_alphnum,score->spk[spkr]->n_subs));
      fprintf(fp,"Substitution Mono-Syl     =  %5.1f%%   (%4d)\n",
         pct(score->spk[spkr]->n_subs_mono,score->spk[spkr]->n_ref),
         score->spk[spkr]->n_subs_mono);
      fprintf(fp,"Fractional Mono-Syl Subs  =  %5.1f%%\n",
         pct(score->spk[spkr]->n_subs_mono,score->spk[spkr]->n_subs));
      if (subset_flag == TRUE)
         for (set=0; set<lsub_num_set(lsub); set++){
             fprintf(fp,"  %-24s=",lsub_set_desc(lsub,set));
             fprintf(fp,"  %5.1f%%   (%4d)\n",
                     pct(score->spk[spkr]->n_subs_lsub[set],score->spk[spkr]->n_ref),
                         score->spk[spkr]->n_subs_lsub[set]);
             fprintf(fp,"  Frac %-19s=",lsub_set_desc(lsub,set));
             fprintf(fp,"  %5.1f%%\n",
                     pct(score->spk[spkr]->n_subs_lsub[set],score->spk[spkr]->n_subs));
         }
      fprintf(fp,"\n");
     }

     fprintf(fp,"Percent Deletions         =  %5.1f%%   (%4d)\n",
        pct(score->spk[spkr]->n_del,score->spk[spkr]->n_ref),
        score->spk[spkr]->n_del);
     if (!spont_ver){
      fprintf(fp,"Deletions of THE          =  %5.1f%%   (%4d)\n",
         pct(score->spk[spkr]->n_del_THE,score->spk[spkr]->n_ref),
         score->spk[spkr]->n_del_THE);
      fprintf(fp,"Deletions Alpha/Num       =  %5.1f%%   (%4d)\n",
         pct(score->spk[spkr]->n_del_alphnum,score->spk[spkr]->n_ref),
         score->spk[spkr]->n_del_alphnum);
      fprintf(fp,"Fractional Alpha/Num Dels =  %5.1f%%\n",
         pct(score->spk[spkr]->n_del_alphnum,score->spk[spkr]->n_del));
      fprintf(fp,"Deletions Mono-Syl        =  %5.1f%%   (%4d)\n",
         pct(score->spk[spkr]->n_del_mono,score->spk[spkr]->n_ref),
         score->spk[spkr]->n_del_mono);
      fprintf(fp,"Fractional Mono-Syl Dels  =  %5.1f%%\n",
         pct(score->spk[spkr]->n_del_mono,score->spk[spkr]->n_del));
      if (subset_flag == TRUE)
         for (set=0; set<lsub_num_set(lsub); set++){
            fprintf(fp,"  %-24s=",lsub_set_desc(lsub,set));
            fprintf(fp,"  %5.1f%%   (%4d)\n",
                     pct(score->spk[spkr]->n_del_lsub[set],score->spk[spkr]->n_ref),
                         score->spk[spkr]->n_del_lsub[set]);
            fprintf(fp,"  Frac %-19s=",lsub_set_desc(lsub,set));
            fprintf(fp,"  %5.1f%%\n",
                     pct(score->spk[spkr]->n_del_lsub[set],score->spk[spkr]->n_del));
         }
      fprintf(fp,"\n");
     }

     fprintf(fp,"Percent Insertions        =  %5.1f%%   (%4d)\n",
        pct(score->spk[spkr]->n_ins,score->spk[spkr]->n_ref),
        score->spk[spkr]->n_ins);
     if (!spont_ver){
      fprintf(fp,"Insertions Alpha/Num      =  %5.1f%%   (%4d)\n",
         pct(score->spk[spkr]->n_ins_alphnum,score->spk[spkr]->n_ref),
         score->spk[spkr]->n_ins_alphnum);
      fprintf(fp,"Fractional Alpha/Num Ins  =  %5.1f%%\n",
         pct(score->spk[spkr]->n_ins_alphnum,score->spk[spkr]->n_ins));
      fprintf(fp,"Insertions Mono-Syl       =  %5.1f%%   (%4d)\n",
         pct(score->spk[spkr]->n_ins_mono,score->spk[spkr]->n_ref),
         score->spk[spkr]->n_ins_mono);
      fprintf(fp,"Fractional Mono-Syl Ins   =  %5.1f%%\n",
         pct(score->spk[spkr]->n_ins_mono,score->spk[spkr]->n_ins));
      if (subset_flag == TRUE)
         for (set=0; set<lsub_num_set(lsub); set++){
             fprintf(fp,"  %-24s=",lsub_set_desc(lsub,set));
             fprintf(fp,"  %5.1f%%   (%4d)\n",
                     pct(score->spk[spkr]->n_ins_lsub[set],score->spk[spkr]->n_ref),
                         score->spk[spkr]->n_ins_lsub[set]);
             fprintf(fp,"  Frac %-19s=",lsub_set_desc(lsub,set));
             fprintf(fp,"  %5.1f%%\n",
                     pct(score->spk[spkr]->n_ins_lsub[set],score->spk[spkr]->n_ins));
         }
     }
#ifdef NEW_SM_METHOD 
     fprintf(fp,"Percent Merges            =  %5.1f%%   (%4d)\n",
        pct(sp_n_mrg(spkr),sp_n_ref(spkr)),
        sp_n_mrg(spkr));
     fprintf(fp,"Percent Splits            =  %5.1f%%   (%4d)\n",
        pct(sp_n_spl(spkr),sp_n_ref(spkr)),
        sp_n_spl(spkr));
      fprintf(fp,"\n");
#endif

#ifdef NEW_SM_METHOD 
     fprintf(fp,"Percent Word Accuracy     =  %5.1f%%\n",
          100.0 -  pct((sp_n_del(spkr) + sp_n_spl(spkr) +
                        sp_n_sub(spkr) + sp_n_mrg(spkr) +
                        sp_n_ins(spkr)),sp_n_ref(spkr)));
#endif
#ifdef OLD_SM_METHOD
     fprintf(fp,"Percent Word Accuracy     =  %5.1f%%\n",
          100.0 -  pct((score->spk[spkr]->n_del +
                        score->spk[spkr]->n_subs +
                        score->spk[spkr]->n_ins),score->spk[spkr]->n_ref));
#endif
     fprintf(fp,"\n");
     if (!spont_ver){
      fprintf(fp,"Fractional Alpha/Num Err  =  %5.1f%%\n",
         pct((score->spk[spkr]->n_subs_alphnum+
             score->spk[spkr]->n_del_alphnum+
             score->spk[spkr]->n_ins_alphnum) ,t_err));
      fprintf(fp,"Fractional Mono-Syl Err   =  %5.1f%%\n",
         pct((score->spk[spkr]->n_subs_mono+
             score->spk[spkr]->n_del_mono+
             score->spk[spkr]->n_ins_mono) ,t_err));
     }
     fprintf(fp,"\n");

#ifdef OLD_SM_METHOD
     /* try to evaluate the effects of splits and merges */
     fprintf(fp,"Split Candidates          =           (%4d)\n",
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_SPLIT,SPLIT,(-10.0)));
     fprintf(fp,"    FOM Splits > %5.2f    =           (%4d)\n",10.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_SPLIT,SPLIT,10.0));
     fprintf(fp,"    FOM Splits > %5.2f    =           (%4d)\n",5.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_SPLIT,SPLIT,5.0));
     fprintf(fp,"    FOM Splits > %5.2f    =           (%4d)\n",2.5,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_SPLIT,SPLIT,2.5));
     fprintf(fp,"    FOM Splits > %5.2f    =           (%4d)\n",2.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_SPLIT,SPLIT,2.0));
     fprintf(fp,"    FOM Splits > %5.2f    =           (%4d)\n",1.75,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_SPLIT,SPLIT,1.75));
     fprintf(fp,"    FOM Splits > %5.2f    =           (%4d)\n",1.5,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_SPLIT,SPLIT,1.5));
     fprintf(fp,"    FOM Splits > %5.2f    =           (%4d)\n",1.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_SPLIT,SPLIT,1.0));
     fprintf(fp,"    FOM Splits > %5.2f    =           (%4d)\n",0.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_SPLIT,SPLIT,0.0));
     fprintf(fp,"\n");
     fprintf(fp,"Merge Candidates          =           (%4d)\n",
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_MERGE,MERGE,(-10.0)));
     fprintf(fp,"    FOM Merges > %5.2f    =           (%4d)\n",10.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_MERGE,MERGE,10.0));
     fprintf(fp,"    FOM Merges > %5.2f    =           (%4d)\n",5.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_MERGE,MERGE,5.0));
     fprintf(fp,"    FOM Merges > %5.2f    =           (%4d)\n",2.5,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_MERGE,MERGE,2.5));
     fprintf(fp,"    FOM Merges > %5.2f    =           (%4d)\n",2.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_MERGE,MERGE,2.0));
     fprintf(fp,"    FOM Merges > %5.2f    =           (%4d)\n",1.75,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_MERGE,MERGE,1.75));
     fprintf(fp,"    FOM Merges > %5.2f    =           (%4d)\n",1.5,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_MERGE,MERGE,1.5));
     fprintf(fp,"    FOM Merges > %5.2f    =           (%4d)\n",1.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_MERGE,MERGE,1.0));
     fprintf(fp,"    FOM Merges > %5.2f    =           (%4d)\n",0.0,
	     count_SM_CAND_type_above_thresh(sm_cand,ACC_MERGE,MERGE,0.0));
#endif

     fprintf(fp,"\n");

     fprintf(fp,"Ref. words                =           (%4d)\n",
        score->spk[spkr]->n_ref);
     fprintf(fp,"Hyp. words                =           (%4d)\n",
        score->spk[spkr]->n_hyp);
     fprintf(fp,"Aligned words             =           (%4d)\n",
        score->spk[spkr]->n_align_words);
     if (!spont_ver)
      fprintf(fp,"Ref. Mono-Syl Words       =  %5.1f%%   (%4d)\n",
         pct(score->spk[spkr]->n_ref_mono_syl,score->spk[spkr]->n_ref),
         score->spk[spkr]->n_ref_mono_syl);
     fprintf(fp,"\n");
#ifdef OLD_SM_METHOD
     if (!spont_ver){
      fprintf(fp,"Acceptable Merges         =           (%4d)\n",
         score->spk[spkr]->n_accept_merge);
      fprintf(fp,"Merges                    =           (%4d)\n",
         score->spk[spkr]->n_merge);
      fprintf(fp,"Acceptable Splits         =           (%4d)\n",
         score->spk[spkr]->n_accept_split);
      fprintf(fp,"Splits                    =           (%4d)\n\n",
         score->spk[spkr]->n_split);
     }
#endif
     print_conf_pairs(score,spkr,pcs,fp,"CONFUSION PAIRS",thresh,
		      score->spk[spkr]->n_in_sub_arr, score->spk[spkr]->conf_count_arr,
		      score->spk[spkr]->conf_word_arr);
     fprintf(fp,"\n\n");

     print_wrd_cnt_arr_of_lex2(score->spk[spkr]->n_ins_word,
			       score->spk[spkr]->max_ins_word,
			       score->spk[spkr]->ins_word_arr,
			       pcs,fp,"INSERTED WORDS",thresh);
     print_wrd_cnt_arr_of_lex2(score->spk[spkr]->n_del_word,
			       score->spk[spkr]->max_del_word,
			       score->spk[spkr]->del_word_arr,
			       pcs,fp,"DELETED WORDS",thresh);
     print_wrd_cnt_arr_of_lex2(score->spk[spkr]->n_misr_word,
			       score->spk[spkr]->max_misr_word,
			       score->spk[spkr]->misr_word_arr,
			       pcs,fp,"MISRECOGNIZED WORDS",thresh);
     print_wrd_cnt_arr_of_lex2(score->spk[spkr]->n_sub_word,
			       score->spk[spkr]->max_sub_word,
			       score->spk[spkr]->sub_word_arr,
			       pcs,fp,"SUBSTITUTED WORDS",thresh);


     /* since this is the speaker summary, set the base_candidate to zero */
     /* to allow all the candidates to be printed out */
#ifdef OLD_SM_METHOD
     if (!spont_ver)
         print_SM_CAND_type(sm_cand,pcs,ACC_SPLIT,fp);
     print_SM_CAND_type(sm_cand,pcs,SPLIT,fp);
     if (!spont_ver)
         print_SM_CAND_type(sm_cand,pcs,ACC_MERGE,fp);
     print_SM_CAND_type(sm_cand,pcs,MERGE,fp);
#endif
#ifdef NEW_SM_METHOD
     print_conf_pairs(score,spkr,pcs,fp,"SPLIT PAIRS",1,
		      sp_n_split_arr(spkr), sp_split_cnt_arr(spkr),
		      sp_split_arr(spkr));
     fprintf(fp,"\n\n");

     print_conf_pairs(score,spkr,pcs,fp,"MERGE PAIRS",1,
		      sp_n_merge_arr(spkr), sp_merge_cnt_arr(spkr),
		      sp_merge_arr(spkr));
     fprintf(fp,"\n\n");
#endif

     report_lexicon_subsets_percents(score,pcs,lsub,spkr,fp,
                "                         Lexicon Subset Percentages");

     report_lexicon_subsets_raw(score,pcs,lsub,spkr,fp);

     if (fp != stdout)
         fclose(fp); 
} 

/*************************************************************************/
/*   generate a report for the lexicon subsets partitioned in the        */
/*   lexicon structure							 */
/*************************************************************************/
void report_lexicon_subsets_percents(SCORE *score, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int spkr, FILE *fp, char *title)
{
    int set;

    if (lsub_num_set(lsub) <= 0)
       return;
    fprintf(fp,"%s\n\n",title);
    fprintf(fp,"------------------------------------------------------");
    fprintf(fp,"---------------------\n");
    fprintf(fp,"|                               | %% in | %% in |");
    fprintf(fp,"  %%   |   %%  |   %%  |   %%  |\n");
    fprintf(fp,"|      Partitions               | REF  | HYP  |");
    fprintf(fp," Corr |  Sub |  Del |  Ins |\n");
    fprintf(fp,"|-------------------------------+------+------+");
    fprintf(fp,"------+------+------+------|\n");

    /* print the actual system results (it's a simple repetition) */
    fprintf(fp,"| %-30s|             |","The Complete Lexicon");
    fprintf(fp,"%5.1f  %5.1f  %5.1f  %5.1f |\n",
                   pct(score->spk[spkr]->n_corr,score->spk[spkr]->n_ref),
                   pct(score->spk[spkr]->n_subs,score->spk[spkr]->n_ref),
                   pct(score->spk[spkr]->n_del,score->spk[spkr]->n_ref),
                   pct(score->spk[spkr]->n_ins,score->spk[spkr]->n_ref));
    fprintf(fp,"|-------------------------------+-------------+");
    fprintf(fp,"---------------------------|\n");
    fprintf(fp,"|      Extra-Lexical Items      |             |");
    fprintf(fp,"                           |\n");
    fprintf(fp,"|-------------------------------+-------------+");
    fprintf(fp,"---------------------------|\n");

    for (set=0; set<lsub_num_set(lsub); set++){
        fprintf(fp,"| %-30s|%5.1f  %5.1f |",lsub_set_desc(lsub,set),
                   pct(score->spk[spkr]->n_subs_lsub[set]+
                       score->spk[spkr]->n_del_lsub[set]+score->spk[spkr]->n_corr_lsub[set],
                      score->spk[spkr]->n_ref),
                   pct(score->spk[spkr]->n_hyp_lsub[set],score->spk[spkr]->n_hyp));
        fprintf(fp,"%5.1f  %5.1f  %5.1f  %5.1f |\n",
                   pct(score->spk[spkr]->n_corr_lsub[set],score->spk[spkr]->n_ref),
                   pct(score->spk[spkr]->n_subs_lsub[set],score->spk[spkr]->n_ref),
                   pct(score->spk[spkr]->n_del_lsub[set],score->spk[spkr]->n_ref),
                   pct(score->spk[spkr]->n_ins_lsub[set],score->spk[spkr]->n_ref));
    }
    fprintf(fp,"------------------------------------------------------");
    fprintf(fp,"---------------------\n");

}

void report_lexicon_subsets_raw(SCORE *score, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int spkr, FILE *fp)
{
    int set;

    if (lsub_num_set(lsub) <= 0)
       return;
    fprintf(fp,"------------------------------------------------------");
    fprintf(fp,"---------------------\n");
    fprintf(fp,"|                               | # in | # in |");
    fprintf(fp,"  #   |   #  |   #  |   #  |\n");
    fprintf(fp,"|      Partitions               | REF  | HYP  |");
    fprintf(fp," Corr |  Sub |  Del |  Ins |\n");
    fprintf(fp,"|-------------------------------+------+------+");
    fprintf(fp,"------+------+------+------|\n");

    /* print the actual system results (it's a simple repetition) */
    fprintf(fp,"| %-30s|             |","The Complete Lexicon");
    fprintf(fp,"%5d  %5d  %5d  %5d |\n",
                   score->spk[spkr]->n_corr,score->spk[spkr]->n_subs,score->spk[spkr]->n_del,
                   score->spk[spkr]->n_ins);
    fprintf(fp,"|-------------------------------+-------------+");
    fprintf(fp,"---------------------------|\n");
    fprintf(fp,"|      Extra-Lexical Items      |             |");
    fprintf(fp,"                           |\n");
    fprintf(fp,"|-------------------------------+-------------+");
    fprintf(fp,"---------------------------|\n");

    for (set=0; set<lsub_num_set(lsub); set++){
        fprintf(fp,"| %-30s|%5d  %5d |",lsub_set_desc(lsub,set),
                   score->spk[spkr]->n_subs_lsub[set]+
                       score->spk[spkr]->n_del_lsub[set]+score->spk[spkr]->n_corr_lsub[set],
                   score->spk[spkr]->n_hyp_lsub[set]);
        fprintf(fp,"%5d  %5d  %5d  %5d |\n",
                   score->spk[spkr]->n_corr_lsub[set],
                   score->spk[spkr]->n_subs_lsub[set],
                   score->spk[spkr]->n_del_lsub[set],
                   score->spk[spkr]->n_ins_lsub[set]);
    }
    fprintf(fp,"------------------------------------------------------");
    fprintf(fp,"---------------------\n");

}

void print_wrd_cnt_arr_of_lex2(int n, int max, PCIND_T **arr, TABLE_OF_CODESETS *pcs, FILE *fp, char *label, int thresh) {
    static PCIND_T *ind_arr;
    static int *ptr_arr;
    int i,tot,k;

    alloc_singarr(ind_arr,n,PCIND_T);
    alloc_singarr(ptr_arr,n,int);

    /* if there was at least one word marked */
    if (n > 0){
        fprintf(fp,"%-23sTotal                 (%2d)\n",label,n);
	
        for (k=0,i=0;i<n;i++)
	    if (arr[1][i] >= thresh)
		k++;

        fprintf(fp,"%-23sWith >=%3d occurances (%2d)\n\n","",thresh,k);
#if PCIND_SHORT
        sort_short_arr(arr[1],n,ptr_arr,DECREASING);
#else
        sort_int_arr(arr[1],n,ptr_arr,DECREASING);
#endif

        for (tot=0,i=0;i<n;i++){
            /* print the word */
	    if (arr[1][ptr_arr[i]] >= thresh){
		fprintf(fp,"  %3d:  %2d  ->  %s\n",i+1,
			arr[1][ptr_arr[i]],
			lex_word(pcs,arr[0][ptr_arr[i]]));
		tot+=arr[1][ptr_arr[i]];
	    }
        }
        fprintf(fp,"      ------\n");
        fprintf(fp,"        %2d\n",tot);
    }
    free_singarr(ind_arr,PCIND_T);
    free_singarr(ptr_arr,int);
    fprintf(fp,"\n\n");
}

/*************************************************************************/
/*   this procedure takes an parallel array to the lexicon and prints    */
/*   it out.  a non-zero value means that there where n occurences of    */
/*   that word.  the words are sorted by decreasing numbers of occurances*/
/*************************************************************************/
void print_wrd_cnt_arr_of_lex(PCIND_T *ins_del, TABLE_OF_CODESETS *pcs, FILE *fp, char *label, int thresh)
{
    static PCIND_T *ind_arr, *ind_val;
    static int *ptr_arr;
    int num_ind=0,i,tot;

    if (ind_arr == NULL){
        alloc_singarr(ind_arr,lex_num(pcs),PCIND_T);
        alloc_singarr(ind_val,lex_num(pcs),PCIND_T);
        alloc_int_singarr(ptr_arr,lex_num(pcs));
    }
    /* find and mark all of the non_zero words */
    for (i=0;i<lex_num(pcs);i++)
        if (ins_del[i] >= thresh){
            if (num_ind >= lex_num(pcs)){
                fprintf(stderr,"Too many insertions or deletions");
                fprintf(stderr," in spkr sum > %d\n",num_ind);
                exit(-1);
	    }
            ind_val[num_ind] = ins_del[i];
            ind_arr[num_ind] = i;
            num_ind++;
	}

    /* if there was at least one word marked */
    if (num_ind > 0){
        fprintf(fp,"%-23s(%2d)\n",label,num_ind);
        fprintf(fp,"Threshold = %1d\n\n",thresh);
#if PCIND_SHORT
        sort_short_arr(ind_val,num_ind,ptr_arr,DECREASING);
#else
        sort_int_arr(ind_val,num_ind,ptr_arr,DECREASING);
#endif

        for (tot=0,i=0;i<num_ind;i++){
            /* print the word */
            fprintf(fp,"  %3d:  %2d  ->  %s\n",i+1,
                         ins_del[ind_arr[ptr_arr[i]]],
                         lex_word(pcs,ind_arr[ptr_arr[i]]));
            tot+=ins_del[ind_arr[ptr_arr[i]]];
        }
        fprintf(fp,"      ------\n");
        fprintf(fp,"        %2d\n",tot);
	fprintf(fp,"\n\n");
    }
}

/*************************************************************************/
/*   print the confusion pairs in order of decreasing numbers of         */
/*   times confused.                                                     */
/*************************************************************************/
void print_conf_pairs(SCORE *score, int spkr, TABLE_OF_CODESETS *pcs, FILE *fp, char *label, int thresh, int num_elem, PCIND_T *elem_cnt, PCIND_T **elem_list)
{
    int i,tot=0,n_cf=0;
    int *ptr_arr;

    alloc_int_singarr(ptr_arr,score->spk[spkr]->conf_max_pairs);

    /* sort the confusion pairs */
#if PCIND_SHORT
    sort_short_arr(elem_cnt,num_elem,ptr_arr,DECREASING);
#else
    sort_int_arr(elem_cnt,num_elem,ptr_arr,DECREASING);
#endif

    sort_int_arr(elem_cnt,num_elem,ptr_arr,DECREASING);


    /* find the number of pairs with occurances above the threshold */
    for (i=0; i< num_elem; i++)
        if (elem_cnt[ptr_arr[i]] >= thresh)
            n_cf++;

    fprintf(fp,"%-23s(%2d)\n",label,n_cf);
    fprintf(fp,"Threshold = %1d\n\n",thresh);
    for (i=0; i< n_cf; i++){
         fprintf(fp," %3d:   %2d   %20s  ->  %s\n",i+1,
		 elem_cnt[ptr_arr[i]],
 		 lex_word(pcs,elem_list[ptr_arr[i]][0]),
		 lex_word(pcs,elem_list[ptr_arr[i]][1]));
	 tot+=elem_cnt[ptr_arr[i]];
    }
    fprintf(fp,"     -------\n");
    fprintf(fp,"       %3d\n",tot);
    free(ptr_arr);
}

/*********************************************************************/
/*   print the system name and description at the head of a sentence */
/*   level report                                                    */
/*********************************************************************/
void print_system_header(FILE *fp, char *name, char *desc)
{
    fprintf(fp,"========================================");
    fprintf(fp,"=======================================\n\n");
    fprintf(fp,"    SENTENCE LEVEL REPORT FOR THE SYSTEM:\n");
    fprintf(fp,"       Name: %s\n",name);
    fprintf(fp,"       Desc: %s\n\n",desc);
    fprintf(fp,"========================================");
    fprintf(fp,"=======================================\n\n");
}

/*************************************************************************/
/*   print the sentence scoring results for a single sentence            */
/*************************************************************************/
void print_sent_desc(SCORE *score, SENT *sent, SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, char *spkr_str, int spkr, FILE *fp)
{
     static char ref[MAX_SENTENCE_STRING_LEN],
                 hyp[MAX_SENTENCE_STRING_LEN], eval[MAX_SENTENCE_STRING_LEN];
     int s_err, t_err;
     int subset_flag = TRUE, set;

     fprintf(fp,"SPEAKER %s\nSENTENCE %1s\n\n",spkr_str,s_id(sent));
     make_readable_SENT(ref,hyp,eval,MAX_SENTENCE_STRING_LEN,sent,pcs);

     fprintf(fp,"REF:  %s\nHYP:  %s\nEVAL: %s\n\n",ref,hyp,eval);
     fprintf(fp,"Correct               =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_corr,score->snt.s_ref), score->snt.s_corr,
        score->spk[spkr]->n_corr);
     if (subset_flag == TRUE)
        for (set=0; set<lsub_num_set(lsub); set++){
            fprintf(fp,"  %-20s=",lsub_set_desc(lsub,set));
            fprintf(fp,"  %5.1f%%   %2d   (%3d)\n",
                     pct(score->snt.s_corr_lsub[set],score->snt.s_ref), score->snt.s_corr_lsub[set],
                         score->spk[spkr]->n_corr_lsub[set]);
        }
     fprintf(fp,"\n");

     fprintf(fp,"Substitutions         =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_sub,score->snt.s_ref), score->snt.s_sub,
        score->spk[spkr]->n_subs);
     fprintf(fp,"Substitution Alpha/Num=  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_sub_alphnum,score->snt.s_ref), score->snt.s_sub_alphnum,
        score->spk[spkr]->n_subs_alphnum);
     fprintf(fp,"Substitution Mono-Syl =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_sub_mono,score->snt.s_ref), score->snt.s_sub_mono,
        score->spk[spkr]->n_subs_mono);
     if (subset_flag == TRUE)
        for (set=0; set<lsub_num_set(lsub); set++){
            fprintf(fp,"  %-20s=",lsub_set_desc(lsub,set));
            fprintf(fp,"  %5.1f%%   %2d   (%3d)\n",
                     pct(score->snt.s_sub_lsub[set],score->snt.s_ref), score->snt.s_sub_lsub[set],
                         score->spk[spkr]->n_subs_lsub[set]);
        }
     fprintf(fp,"\n");

     fprintf(fp,"Deletions             =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_del,score->snt.s_ref), score->snt.s_del,
        score->spk[spkr]->n_del);
     fprintf(fp,"Deletions of THE      =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_del_THE,score->snt.s_ref), score->snt.s_del_THE,
        score->spk[spkr]->n_del_THE);
     fprintf(fp,"Deletions Alpha/Num   =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_del_alphnum,score->snt.s_ref), score->snt.s_del_alphnum,
        score->spk[spkr]->n_del_alphnum);
     fprintf(fp,"Deletions Mono-Syl    =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_del_mono,score->snt.s_ref), score->snt.s_del_mono,
        score->spk[spkr]->n_del_mono);
     if (subset_flag == TRUE)
        for (set=0; set<lsub_num_set(lsub); set++){
            fprintf(fp,"  %-20s=",lsub_set_desc(lsub,set));
            fprintf(fp,"  %5.1f%%   %2d   (%3d)\n",
                     pct(score->snt.s_del_lsub[set],score->snt.s_ref), score->snt.s_del_lsub[set],
                         score->spk[spkr]->n_del_lsub[set]);
        }
     fprintf(fp,"\n");

     fprintf(fp,"Insertions            =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_ins,score->snt.s_ref), score->snt.s_ins,
        score->spk[spkr]->n_ins);
     fprintf(fp,"Insertions Alpha/Num  =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_ins_alphnum,score->snt.s_ref), score->snt.s_ins_alphnum,
        score->spk[spkr]->n_ins_alphnum);
     fprintf(fp,"Insertions Mono-Syl   =  %5.1f%%   %2d   (%3d)\n",
        pct(score->snt.s_ins_mono,score->snt.s_ref), score->snt.s_ins_mono,
        score->spk[spkr]->n_ins_mono);
     if (subset_flag == TRUE)
        for (set=0; set<lsub_num_set(lsub); set++){
            fprintf(fp,"  %-20s=",lsub_set_desc(lsub,set));
            fprintf(fp,"  %5.1f%%   %2d   (%3d)\n",
                     pct(score->snt.s_ins_lsub[set],score->snt.s_ref), score->snt.s_ins_lsub[set ],
                         score->spk[spkr]->n_ins_lsub[set]);
        }
#ifdef NEW_SM_METHOD
     fprintf(fp,"\n");
     fprintf(fp,"Merges                =  %5.1f%%   %2d   (%3d)\n",
	     pct(st_mrg(),st_ref()), st_mrg(), sp_n_mrg(spkr));
     fprintf(fp,"Splits                =  %5.1f%%   %2d   (%3d)\n",
	     pct(st_spl(),st_ref()), st_spl(), sp_n_spl(spkr));
#endif

     fprintf(fp,"\n");


#ifdef NEW_SM_METHOD
     s_err = st_sub() + st_ins() + st_del() + st_spl() + st_mrg();
     t_err = sp_n_sub(spkr) + sp_n_ins(spkr) +
             sp_n_del(spkr) + sp_n_spl(spkr) + sp_n_mrg(spkr);
#endif
#ifdef OLD_SM_METHOD
     s_err = score->snt.s_sub + score->snt.s_ins + score->snt.s_del;
     t_err = score->spk[spkr]->n_subs + score->spk[spkr]->n_ins +
             score->spk[spkr]->n_del;
#endif
     fprintf(fp,"Errors                =  %5.1f%%   %2d   (%3d)\n",
        pct(s_err,score->snt.s_ref), s_err ,t_err);

     fprintf(fp,"\nRef. words            =          %2d   (%3d)\n",
        score->snt.s_ref, score->spk[spkr]->n_ref);
     fprintf(fp,"Hyp. words            =          %2d   (%3d)\n",
        score->snt.s_hyp, score->spk[spkr]->n_hyp);
     fprintf(fp,"Aligned words         =          %2d   (%3d)\n",
        score->snt.s_align_words, score->spk[spkr]->n_align_words);
     fprintf(fp,"Ref. Mono-Syl Words   =          %2d   (%3d)\n\n",
        score->snt.s_ref_mono_syl, score->spk[spkr]->n_ref_mono_syl);
#ifdef OLD_SM_METHOD
     fprintf(fp,"Acceptable Merges     =          %2d   (%3d)\n",
        score->snt.s_accept_merge, score->spk[spkr]->n_accept_merge);
     fprintf(fp,"Merges                =          %2d   (%3d)\n",
        score->snt.s_merge, score->spk[spkr]->n_merge);
     fprintf(fp,"Acceptable Splits     =          %2d   (%3d)\n",
        score->snt.s_accept_split, score->spk[spkr]->n_accept_split);
     fprintf(fp,"Splits                =          %2d   (%3d)\n",
        score->snt.s_split, score->spk[spkr]->n_split);

     fprintf(fp,"\n");
     print_SM_CAND(sm_cand,pcs,fp);
#endif
     fprintf(fp,"----------------------------------------");
     fprintf(fp,"---------------------------------------\n\n");
} 

/***********************************************************************/
/*   this procedure makes a filename out of the three arguments then   */
/*   tries to open a file of that name, if for any reason the file     */
/*   be opened, stdout is passed back as a file pointer                */
/***********************************************************************/
FILE *make_filename(char *dir, char *name, char *ext)
{
    FILE *tfp;
    char fname[MAX_BUFF_LEN];

    if ((*dir == NULL_CHAR) || (*name == NULL_CHAR) || (*ext == NULL_CHAR))
        return(stdout);
    sprintf(fname,"%s/%s.%s",dir,name,ext);
    if (strlen(fname) > MAX_BUFF_LEN){
        fprintf(stderr,"Error: Filename possible too long > %d\n",
                       MAX_BUFF_LEN);
        fprintf(stderr,"       Writing to stdout\n");
        return(stdout);
    }
    if ((*(fname) == NULL_CHAR) || ((tfp = fopen(fname,"w")) == NULL)){
        fprintf(stderr,"Error: could not open file %s for writing\n",fname);
        return(stdout);
    }
    else
        return(tfp);
}


void add_to_wordlist(int *n, int *max, PCIND_T ***arr, PCIND_T ind, int count){
    int i;
    PCIND_T **a = *arr, **ta;
    
    for (i=0; i<*n; i++)
	if (a[0][i] == ind){
	    a[1][i] += count;
	    return;
	}
    if (i == *max){
	int new_max = (int)(*max * 1.5);
	/* printf("Error: Array too large, expanding from %d to %d.\n",
	       *max,new_max); */

	alloc_2dimZ(ta,2,new_max,PCIND_T,0); 
	for (i=0; i<*n; i++){
	    ta[0][i] = a[0][i];
	    ta[1][i] = a[1][i];
	}
	free_2dimarr(a,2,PCIND_T);
	*arr = ta;
	a = ta;
	*max = new_max;
    }

    a[0][i] = ind;
    a[1][i] = count;
    (*n)++;
    return;
}
