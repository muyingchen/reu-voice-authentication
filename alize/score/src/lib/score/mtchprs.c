/**********************************************************************/
/*                                                                    */
/*             FILENAME:  matched_pairs.c                             */
/*             BY:  Jonathan G. Fiscus                                */
/*             DATE: April 14 1989                                    */
/*                   NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY   */
/*                   SPEECH RECOGNITION GROUP                         */
/*                                                                    */
/*             DESCRIPTION:  These programs perform the Matched Pairs */
/*                           tests. which include:                    */
/*                                  SEGMENTATION ANALYSIS             */
/*                                  MATCHED PAIRS TEST FOR SYSTEMS    */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/**********************************************************************/
/*   structures and defines for the sentence segment list             */
#define MAX_SEG			4000
#define MIN_NUM_GOOD		2
#define MAX_SEG_ANAL		10

typedef struct segment_count_struct{
    int number_ref;
    int number_errors_for_hyp1;
    int number_errors_for_hyp2;
} SEG;

/* alloc a segment list */
#define alloc_SEG_LIST(_seg,_num) \
   alloc_2dimarr(_seg,_num,1,SEG);

#define free_SEG_LIST(_seg,_num)   free_2dimarr(_seg,_num,SEG)


/*  macros to access the segment list */
#define seg_n(_sg,_s)		_sg[_s]
#define seg_ref(_sg,_s)		_sg[_s]->number_ref
#define seg_hyp1(_sg,_s)	_sg[_s]->number_errors_for_hyp1
#define seg_hyp2(_sg,_s)	_sg[_s]->number_errors_for_hyp2

/* mtchprs.c */	int do_mtch_pairs_on_sys PROTO((SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, int sys1_ind, int sys2_ind, float *sys1_pct, float *sys2_pct, SEG **seg_l, int *num_seg, float *Z_stat, float *seg_per_sent, int min_num_good, int verbose));
/* mtchprs.c */	void evaluate_SEG PROTO((char *sys1_str, char *sys2_str, SEG **seg_l, int *num_seg, int sent_cnt, int min_good, float *s1_pct, float *s2_pct, float *seg_per_sent, int verbose));
/* mtchprs.c */	int analyze_Z_score PROTO((SYS_ALIGN *sys1, SYS_ALIGN *sys2, float mean, float variance, float std_dev, float Z_stat, int verbose));
/* mtchprs.c */	void test_comp_sents PROTO((TABLE_OF_CODESETS *pcs, SENT *snt1, SENT *snt2, SEG **seg_l, int *num_seg, int min_good));
/* mtchprs.c */	void count_seg PROTO((TABLE_OF_CODESETS *pcs, SENT *snt1, SENT *snt2, int beg1, int beg2, int end1, int end2, SEG **seg_l, int *num_seg, int dbg));

/**********************************************************************/
/*   this procedure is written to analyze the segmentation for        */
/*   varying numbers on the minimum number of good words to bound     */
/*   the beginning and the ends of the segments                       */
/**********************************************************************/
void do_mtch_pairs_seg_analysis(SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, char *t_name, int seg_ave, int seg_long)
{
    int sys1, sys2, min_g;
    int result;
    int **num_seg;
    float **sys1_pct_arr, **sys2_pct_arr, **Z_stat_arr, ***seg_per_sent;

    SEG **seg_l;

    /* do all allocations */
    fprintf(stderr,"Doing segmentation analysis. This is slow so . . . \n\n");
    alloc_int_2dimarr(num_seg,num_sys(sa_list),num_sys(sa_list));
    alloc_float_2dimarr(sys1_pct_arr,num_sys(sa_list),num_sys(sa_list));
    alloc_float_2dimarr(sys2_pct_arr,num_sys(sa_list),num_sys(sa_list));
    alloc_float_2dimarr(Z_stat_arr,num_sys(sa_list),num_sys(sa_list));
    alloc_float_3dimarr_zero(seg_per_sent,MAX_SEG_ANAL,
                             num_sys(sa_list),num_sys(sa_list));
    alloc_SEG_LIST(seg_l,MAX_SEG);

    /* loop for min_good, and compared systems */
    for (min_g=0; min_g < MAX_SEG_ANAL; min_g++){
        fprintf(stderr,"   calculating buffer of  %d\n",min_g);
        for (sys1=0; sys1 <(num_sys(sa_list)-1); sys1++)
            for (sys2=sys1+1; sys2<(num_sys(sa_list)); sys2++){
                result = do_mtch_pairs_on_sys(sa_list,pcs,sys1,sys2,
                                              &(sys1_pct_arr[sys1][sys2]),
                                              &(sys2_pct_arr[sys1][sys2]),
                                              seg_l,
                                              &(num_seg[sys1][sys2]),
                                              &(Z_stat_arr[sys1][sys2]),
                                           &(seg_per_sent[min_g][sys1][sys2]),
                                              min_g+1,FALSE);
        }
    }

    /* if the flag, print the requested analysis */
    if (seg_long)
        print_sent_seg_long_analysis(sa_list,seg_per_sent,t_name);
    if (seg_ave)
        print_sent_seg_averaged_analysis(sa_list,seg_per_sent,t_name);

    free_SEG_LIST(seg_l,MAX_SEG);
    free_2dimarr(num_seg,num_sys(sa_list),int);
    free_2dimarr(sys1_pct_arr,num_sys(sa_list),float);
    free_2dimarr(sys2_pct_arr,num_sys(sa_list),float);
    free_2dimarr(Z_stat_arr,num_sys(sa_list),float);
    free_3dimarr(seg_per_sent,MAX_SEG_ANAL,num_sys(sa_list),float);
}


