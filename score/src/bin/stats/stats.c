/**********************************************************************/
/*                                                                    */
/*             FILENAME:  stats.c                                     */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains the main program for doing     */
/*                  statistical tests on recognition systems.         */
/*                  Currently, the following tests have been finished:*/
/*                      McNemar's test                                */
/*                      Matched Pairs test                            */
/*                      Analysis of Variance by Ranks                 */
/*                      Segment Analysis for Matched Pairs programs   */
/*                                                                    */
/*                  Theres is an option to print the results of       */
/*                  both the McNemar and Matched pairs test in 'tbl'  */
/*                  troff format.                                     */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvp.h>
#include "defs.h"

void print_out_hdr PROTO((SYS_ALIGN_LIST *sa_list));
int check_num_align_files PROTO((CONFIG *cfg));

CONFIG score_config = {
  /*   intern id,     count,   type,    	groups, value  */
  {  { CFG_LEX,		1,	CFG_C,		1,	NULL} ,
     { CFG_CODESET,	1,	CFG_C,		1,	NULL} ,
     { CFG_LSUBS,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_TESTNAME,	1,	CFG_STR,	NO_GRP,	NULL} ,
     { CFG_ALI_FILE,	20,	CFG_C2,		NO_GRP,	NULL} ,
     { CFG_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,

     { CFG_PCT_FORMULA,	1,	CFG_STR,	NO_GRP,	NULL} ,
     { CFG_MINGOOD,	1,	CFG_STR,	NO_GRP,	NULL} ,

     { CFG_SHLSUBS,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_LPR,		1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_HDR,		1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_MTCHPR,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SENT_MCN,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_LONG_SEG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_AVE_SEG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_ANOVAR,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SIGNTEST,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_WILCOXON,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_2_SAMP,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SIG,		1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_RANGE,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { "G_"CFG_RANGE,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_TROFF,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_COUNT,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_VERBOSE,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_DUMPARG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { "",		0,	CFG_TGL,	NO_GRP,	TOGGLE_OFF}}
};
CONFIG *cfg = &score_config;


/**********************************************************************/
/*   the main program for  "stats"                                    */
/*       1: init the globals and read the command lines               */
/*       2: load the LEXICON and SENT_TYPE files                      */
/*       3: load int the list of SYS_ALIGN structures                 */
/*       4: perform the statistics tests                              */
/**********************************************************************/
int main(int argc, char **argv)
{
    SYS_ALIGN_LIST sa_list;
    TABLE_OF_CODESETS codesets, *pcs = &(codesets);
    LEX_SUBSET *lsub;
    COUNT ****count;
    RANK rank;
    int **mtch_pr_winner, **sent_McN_winner, **sign_winner;
    int **wilcoxon_winner;
    int **zw_winner, **zs_winner, **speaker_w, **speaker_s;
    char *formula_str(char *);
    int error, *perr = &error;
  

    /**** initialization routines */
    print_version(argv[0]);
    alloc_CONFIG(cfg);
    cfg_get_args(cfg,argc,argv,"");

    if (CONFIG_elem_TGL(cfg,CFG_DUMPARG) == TOGGLE_ON) dump_CONFIG(cfg);

    /* Check to make sure all the necessary arguements are set */
    {   int failure=FALSE;

	if (! is_CONFIG_group_set(cfg,1,TRUE))
	    failure=TRUE;
	if (! is_CONFIG_set(cfg,CFG_ALI_FILE,TRUE))
	    failure=TRUE;
	
	if (failure){
	    print_usage_CONFIG(cfg, PROG_NAME);
	    exit(-1);
	}
    }

    if (CONFIG_elem_TGL(cfg,CFG_LPR) == TOGGLE_ON) init_pad_util(FULL_SCREEN);

    check_num_align_files(cfg);

    /**** load necessary data files */
    if (strcmp(CONFIG_elem_C(cfg,CFG_CODESET),"") != 0){
        get_code_table2(pcs, CONFIG_elem_C(cfg,CFG_CODESET),
			TRUE /*USE Add File*/,perr);
        if (*perr != 0){
            fprintf(stderr,"Error: Unable to load codeset %s, error code %d\n",
                       CONFIG_elem_C(cfg,CFG_CODESET),*perr);
            exit(-1);
        }
    } else {
        get_code_table_from_old_lex(pcs, CONFIG_elem_C(cfg,CFG_LEX), perr);
        if (*perr != 0){
            fprintf(stderr,"Error: Unable to load Lexicon %s, error code %d\n",
                       CONFIG_elem_C(cfg,CFG_LEX),*perr);
            exit(-1);
        }
    }
    load_LEX_SUBSET(&lsub,pcs,CONFIG_elem_C(cfg,CFG_LSUBS));

    /**** if requested, dump the loaded in data files */
    if (CONFIG_elem_TGL(cfg,CFG_SHLSUBS) == TOGGLE_ON) dump_LEX_SUBSET(lsub,pcs);

    /**** load in the SYS_ALIGN files that were specified on the com line*/
    load_in_SYS_ALIGN_LIST(&sa_list,CONFIG_elem_C2(cfg,CFG_ALI_FILE),
			   check_num_align_files(cfg),
			   CONFIG_elem_present_C2(cfg,CFG_ALI_FILE),pcs);

    /**** count the errors and correct words for each system */
    alloc_COUNT(count,num_sys((&sa_list)),max_spkrs_SYS_ALIGN_LIST(&sa_list),
                                          max_sents_SYS_ALIGN_LIST(&sa_list));

    /**** alloc a result array for future use */
    alloc_2dimZ(mtch_pr_winner,num_sys((&sa_list)),
		    num_sys((&sa_list)),int,NO_DIFF);
    alloc_2dimZ(sign_winner,num_sys((&sa_list)),
		    num_sys((&sa_list)),int,NO_DIFF);
    alloc_2dimZ(wilcoxon_winner,num_sys((&sa_list)),
		    num_sys((&sa_list)),int,NO_DIFF);
    alloc_2dimZ(sent_McN_winner,num_sys((&sa_list)),
		    num_sys((&sa_list)),int,NO_DIFF);
    alloc_2dimZ(zw_winner,num_sys((&sa_list)),
		    num_sys((&sa_list)),int,NO_DIFF);
    alloc_2dimZ(zs_winner,num_sys((&sa_list)),
		    num_sys((&sa_list)),int,NO_DIFF);
    alloc_2dimZ(speaker_w,num_spkr(sys_i((&sa_list),1)),
		    num_sys((&sa_list)),int,NO_DIFF);
    alloc_2dimZ(speaker_s,num_spkr(sys_i((&sa_list),1)),
		    num_sys((&sa_list)),int,NO_DIFF);
                                          

    count_errors(&sa_list,count);
    count_mtch_snt_err(&sa_list,count);
    if (CONFIG_elem_TGL(cfg,CFG_COUNT) == TOGGLE_ON)
                        dump_COUNT(&sa_list,count);

    /**********************************/
    /**** begin doing the statistics  */
    /**********************************/
    if (CONFIG_elem_TGL(cfg,CFG_HDR) == TOGGLE_ON)
                        print_out_hdr(&sa_list);

    /**** all procedures for matched pairs type analysis */
    if ((CONFIG_elem_TGL(cfg,CFG_MTCHPR) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_TROFF) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_SIG) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_AVE_SEG) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_LONG_SEG) == TOGGLE_ON)){

        /* do the matched pairs analysis */
        if ((CONFIG_elem_TGL(cfg,CFG_MTCHPR) == TOGGLE_ON) ||
            CONFIG_elem_TGL(cfg,CFG_SIG) == TOGGLE_ON ||
            (CONFIG_elem_TGL(cfg,CFG_TROFF) == TOGGLE_ON))
            do_mtch_pairs(&sa_list,pcs,
                          CONFIG_elem_C(cfg,CFG_MINGOOD),
                          CONFIG_elem_C(cfg,CFG_TESTNAME),
                          (CONFIG_elem_TGL(cfg,CFG_MTCHPR) == TOGGLE_ON),
                          (CONFIG_elem_TGL(cfg,CFG_VERBOSE) == TOGGLE_ON),
                          mtch_pr_winner);

        /* do the matched pairs segmentation analysis */
        if ((CONFIG_elem_TGL(cfg,CFG_AVE_SEG) == TOGGLE_ON) ||
            (CONFIG_elem_TGL(cfg,CFG_LONG_SEG) == TOGGLE_ON))
            do_mtch_pairs_seg_analysis(&sa_list,pcs,
                            CONFIG_elem_C(cfg,CFG_TESTNAME),
                            (CONFIG_elem_TGL(cfg,CFG_AVE_SEG) == TOGGLE_ON),
                            (CONFIG_elem_TGL(cfg,CFG_LONG_SEG) == TOGGLE_ON));
    }

    /**** all the procedures for the sentence McNemar's test */
    if ((CONFIG_elem_TGL(cfg,CFG_SENT_MCN) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_SIG) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_TROFF) == TOGGLE_ON)){
        McNemar_sent(&sa_list,count,sent_McN_winner,
                     CONFIG_elem_C(cfg,CFG_TESTNAME),
                     CONFIG_elem_TGL(cfg,CFG_SENT_MCN) == TOGGLE_ON,
                     CONFIG_elem_TGL(cfg,CFG_VERBOSE) == TOGGLE_ON);
    }


    /**** all the procedures for the 2-sample Z test */