/**********************************************************************/
/*   this procedure performs a single matched paired analysis for the */
/*   one level of the min_good and returns the results in the winner  */
/*   array                                                            */
/**********************************************************************/
void do_mtch_pairs(SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, char *min_num_good_str, char *test_name, int print_report, int verbose, int **winner)
{
    int sys1, sys2;
    int result;
    int *sum_winner, **num_seg, min_num_good;
    float **sys1_pct_arr, **sys2_pct_arr, **Z_stat_arr, **seg_per_sent;
    SEG **seg_l;
    char *matrix_name = "COMPARISON MATRIX: FOR THE MATCHED PAIRS TEST";
    char *mean_desc = "PECENTAGES ARE MEAN PCT ERROR/SEGMENT.  FIRST # IS LEFT SYSTEM";

    if ((min_num_good = atoi(min_num_good_str)) <= 0){
        fprintf(stderr,"Warning: Minimum seperation by correct words is \n");
        fprintf(stderr,"         too low, setting to %d\n",
                       DEFAULT_MIN_NUM_GOOD);
        min_num_good = DEFAULT_MIN_NUM_GOOD;
    }
    if (!print_report) verbose = FALSE;

    /* all allocations */
    fprintf(stderr,"entering Matched pairs analysis\n");
    alloc_2dimarr(num_seg,num_sys(sa_list),num_sys(sa_list),int);
    alloc_2dimarr(sys1_pct_arr,num_sys(sa_list),num_sys(sa_list),float);
    alloc_2dimarr(sys2_pct_arr,num_sys(sa_list),num_sys(sa_list),float);
    alloc_2dimarr(Z_stat_arr,num_sys(sa_list),num_sys(sa_list),float);
    alloc_2dimZ(seg_per_sent,num_sys(sa_list),num_sys(sa_list),float,0.0);
    alloc_singZ(sum_winner,num_sys(sa_list),int,NO_DIFF);
    alloc_SEG_LIST(seg_l,MAX_SEG);

    /* loop each pair of compared systems */
    for (sys1=0; sys1 <(num_sys(sa_list)-1); sys1++)
        for (sys2=sys1+1; sys2<(num_sys(sa_list)); sys2++){
            result = do_mtch_pairs_on_sys(sa_list,pcs,sys1,sys2,
                                          &(sys1_pct_arr[sys1][sys2]),
                                          &(sys2_pct_arr[sys1][sys2]),
                                          seg_l,
                                          &(num_seg[sys1][sys2]),
                                          &(Z_stat_arr[sys1][sys2]),
                                          &(seg_per_sent[sys1][sys2]),
                                          min_num_good,verbose);
            winner[sys1][sys2] = result;
            sum_winner[sys2] += result * (-1);
            sum_winner[sys1] += result;
	  }

    /* if print_report print the requested analysis */
    if (print_report){
       print_compare_matrix_for_sys(sa_list,winner,matrix_name,test_name,
                                 mean_desc,sys1_pct_arr,sys2_pct_arr,
                                 num_seg,Z_stat_arr,min_num_good);
       form_feed();
    }

    free_SEG_LIST(seg_l,MAX_SEG);
    free_2dimarr(sys1_pct_arr,num_sys(sa_list),float);
    free_2dimarr(sys2_pct_arr,num_sys(sa_list),float);
    free_2dimarr(Z_stat_arr,num_sys(sa_list),float);
    free_2dimarr(seg_per_sent,num_sys(sa_list),float);
    free_singarr(sum_winner,int);
    free_2dimarr(num_seg,num_sys(sa_list),int);
}


int do_mtch_pairs_on_sa_list_systems(SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, int sys1_ind, int sys2_ind, int min_num_good, int verbose)
{
    float sys1_mperr, sys2_mperr, mapsswe_zstat, seg_per_sent;
    int num_seg;
    static SEG **seg_l=(SEG **)0;
    
    if (seg_l == (SEG **)0)
	alloc_SEG_LIST(seg_l,MAX_SEG);
    return(do_mtch_pairs_on_sys(sa_list,pcs,sys1_ind,sys2_ind,&sys1_mperr,
				&sys2_mperr,
				seg_l, &num_seg, &mapsswe_zstat, &seg_per_sent,
				min_num_good,verbose));

}