/*    if ((CONFIG_elem_TGL(cfg,CFG_2_SAMP) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_SIG) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_TROFF) == TOGGLE_ON)){
*/
    if (CONFIG_elem_TGL(cfg,CFG_2_SAMP) == TOGGLE_ON){
	fprintf(stderr,"Warning: 2 Sample Z test was been temporarily removed\n");
	/* do_2_samp_z(&sa_list,cfg,zw_winner,zs_winner); */
    }
 
    /**** all the procedure for the ANOVAR and range analysis */
    if ((CONFIG_elem_TGL(cfg,CFG_ANOVAR) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_SIGNTEST) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_WILCOXON) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_SIG) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,"G_"CFG_RANGE) == TOGGLE_ON) ||
        (CONFIG_elem_TGL(cfg,CFG_RANGE) == TOGGLE_ON)){
        /* init and fill the RANK structure */
        init_RANK_struct_from_SA_LIST(&rank,&sa_list);
        calc_rank_pct(&rank,count,&sa_list,
                         formula_index(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)));

        /* do the ANOVAR */
        if (CONFIG_elem_TGL(cfg,CFG_ANOVAR) == TOGGLE_ON){
            dump_full_RANK_struct(&rank,"SYSTEM","SPKR",
				  "Sys","Spk",
                         formula_str(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)),
                         CONFIG_elem_C(cfg,CFG_TESTNAME),
                         "System ranks for the speaker",
                         "Speaker ranks for the system");
            compute_anovar(&rank,TRUE);
	}
        /* do the SIGN test */
        if (CONFIG_elem_TGL(cfg,CFG_SIGNTEST) == TOGGLE_ON ||
            CONFIG_elem_TGL(cfg,CFG_SIG) == TOGGLE_ON){
	  perform_signtest(&rank,(CONFIG_elem_TGL(cfg,CFG_VERBOSE) == TOGGLE_ON),
                   CONFIG_elem_TGL(cfg,CFG_SIGNTEST) == TOGGLE_ON,
                   formula_str(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)),
                   formula_index(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)),
                   sign_winner);
	}
        /* do the WILCOXON test */
        if (CONFIG_elem_TGL(cfg,CFG_WILCOXON) == TOGGLE_ON ||
            CONFIG_elem_TGL(cfg,CFG_SIG) == TOGGLE_ON){
          perform_wilcoxon(&rank,(CONFIG_elem_TGL(cfg,CFG_VERBOSE) == TOGGLE_ON),
                   CONFIG_elem_TGL(cfg,CFG_WILCOXON) == TOGGLE_ON,
                   formula_str(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)),
                   formula_index(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)),
                   wilcoxon_winner);
	}
        /* print the range graphs from the RANK structure */
        if (CONFIG_elem_TGL(cfg,CFG_RANGE) == TOGGLE_ON){
            char for_name[100];
            strcpy(for_name,formula_str(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)));
            print_rank_ranges(&rank, for_name,CONFIG_elem_C(cfg,CFG_TESTNAME));
        }
        /* print the GNUPLOT range graphs from the RANK structure */
        if (CONFIG_elem_TGL(cfg,"G_"CFG_RANGE) == TOGGLE_ON){
            char for_name[100];
            strcpy(for_name,formula_str(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)));
            print_gnu_rank_ranges(&rank, for_name,
				  CONFIG_elem_C(cfg,CFG_TESTNAME),"range");
        }
    }

    if (CONFIG_elem_TGL(cfg,CFG_SIG) == TOGGLE_ON) {
        char sign_str[100], wilc_str[100];
        sprintf(sign_str,"Signed Paired Comparison (%s) Test",
                         formula_str(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)));
        sprintf(wilc_str,"Wilcoxon Signed Rank (%s) Test", 
                         formula_str(CONFIG_elem_C(cfg,CFG_PCT_FORMULA)));
        print_composite_significance(&rank,
              (CONFIG_elem_TGL(cfg,CFG_LPR) == TOGGLE_ON) ? FULL_SCREEN :
				     SCREEN_WIDTH,
	      4, 		     
  	      mtch_pr_winner,"Matched Pair Sentence Segment (Word Error) Test","MP",
              sign_winner,sign_str,"SI",
              wilcoxon_winner, wilc_str, "WI",
              sent_McN_winner,"McNemar (Sentence Error) Test","MN",
	      (int **)0,(char *)0, (char *)0,
	      (int **)0,(char *)0, (char *)0,
              TRUE, CONFIG_elem_TGL(cfg,CFG_VERBOSE) == TOGGLE_ON,
              CONFIG_elem_C(cfg,CFG_TESTNAME));
    }