/**********************************************************************/
/*   the guts of the Matched Pairs program,  this procedure compares  */
/*   2 systems in a SYS_ALIGN_LIST with indexes sys1_ind and sys2_ind */
/*   then goes through each matching sentence then calls another      */
/*   procedure to find the segments and store them in the segment     */
/*   list. after all sentences are compared, the segment list is      */
/*   analyzed                                                         */
/**********************************************************************/
int do_mtch_pairs_on_sys(SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, int sys1_ind, int sys2_ind, float *sys1_pct, float *sys2_pct, SEG **seg_l, int *num_seg, float *Z_stat, float *seg_per_sent, int min_num_good, int verbose)
{
    int spk1, spk2, snt1, snt2, i, sent_cnt=0, *err_diff;
    float mean, std_dev, variance;
    SYS_ALIGN *sys1, *sys2;

    alloc_singZ(err_diff,MAX_SEG,int,0);
    *num_seg = 0;
    sys1 = sys_i(sa_list,sys1_ind);
    sys2 = sys_i(sa_list,sys2_ind);


    for (spk1=0;spk1 < num_spkr(sys1); spk1++){ /* for all speaker sys1 */
        /**** find the matching speaker */
        for (spk2=0;spk2 < num_spkr(sys2); spk2++)
            if (strcmp(spkr_name(sys1,spk1), spkr_name(sys2,spk2)) == 0)
                break;
        /**** the the speakers match, start on the sentences */
        if (spk2 != num_spkr(sys2)){
            /**** for all sents in sys1,spkr1 */
            for (snt1 = 0; snt1 < num_sents(sys1,spk1); snt1++){
                /**** for all sents in sys2,spkr2 */
                for (snt2 = 0; snt2 < num_sents(sys2,spk2); snt2++) 
                   /**** if the sentences are the same, compare them */
                   if(strcmp(sent_id(sys1,spk1,snt1),
                             sent_id(sys2,spk2,snt2)) == 0){
                        test_comp_sents(pcs,sent_n(sys1,spk1,snt1),
                                            sent_n(sys2,spk2,snt2),
                                            seg_l, num_seg, min_num_good);
                        sent_cnt++;
                        break;
                   }
	    }
	} else {
            fprintf(stderr,"Warning: Speaker %s is in system %s but not system %s\n",
                            spkr_name(sys1,spk1),sys_name(sys1),sys_name(sys2));
        }
    }
    /* calculate the percentages of errors for both systems and print */
    /* a report if verbose == TRUE                                    */
    evaluate_SEG(sys_name(sys_i(sa_list,sys1_ind)),
                     sys_name(sys_i(sa_list,sys2_ind)),
                     seg_l,num_seg,sent_cnt,min_num_good,
                     sys1_pct,sys2_pct,seg_per_sent,verbose);

    /* calc an arr with the difference in the errors for each segment */
    for (i=0;i<*num_seg;i++)
        err_diff[i] = seg_hyp1(seg_l,i) - seg_hyp2(seg_l,i);

    calc_mean_var_std_dev_Zstat(err_diff,*num_seg, 
                                &mean,&variance,&std_dev,Z_stat);
    /* Special output routine */
    if (verbose){  
        printf("MTCH_PR_RESULTS (systems: %s %s) (# segs: %d) (Seg per Sent: %5.3f) ",
                     sys_name(sys_i(sa_list,sys1_ind)),sys_name(sys_i(sa_list,sys2_ind)),*num_seg,
                     *seg_per_sent);
        printf("(%% Error: %5.2f %5.2f) (mean: %5.3f) (std dev: %5.3f) (Z Stat: %5.3f) (Stat Diff: %s)\n",
                       *sys1_pct,*sys2_pct,mean,std_dev,*Z_stat,
                       (analyze_Z_score(sys1,sys2,mean,variance,std_dev,*Z_stat,SILENT) == NO_DIFF) ? "No" : "Yes"); 
    }

    free_singarr(err_diff,int);
    return(analyze_Z_score(sys1,sys2,mean,variance,std_dev,*Z_stat,SILENT));
}

/**********************************************************************/
/*   this procedure calculates the percentages of errors for each     */
/*   systems segments,  if verbose == TRUE then a report is printed   */
/*   to stdout for looking at segment counts                          */
/**********************************************************************/
void evaluate_SEG(char *sys1_str, char *sys2_str, SEG **seg_l, int *num_seg, int sent_cnt, int min_good, float *s1_pct, float *s2_pct, float *seg_per_sent, int verbose)
{
    int i, tot_ref=0, tot_err_h1=0, tot_err_h2=0;

    /* calc the pcts */   
    for (i=0;i<*num_seg;i++){
        tot_ref += seg_ref(seg_l,i);
        tot_err_h1 += seg_hyp1(seg_l,i);
        tot_err_h2 += seg_hyp2(seg_l,i);
    }
    (*s1_pct) = pct(tot_err_h1,tot_ref);
    (*s2_pct) = pct(tot_err_h2,tot_ref);
    (*seg_per_sent) = (float)(*num_seg) / (float)sent_cnt;

    /* if needed, print the systems segmentation reports */
    if (verbose){
      printf("      SEGMENTATION REPORT FOR SYSTEMS\n");
      printf("              %4s and %4s\n",sys1_str, sys2_str);
      printf("Minimum Number of Correct Boundary words %1d\n\n",min_good);
      printf("  Number of Segments %2d,     %5.3f per sentence\n",*num_seg,
                              (*seg_per_sent));
      printf("  Number of Sentences %2d\n\n",sent_cnt);
      printf("    # Ref wrds      Err %4s   Err %4s\n",sys1_str, sys2_str);
      printf("---------------------------------------\n");
      if (FALSE){
          for (i=0;i<*num_seg;i++)
              printf("       %4d           %4d        %4d\n",
                  seg_ref(seg_l,i),seg_hyp1(seg_l,i),seg_hyp2(seg_l,i));
          printf("---------------------------------------\n");
      }
      printf("Totals %4d           %4d        %4d\n\n",
                 tot_ref,tot_err_h1,tot_err_h2);
      printf("Pct err              %5.1f%%      %5.1f%%\n\n",
                 *s1_pct,*s2_pct);
/*      printf("MTCH_PR_RESULTS (systems: %s %s) (# segs: %d) (# sents: %d) (Seg per Sent: %5.3f) ",
                       sys1_str,sys2_str,*num_seg,sent_cnt,*seg_per_sent);
      printf("(# ref: %d) (# Error: %d %d) (%% Error: %5.2f %5.2f)\n",tot_ref,tot_err_h1,tot_err_h2,*s1_pct,*s2_pct);*/
      printf("\n\n\n");
    }
             
}