/* 
    do_speaker_stat_analysis(&sa_list,cfg,speaker_w,speaker_s,&num_spkrs);
    print_speaker_stats(&sa_list,num_spkrs,speaker_w,"Speaker Word Error");
    print_speaker_stats(&sa_list,num_spkrs,speaker_s,"Speaker Sent Error");
*/

    /**** run the troff procedures to print the Mcnemar and Matched Pair */
    if (CONFIG_elem_TGL(cfg,CFG_TROFF) == TOGGLE_ON){
        troff_print_mtch_and_MCN(&sa_list,sent_McN_winner,mtch_pr_winner,
                           CONFIG_elem_C(cfg,CFG_TESTNAME));
        troff_print_MCN(&sa_list,sent_McN_winner,
                                          CONFIG_elem_C(cfg,CFG_TESTNAME));
        troff_print_mtch(&sa_list,mtch_pr_winner,
                                          CONFIG_elem_C(cfg,CFG_TESTNAME));
    }

    free_SYS_ALIGN_LIST((&sa_list));
    free_LEX_SUBSET(lsub);
    free_code_table(pcs);

    return(0);
}

/**********************************************************************/
/*   print a header page for the standard printer                     */
/**********************************************************************/
void print_out_hdr(SYS_ALIGN_LIST *sa_list)
{
    int i;
    char pad[SCREEN_WIDTH];
    char *title = "STATISTICAL CALCULATION PACKAGE FOR SPEECH RECOGNITION";
    char *title1 = "SYSTEMS USING DARPA TEST DATABASES";
    char *company = "National Institute of Standards and Technology (NIST)";
    char *author = "By Jonathan G. Fiscus";
    char *date = "July 1989";
    char *sub_t =  "Recognition System Abbreviations";


    set_pad(pad,title);
    printf("\n\n\n\n\n\n\n%s%s\n",pad,title);
    set_pad(pad,title1);
    printf("%s%s\n\n",pad,title1);
    set_pad(pad,company);
    printf("%s%s\n",pad,company);
    set_pad(pad,author);
    printf("%s%s\n",pad,author);
    set_pad(pad,date);
    printf("%s%s\n\n\n",pad,date);

    set_pad(pad,sub_t);
    printf("%s%s\n\n",pad,sub_t);
    set_pad_cent_n(pad,MAX_DESC_LEN+MAX_NAME_LEN+7);
    for (i=0;i<num_sys(sa_list);i++)
        printf("%s%4s  -->  %s\n",pad,sys_name(sys_i(sa_list,i)),
                                      sys_desc(sys_i(sa_list,i)));
    printf("\n\n");
    form_feed();
}

int check_num_align_files(CONFIG *cfg)
{
    int cnt;

    if ((cnt=CONFIG_elem_present_C2(cfg,CFG_ALI_FILE)) < 2){
	fprintf(stderr,"Not enough systems to evaluate %d < 2\n",cnt);
	print_usage_CONFIG(cfg,PROG_NAME);
	exit(-1);
    }
    return(cnt);
}