/**********************************************************************/
/*   this function returns whether or not the Z_stat is outside the   */
/*   range of significance, if needed, a verbose report is printed    */
/**********************************************************************/
int analyze_Z_score(SYS_ALIGN *sys1, SYS_ALIGN *sys2, float mean, float variance, float std_dev, float Z_stat, int verbose)
{
    int eval;
    char pad = '\0';

    if (verbose){
      printf("\n%s\t             Analysis of matched pairs comparison\n\n",&pad);
      printf("%s\t\t\t   Comparing %s and %s\n\n",&pad,
                                           sys_name(sys1),sys_name(sys2));
      printf("%s\t       H : means of sentence segment errors are equal.\n",&pad);
      printf("%s\t        0\n\n",&pad);
      eval = print_Z_analysis(Z_stat);
    }
   else
      eval = Z_pass(Z_stat);
   /* if the mean is less then we want the sys1 index to be the winner */
   if (mean < 0.0)
       eval *= (-1);
   return(eval);
}

/**********************************************************************/
/*   this procedure tries to break up sentences into segments and then*/
/*   counts and stores the segment errors and size for later          */
/*   processing.  The algorithm is a modified state transition graph  */
/*   that looks like this:                                            */
/*                                                                    */
/*      the beginning state is b:                                     */
/*      corr means both sentences have the current word correct:      */
/*      error means at least one sentence had an error:               */
/*                              _                                     */
/*                             / \ corr                               */
/*                             v /                                    */
/*                              b <--------------.                    */
/*                              |                |                    */
/*                              |  error         |                    */
/*                              |                |                    */
/*                              V                |                    */
/*        ,-------------------> e<-. error       |                    */
/*        |                     |\_/             |                    */
/*        |                     |                |                    */
/*        |         ,---------> |  good          |                    */
/*        |         |           V                |                    */
/*        |         |  if #good == min_good      |                    */
/*   error|     corr|  then store the segment ---'                    */
/*        |         |           |                                     */
/*        |         |           V                                     */
/*        |         `---------- g                                     */
/*        |                     |                                     */
/*        |                     |                                     */
/*        `---------------------'                                     */
/* Modified: Nov 10, 1994                                             */
/*   The synchronization between reference strings is lost if altern- */
/*   ation is use in the reference strings.  To correct the problem,  */
/*   code was added to re-sync the reference strings if needed.       */
/*   if the sync passes, the state is modified to dependent in the    */
/*   words skipped to for re-synchronization.                         */
/**********************************************************************/
void test_comp_sents(TABLE_OF_CODESETS *pcs, SENT *snt1, SENT *snt2, SEG **seg_l, int *num_seg, int min_good)
{
    int ind1=0, ind2=0, eog1, eog2, state, num_good, dbg = FALSE;

    eog1 = eog2 = 0;
    state = 'b';

    if (dbg) printf("--------------------------------------------------\n");
    if (dbg) printf("   number of adjacent good words %d\n\n",min_good);
    if (dbg) print_readable_n_SENT(pcs,2,snt1,snt2,(SENT *)0,(SENT *)0,
				   (SENT *)0,(SENT *)0,(SENT *)0,(SENT *)0,
				   (SENT *)0,(SENT *)0);
    while (s_eval_wrd(snt1,ind1) != END_OF_WORDS){
	if (dbg) printf("top of loop: state: %c  ind1=%d ind2=%d\n",
			state,ind1,ind2);
        /* if either sent has an insertion mark it and goto state 'e' */
        while ((is_INS(s_eval_wrd(snt1,ind1))) &&
               (s_eval_wrd(snt1,ind1) != END_OF_WORDS))
            ind1++, state = 'e';
        while ((is_INS(s_eval_wrd(snt2,ind2))) &&
               (s_eval_wrd(snt2,ind2) != END_OF_WORDS))
            ind2++, state = 'e';
	if (dbg) printf("  rm insert: state: %c  ind1=%d ind2=%d\n",
			state,ind1,ind2);
	/*  Check to see if there is a synchony error, and fix it ONLY */
	/*  if the evaluation of the words are different               */
	if ((s_ref_wrd(snt1,ind1) != s_ref_wrd(snt2,ind2)) &&
            (s_eval_wrd(snt1,ind1) != s_eval_wrd(snt2,ind2))) {
	    int i1=ind1, i2, synched=FALSE;
	    if (dbg) printf("  Warning: Reference words out of alignment\n");
	    if (dbg) printf("           Attempting to correct\n");
	    /* the algorithm: trace the following network in a depth first  */
	    /* manner until the first synchronization occurs:               */
	    /*   Where A1 is the ref word s_ref_wrd(snt1,i1) . . .          */
	    /*      A1  A2  A3  A4  ||  A1  A2  A3  A4  ||  A1  A2  A3  A4  */
	    /*      | \             ||     /| \         ||    ____//| \     */
            /*      |  \            ||   /  |  \        ||   /    / |  \    */
	    /*      B1  B2  B3  B4  ||  B1  B2  B3  B4  ||  B1  B2  B3  B4  */
	    while ((s_eval_wrd(snt1,i1) != END_OF_WORDS) && !synched){
		i2 = ind2;
		if (dbg) printf("    Starting at %d - %d\n",i1,i2);
		while ((s_eval_wrd(snt2,i2) != END_OF_WORDS) &&
		       (i2 <= (ind2+1+(i1-ind1))) && !synched){
		    if (dbg) printf("        compare %d to %d  ",i1,i2);
		    if (s_ref_wrd(snt1,i1) == s_ref_wrd(snt2,i2))
			synched = TRUE;
		    if (!synched) {
			if (dbg) printf("Not synched\n");
			i2++;
		    } else
			if (dbg) printf("Synched\n");
		}
		if (!synched)
		    i1++;
	    }
	    if (!synched) {
		if (dbg) printf("   I FAILED to synchronize, I'm sorry\n");
	    } else {
		char tstate = state;
		int ii1, ii2;
		for (ii1 = ind1; ii1 <= i1; ii1++)
		    if ( ! is_CORR(s_eval_wrd(snt1,ii1))) state = 'e';
		for (ii2 = ind2; ii2 <= i2; ii2++)
		    if ( ! is_CORR(s_eval_wrd(snt2,ii2))) state = 'e';
		ind1 = i1; ind2 = i2;
		if (dbg) {
		    printf("   Synch WORKED, at %d - %d\n",ind1,ind2);
		    printf("       State was '%c' now '%c'\n",tstate,state);
		}
	    }
	}
        switch (state){
            case 'b':
               /* this state means we haven't found any errors yet */
               /* if both corr, mark the end of the good(eog) for each sent*/
                if ((is_CORR(s_eval_wrd(snt1,ind1)) &&
                    (is_CORR(s_eval_wrd(snt2,ind2)))))
                    eog1 = ind1, eog2 = ind2;
                else
                    state = 'e';
                break;
            case 'e':
                /*  if both are correct, then check to see if the num_good */
                /*  equals the minimum if it is mark the seg and goto 'b'  */
                if ((is_CORR(s_eval_wrd(snt1,ind1))) &&
                    (is_CORR(s_eval_wrd(snt2,ind2)))){
                    num_good = 1;
                    if (num_good == min_good){
			if (dbg) printf("    segment: snt1:%d-%d  snt2:%d-%d\n",
					eog1-(min_good-1),ind1,eog2-(min_good-1),ind2);
                        count_seg(pcs,snt1,snt2,eog1-(min_good-1),
                                  eog2-(min_good-1),ind1,ind2,
                                  seg_l, num_seg, dbg);
                        eog1 = ind1, eog2 = ind2, state = 'b';
		    }
                    else
                        state = 'g';
		}
                break;
            case 'g':
                /* count the good words until the minimum number is found*/
                if ((is_CORR(s_eval_wrd(snt1,ind1))) &&
                    (is_CORR(s_eval_wrd(snt2,ind2)))){
                    num_good++;
                    if (num_good == min_good){
			if (dbg) printf("    segment: snt1:%d-%d  snt2:%d-%d\n",
					eog1-(min_good-1),ind1,eog2-(min_good-1),ind2);
                        count_seg(pcs,snt1,snt2,eog1-(min_good-1),
                               eog2-(min_good-1),ind1,ind2,seg_l,num_seg,dbg);
                        eog1 = ind1, eog2 = ind2, state = 'b';
		    }
                    else
                        state = 'g';
		}
                /* an error was found before the num_good == min_good */
                else
                    state = 'e';
                break;
	}
       
	if (s_eval_wrd(snt1,ind1) != END_OF_WORDS)
	    ind1++;
	if (s_eval_wrd(snt2,ind2) != END_OF_WORDS)
	    ind2++;
	if (dbg) printf("end of loop: state: %c  ind1=%d ind2=%d\n\n",state,ind1,ind2);
    }
    /*  if the state is anything but 'b' then the end of the sentence ends */
    /*  the segment */
    if (state != 'b'){
	if (dbg) printf("    segment: snt1:%d-%d  snt2:%d-%d\n",
			eog1-(min_good-1),ind1-1,eog2-(min_good-1),ind2-1);
        count_seg(pcs,snt1,snt2,eog1-(min_good-1),
                      eog2-(min_good-1),ind1-1,ind2-1,seg_l,num_seg,dbg);
    }
}

/**********************************************************************/
/*   this procedure only stores the errors and reference words in     */
/*   the segment then it stores the info in a list for segments       */
/**********************************************************************/
void count_seg(TABLE_OF_CODESETS *pcs, SENT *snt1, SENT *snt2, int beg1, int beg2, int end1, int end2, SEG **seg_l, int *num_seg, int dbg)
{
    static char *ref_str=NULL, **hyp_strs;
    int err1, err2, ref1, ref2;

    /* do bounds checking, if indexes are < 0 */
    if (beg1 < 0) beg1 = 0;
    if (beg2 < 0) beg2 = 0;

    if (*num_seg >= MAX_SEG){
	fprintf(stderr,"Too many segments to load %d >= MAX_SEG\n",*num_seg);
	exit(-1);
    }

    seg_count(snt1,pcs,beg1,end1,&err1,&ref1);
    seg_count(snt2,pcs,beg2,end2,&err2,&ref2);

    seg_ref(seg_l,*num_seg) = (ref1 + ref2)/2;
    seg_hyp1(seg_l,*num_seg) = err1;
    seg_hyp2(seg_l,*num_seg) = err2;

    /* print the sentence segments to stdout */
    if (dbg){
      if (ref_str == NULL){
        alloc_char_2dimarr(hyp_strs,2,MAX_BUFF_LEN);
        alloc_char_singarr(ref_str,MAX_BUFF_LEN);
      }
      if (ref1 == ref2){
	  make_readable_2_seg(pcs,ref_str,hyp_strs,snt1,beg1,end1,snt2,beg2,end2);
	  printf("Segment# %3d:  #ref %1d\n",*num_seg,ref1);
	  printf("  #Err.    ref:  %s\n\n",ref_str);
	  printf("    %2d     hyp%1d: %s\n",err1,0,hyp_strs[0]);
	  printf("    %2d     hyp%1d: %s\n",err2,1,hyp_strs[1]);
	  printf("\n\n");
      } else {
	  printf("Segment# %3d:  #ref %1d  (%d+%d)/2\n",*num_seg,(ref1+ref2)/2,ref1,ref2);

	  make_readable_seg(pcs,ref_str,hyp_strs,snt1,beg1,end1);
	  printf("  #Err.    ref:  %s\n",ref_str);
	  printf("    %2d     hyp%1d: %s\n",err1,0,hyp_strs[0]);

	  make_readable_seg(pcs,ref_str,hyp_strs,snt2,beg2,end2);
	  printf("  #Err.    ref:  %s\n",ref_str);
	  printf("    %2d     hyp%1d: %s\n",err1,1,hyp_strs[0]);
      }
    }
    (*num_seg)++;
}

void make_readable_seg(TABLE_OF_CODESETS *pcs, char *ref_str, char **hyp_strs, SENT *snt1, int from1, int to1)
{
    int max_ins_len, max_len, snt, tmp;
    static char **space, *ref, **hyp;
    static SENT **s_list;
    static int *h_ind;

    if (space == NULL){
        alloc_char_2dimarr(space,MAX_LEX_WORD_LEN,MAX_LEX_WORD_LEN+1);
        make_space(space,MAX_LEX_WORD_LEN,SPACE);
        alloc_SENT_list(s_list,2);
        alloc_singarr(hyp,2,char *);
        alloc_int_singarr(h_ind,2);
    }

    ref = ref_str;
    *ref  = NULL_CHAR;
    s_list[0] = snt1, hyp[0]=hyp_strs[0], *(hyp[0])=NULL_CHAR, h_ind[0]=from1;

    while (h_ind[0] <= to1){
        /*   check to see if the current word of any sentence is an INSERT */
        /*   if there is, find the maximum length of the INSERTIONS        */
        max_ins_len = 0;
        for (snt=0; snt< 1; snt++){
            if (is_INS(s_eval_wrd(s_list[snt],h_ind[snt]))){
                tmp = strlen(lex_uc_word(pcs,s_hyp_wrd(s_list[snt],h_ind[snt])));
                if (max_ins_len < tmp)
                   max_ins_len = tmp;
	    }
        }
        if (max_ins_len > 0){
            /* there is an insertion somewhere */
            strcpy(ref,space[max_ins_len+1]);
            move_to_null(ref);
            for (snt=0; snt< 1; snt++){
                if (! is_INS(s_eval_wrd(s_list[snt],h_ind[snt]))){
                    /* this word not an insertion, pad with spaces */
                    strcpy(hyp[snt],space[max_ins_len+1]);
                    move_to_null(hyp[snt]);
                }
                else {
                    /* insertion, copy and pad */
                    strcpy(hyp[snt],lex_uc_word(pcs,s_hyp_wrd(s_list[snt],
                                                 h_ind[snt])));
                    move_to_null(hyp[snt]);
                    strcpy(hyp[snt],space[1]);
                    move_to_null(hyp[snt]);
                    h_ind[snt]++;
		}
	    }
	}
        else{
          /* take care of subs, dels, and correct words */
            /* find the maximum length of the current words */
            max_len = strlen(lex_uc_word(pcs,s_ref_wrd(s_list[0],h_ind[0])));
            for (snt=0; snt< 1; snt++){
              if (! is_DEL(s_eval_wrd(s_list[snt],h_ind[snt]))){
                tmp = strlen(lex_uc_word(pcs,s_hyp_wrd(s_list[snt],h_ind[snt])));
                if (max_len < tmp)
                   max_len = tmp;
	      }
            }
            /* copy in the reference word */
            strncpy_pad(ref,lex_lc_word(pcs,s_ref_wrd(s_list[0],h_ind[0])),
                        max_len+1,SPACE);
            move_to_null(ref);
            /* copy in all the hyps words.  if CORR, use lower case words */
            for (snt=0; snt< 1; snt++){
              if (! is_DEL(s_eval_wrd(s_list[snt],h_ind[snt])))
                if (is_CORR(s_eval_wrd(s_list[snt],h_ind[snt])))
                    strncpy_pad(hyp[snt],lex_lc_word(pcs,
                         s_hyp_wrd(s_list[snt],h_ind[snt])),max_len+1,SPACE);
                else
                    strncpy_pad(hyp[snt],lex_uc_word(pcs,
                         s_hyp_wrd(s_list[snt],h_ind[snt])),max_len+1,SPACE);
              else
                strcpy(hyp[snt],space[max_len+1]);
              move_to_null(hyp[snt]);
              h_ind[snt]++;
            }
	}
    }
}

/**********************************************************************/
/* this routine counts the errors and ref words in a segment          */
/**********************************************************************/
void seg_count(SENT *snt, TABLE_OF_CODESETS *pcs, int from, int to, int *err, int *ref)
{
    int i;

    *err = *ref = 0;
    for (i=from;i<= to;i++){
        if (! is_CORR(s_eval_wrd(snt,i)))
           (*err)++;
        if (! is_INS(s_eval_wrd(snt,i)))
           (*ref)++;
    }
}

/**********************************************************************/
/*   when the average segment analysis is ran, this procedure prints  */
/*   the report, for each number of minimum good words.  this report  */
/*   prints the average number of segments per sentence for each      */
/*   number of buffer words (min_good)                                */
/**********************************************************************/
void print_sent_seg_averaged_analysis(SYS_ALIGN_LIST *sa_list, float ***seg_per_sent, char *tname)
{
    int sys1,sys2,num_g,num_comp;
    float tot_sps;
    char pad[FULL_SCREEN];
    char *sub_title="Average Number of Segments Per Sentence Report for";
    char *sub_title1="Changing Numbers of Buffer Words";

    set_pad_cent_n(pad,strlen(sub_title));
    printf("%s%s\n",pad,sub_title);
    set_pad_cent_n(pad,strlen(sub_title1));
    printf("%s%s\n",pad,sub_title1);
    if (*tname != NULL_CHAR){
        set_pad(pad,tname);
        printf("%s%s\n",pad,tname);
    }
    printf("\n\n");

    set_pad(pad,"--------------------------------");
    printf("%s--------------------------------\n",pad);
    printf("%s|   Num Buf   |  Ave Seg/Sent  |\n",pad);
    printf("%s|-------------+----------------|\n",pad);
    for (num_g=0; num_g<MAX_SEG_ANAL; num_g++){
        num_comp = tot_sps = 0;
        for (sys1=0;sys1<num_sys(sa_list);sys1++)
            for (sys2=sys1+1;sys2<num_sys(sa_list);sys2++)
                num_comp++, tot_sps+= seg_per_sent[num_g][sys1][sys2];
        printf("%s|      %2d     |     %5.3f      |\n",
                      pad,num_g+1,(tot_sps/num_comp));
    }
    printf("%s--------------------------------\n",pad);
    form_feed();
}

/**********************************************************************/
/*   when the long segment analysis is ran, this procedure prints     */
/*   the  report, for each number of minimum good words.  this report */
/*   prints the number of segments per sentence for each system       */
/*   by system comparison                                             */
/**********************************************************************/
void print_sent_seg_long_analysis(SYS_ALIGN_LIST *sa_list, float ***seg_per_sent, char *tname)
{
    int i,j,k;
    char pad[FULL_SCREEN];
    char *sub_title="Number of Segments Per Sentence Report for Changing";
    char *sub_title1="Numbers of Buffer Words";

    set_pad_cent_n(pad,strlen(sub_title));
    printf("%s%s\n",pad,sub_title);
    set_pad_cent_n(pad,strlen(sub_title1));
    printf("%s%s\n",pad,sub_title1);
    if (*tname != NULL_CHAR){
        set_pad(pad,tname);
        printf("%s%s\n",pad,tname);
    }
    printf("\n");

    set_pad_cent_n(pad,(num_sys(sa_list)+1) * 15);
    printf("\n%s|-------------",pad);
    for (k=0;k<num_sys(sa_list);k++)
        printf("%13s","--------------");
    printf("|\n");

    printf("%s|             |",pad);
    for (i=0;i<num_sys(sa_list);i++)
        printf("     %4s    |",sys_name(sys_i(sa_list,i)));
    printf("\n");

    for (i=0;i<num_sys(sa_list);i++){
        printf("%s|-------------",pad);
        for (k=0;k<num_sys(sa_list);k++)
            printf("+-------------");
        printf("|\n");
        printf("%s|  %4s    0  |",pad,sys_name(sys_i(sa_list,i)));
        for (j=0;j<num_sys(sa_list);j++)
            if (j > i)
                printf("    %5.3f    |",seg_per_sent[0][i][j]);
            else
                printf("             |");
        printf("\n");
        if (MAX_SEG_ANAL >=2)
            for (k=1; k<MAX_SEG_ANAL; k++){
                printf("%s|         %2d  |",pad,k);
                for (j=0;j<num_sys(sa_list);j++)
                    if (j > i)
                        printf("    %5.3f    |",seg_per_sent[k][i][j]);
                    else
                        printf("             |");
                printf("\n");
	    }
    }
    printf("%s|-------------",pad);
    for (k=0;k<num_sys(sa_list);k++)
        printf("%11s","--------------");
    printf("|\n");
    form_feed();
}

/**********************************************************************/
/*   this procedure prints the Matched pairs report for a single      */
/*   min_num_good length.                                             */
/**********************************************************************/
void print_compare_matrix_for_sys(SYS_ALIGN_LIST *sa_list, int **winner, char *matrix_name, char *tname, char *v_desc, float **sys1_pct_arr, float **sys2_pct_arr, int **num_seg, float **Z_stat, int min_num_good)
{
    int i,j,k,block_size,sys,max_name_len=0;
    int hyphen_len=49, space_len=49;
    char pad[FULL_SCREEN], hyphens[50], spaces[50], sysname_fmt[40];
    char *min_good_title="Minimum Number of Correct Boundary words";

/* init the hyphens and spaces array */
    for (i=0; i<hyphen_len; i++){
         hyphens[i]=HYPHEN; 
         spaces[i]=SPACE;
    }
    hyphens[49]=NULL_CHAR; spaces[49]=NULL_CHAR;
    /* find the largest system length */
    for (sys=0;sys<num_sys(sa_list);sys++)
      if ((i=strlen(sys_name(sys_i(sa_list,sys)))) > max_name_len)
         max_name_len = i;
    block_size = (3+max_name_len);
    sprintf(sysname_fmt," %%%ds |",max_name_len);

    set_pad(pad,matrix_name);
    printf("\n\n\n%s%s\n",pad,matrix_name);
    if (*v_desc != NULL_CHAR){
        set_pad(pad,v_desc);
        printf("%s%s\n",pad,v_desc);
    }
    if (*tname != NULL_CHAR){


        set_pad(pad,tname);
        printf("%s%s\n",pad,tname);
    }
    set_pad_cent_n(pad,strlen(min_good_title)+2);
    printf("%s%s %1d\n",pad,min_good_title,min_num_good);
    printf("\n");

    set_pad_cent_n(pad,(num_sys(sa_list)+1) * block_size);
    printf("\n%s|%s",pad,hyphens+hyphen_len-(block_size-1));
    for (k=0;k<num_sys(sa_list);k++)
        printf("%s",hyphens+hyphen_len-block_size);
    printf("|\n");

    printf("%s|%s|",pad,spaces+space_len-(block_size-1));
    for (i=0;i<num_sys(sa_list);i++)
        printf(sysname_fmt,sys_name(sys_i(sa_list,i)));
    printf("\n");

    for (i=0;i<num_sys(sa_list);i++){
        printf("%s|%s",pad,hyphens+hyphen_len-(block_size-1));
        for (k=0;k<num_sys(sa_list);k++)
            printf("+%s",hyphens+hyphen_len-(block_size-1));
        printf("|\n");
        printf("%s|",pad);
        printf(sysname_fmt,sys_name(sys_i(sa_list,i)));
        for (j=0;j<num_sys(sa_list);j++){
            char *name="";
            int t=0;
            if (j > i){
                if (winner[i][j] == TEST_DIFF)
                    name=sys_name(sys_i(sa_list,j));
                else if (winner[i][j] == NO_DIFF)
                    name="same";
                else
                    name=sys_name(sys_i(sa_list,i));
                t = (block_size-1-strlen(name))/2;
            }
            printf("%s%s%s|",spaces+space_len-t,name,
                             spaces+space_len-(block_size-1-strlen(name)-t));
        }
        printf("\n");
#ifdef you_want_more_output
        printf("%s|             |",pad);
        for (j=0;j<num_sys(sa_list);j++)
            if (j > i)
                    printf(" %3d  %6.2f |",num_seg[i][j],Z_stat[i][j]);
            else
                printf("             |");
        printf("\n");
        printf("%s|             |",pad);
        for (j=0;j<num_sys(sa_list);j++)
            if (j > i)
                printf(" %4.1f%% %4.1f%% |",sys1_pct_arr[i][j],
                                             sys2_pct_arr[i][j]);
            else
                printf("             |");
        printf("\n");
#endif
    }
    printf("%s|%s",pad,hyphens+hyphen_len-(block_size-1));
    for (k=0;k<num_sys(sa_list);k++)
        printf("%s",hyphens+hyphen_len-block_size);
    printf("|\n");
}

