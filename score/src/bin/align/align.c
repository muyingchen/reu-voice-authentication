/**********************************************************************/
/*                                                                    */
/*             FILENAME:  align.c                                     */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This is the main program to align a file of       */
/*                  hypothesis sentences.   In doing so, the program  */
/*                  writes out the alignment file for scoring and     */
/*                  statistical analysis                              */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvp.h>
#include <score/scorelib.h>

#include "alifunc.h"

void set_SYS_ALIGN_from_CONFIG PROTO((CONFIG *cfg, SYS_ALIGN *align_str));
/* loadhyps.c */ void load_and_align_hyp_to_ref_into_SYS_ALIGN PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *hyp_file, char *ref_file, int strip_silence));


/**********************************************************************/
/*   general sizes and defines                                        */
#define PROG_NAME		"align"


CONFIG score_config = {
 /*   intern id,     count,   type,    	groups, value  */
  {  { CFG_CODESET,	1,	CFG_C,		3,	NULL} ,
     { CFG_LEX,		1,	CFG_C,		3,	NULL} ,
     { CFG_LSUBS,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_HOMO,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_REF,		1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_HYP_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_OUT_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_SYS_NAME,	1,	CFG_STR,	NO_GRP,	NULL} ,
     { CFG_SYS_DESC,	1,	CFG_STR,	NO_GRP,	NULL} ,
     { CFG_SHLEX,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SHLSUBS,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SHHOMO,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_STR_SIL,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_NO_GR,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SM_RESCORE,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SM_FOM,	1,	CFG_STR,	NO_GRP,	NULL} ,
     { CFG_FRAG_RESCORE,1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_USE_PHONE ,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_ONE_TO_ONE,	1,	CFG_TGL,	2,	TOGGLE_OFF} ,
     { CFG_ONE_TO_MANY,	1,	CFG_TGL,	2,	TOGGLE_OFF} ,
     { CFG_DUMPARG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_ATIS_FILES,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_RM_FILES,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_TIMIT_FILES,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_WSJ_FILES,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_SWB_FILES,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_SPUID_FILES,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_NTYPE_FILES,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_NO_ID_FILES,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_QSCR,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_QSCR_RAW,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_WWL_FILE,	20,	CFG_C2,		NO_GRP,	NULL} ,
     { CFG_PERMIT_OOCE,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_MRK_FILES,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_CMRK_FILES,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { "",		0,	CFG_TGL,	NO_GRP,	TOGGLE_OFF}}
};
CONFIG *cfg = &score_config;

/* defined in src_lib/score/sysalign.c */
extern int global_use_phonetic_alignments;

/**********************************************************************/
/*    main program for aligning a file of hypothesis sentences.       */
/*    Many extensions have been made for different scoring strategies */
/*    each dependent on the relation for the hypothesis file to the   */
/*    reference file.  The relation may be one to one, or one to many.*/
/*    In either case, the following processing sequence is used:      */
/*       - if one 2 one is used:                                      */
/*            load a line from the hyp and ref file and align them    */
/*       - otherwise:                                                 */
/*            load in the hyp file.                                   */
/*            load reference transcripts and align the sentences      */
/*       - write out the SYS_ALIGN structure                          */
/**********************************************************************/


int main(int argc, char **argv)
{
    SYS_ALIGN *align_str;
    TABLE_OF_CODESETS codesets, *pcs = &(codesets);
    HOMO *homo;
    LEX_SUBSET *lsub;
    int error, *perr = &error;
    void get_code_table_from_old_lex(TABLE_OF_CODESETS *, char *, int *);

/*     mtrf_set_verbose(TRUE);
    mtrf_set_dealloc(TRUE); */
    alloc_and_init_SYS_ALIGN(align_str);
    alloc_CONFIG(cfg);
    set_CONFIG_elem_C(cfg,CFG_SM_FOM,"2.0");
    set_CONFIG_elem_TGL(cfg,CFG_STR_SIL,TOGGLE_ON);
    set_CONFIG_elem_TGL(cfg,CFG_ONE_TO_ONE,TOGGLE_ON);
    set_CONFIG_elem_TGL(cfg,CFG_NTYPE_FILES,TOGGLE_ON);

    print_version(argv[0]);
    cfg_get_args(cfg,argc,argv,"");
    if (CONFIG_elem_TGL(cfg,CFG_DUMPARG) == TOGGLE_ON) dump_CONFIG(cfg);

    /* Check to make sure all the necessary arguements are set */
    {   int failure=FALSE;

	if (! is_CONFIG_group_set(cfg,3,TRUE))
	    failure=TRUE;
	if (! is_CONFIG_set(cfg,CFG_HYP_FILE,TRUE))
	    failure=TRUE;
	if (! is_CONFIG_set(cfg,CFG_REF,TRUE))
	    failure=TRUE;
	if (is_CONFIG_set(cfg,CFG_NO_GR,FALSE) &&
	    (!is_CONFIG_set(cfg,CFG_HOMO,FALSE))){
	    fprintf(stderr,"Error: The no_grammar option '-%s' was selected, ",
		    CFG_NO_GR);
	    fprintf(stderr,"however no\nhomophone file was defined on the ");
	    fprintf(stderr,"command line using the '-%s fname' option\n",
		    CFG_HOMO);
	    failure=TRUE;
	}
	if (strcmp(CONFIG_elem_C(cfg,CFG_OUT_FILE),"") == 0)
	    if ((CONFIG_elem_TGL(cfg,CFG_QSCR) == TOGGLE_OFF) &&
		(CONFIG_elem_TGL(cfg,CFG_QSCR_RAW) == TOGGLE_OFF)){
		fprintf(stderr,"Error: No outputs have been specified.\n");
		fprintf(stderr,"       Specify either -OUTFILE | -SUM | ");
		fprintf(stderr,"-RSUM\n");
		failure=TRUE;
	    }
	if (failure){
	    print_usage_CONFIG(cfg, "align");
	    exit(-1);
	}
    }

    set_SYS_ALIGN_from_CONFIG(cfg,align_str);

    if (strcmp(CONFIG_elem_C(cfg,CFG_CODESET),"") != 0){
        get_code_table2(pcs, CONFIG_elem_C(cfg,CFG_CODESET),
			TRUE /*USE Add File*/,perr);
        if (*perr != 0){
            fprintf(stderr,"Error: Unable to load codeset %s, error code %d\n",
                       CONFIG_elem_C(cfg,CFG_CODESET),*perr);
            exit(-1);
        }
    } else {
        printf("I'm loading the old Style lexicon\n");
        get_code_table_from_old_lex(pcs, CONFIG_elem_C(cfg,CFG_LEX), perr);
        if (*perr != 0){
            fprintf(stderr,"Error: Unable to load Lexicon %s, error code %d\n",
		    CONFIG_elem_C(cfg,CFG_LEX),*perr);
            exit(-1);
        }
    }
    if (CONFIG_elem_TGL(cfg,CFG_SHLEX) == TOGGLE_ON)
	dump_pcode2(pcs->cd[1].pc);
    

    load_HOMO(&homo,pcs,CONFIG_elem_C(cfg,CFG_HOMO));
    if (CONFIG_elem_TGL(cfg,CFG_SHHOMO) == TOGGLE_ON) dump_HOMO(homo,pcs);
    load_LEX_SUBSET(&lsub,pcs,CONFIG_elem_C(cfg,CFG_LSUBS));
    if (CONFIG_elem_TGL(cfg,CFG_SHLSUBS) == TOGGLE_ON)
	dump_LEX_SUBSET(lsub,pcs);

    if (CONFIG_elem_TGL(cfg,CFG_USE_PHONE) == TOGGLE_ON)
	global_use_phonetic_alignments = T;

    if (CONFIG_elem_TGL(cfg,CFG_ONE_TO_ONE) == TOGGLE_ON){
	if (CONFIG_elem_TGL(cfg,CFG_MRK_FILES) == TOGGLE_ON)
	    load_and_align_timemarked_hyp_to_ref_into_SYS_ALIGN(align_str,pcs,
		 CONFIG_elem_C(cfg,CFG_HYP_FILE),
		 CONFIG_elem_C(cfg,CFG_REF),
		 CONFIG_elem_TGL(cfg,CFG_STR_SIL) == TOGGLE_ON);
	else if (CONFIG_elem_TGL(cfg,CFG_CMRK_FILES) == TOGGLE_ON)
	    load_and_align_timemarked_conversations(align_str,pcs,
		 CONFIG_elem_C(cfg,CFG_HYP_FILE),
		 CONFIG_elem_C(cfg,CFG_REF),
		 !(CONFIG_elem_TGL(cfg,CFG_PERMIT_OOCE) == TOGGLE_ON));
	else
	    load_and_align_hyp_to_ref_into_SYS_ALIGN(align_str,pcs,
		 CONFIG_elem_C(cfg,CFG_HYP_FILE),
		 CONFIG_elem_C(cfg,CFG_REF),
		 CONFIG_elem_TGL(cfg,CFG_STR_SIL) == TOGGLE_ON);
    } else {
	if (CONFIG_elem_TGL(cfg,CFG_MRK_FILES) == TOGGLE_ON){
	    fprintf(stderr,"Error: Can not do timemarked alignment using");
	    fprintf(stderr," the one2many hyp to ref correspondence\n");
	    return(-1);
	}
        load_hyp_into_SYS_ALIGN(align_str,pcs,
                         CONFIG_elem_C(cfg,CFG_HYP_FILE),
                         CONFIG_elem_TGL(cfg,CFG_STR_SIL) == TOGGLE_ON);
        load_and_align_ref_from_SYS_ALIGN(align_str,pcs,
                         CONFIG_elem_C(cfg,CFG_REF));
    }

    score_SYS_ALIGN(align_str,pcs,homo,lsub,
                    CONFIG_elem_TGL(cfg,CFG_NO_GR) == TOGGLE_ON);

    if (CONFIG_elem_TGL(cfg,CFG_SM_RESCORE) == TOGGLE_ON){
	rescore_align_for_splits_and_merges(pcs,align_str,
		(double)atof(CONFIG_elem_C(cfg,CFG_SM_FOM)),FALSE);
	/* print_score_for_sm_scoring(align_str, pcs, stdout);	 */
    }

    if (CONFIG_elem_TGL(cfg,CFG_FRAG_RESCORE) == TOGGLE_ON){
	rescore_align_for_fragments(pcs, align_str, FALSE);
    }

    set_checksum(align_str,pcs);
    if (strcmp(CONFIG_elem_C(cfg,CFG_OUT_FILE),"") != 0)
	write_SYS_ALIGN(align_str,CONFIG_elem_C(cfg,CFG_OUT_FILE));

    if (0) dump_SYS_ALIGN(align_str,pcs,lsub,TRUE,TRUE);
    if (0) dump_SYS_ALIGN_by_num(align_str);

    if (CONFIG_elem_TGL(cfg,CFG_QSCR) == TOGGLE_ON)
	quick_print_overall_desc(align_str, CONFIG_elem_C(cfg,CFG_HYP_FILE),
			 CONFIG_elem_TGL(cfg,CFG_SM_RESCORE)==TOGGLE_ON,
			 FALSE);
    if (CONFIG_elem_TGL(cfg,CFG_QSCR_RAW) == TOGGLE_ON)
        quick_print_overall_desc(align_str,CONFIG_elem_C(cfg,CFG_HYP_FILE),
			     CONFIG_elem_TGL(cfg,CFG_SM_RESCORE) == TOGGLE_ON,
				 TRUE);

    if (CONFIG_elem_present_C2(cfg,CFG_WWL_FILE) > 0)
	perform_word_weighted_scoring(align_str, pcs, 
				      CONFIG_elem_C(cfg,CFG_HYP_FILE),
				      TRUE,FALSE,FALSE,
				      CONFIG_elem_present_C2(cfg,CFG_WWL_FILE),
				      CONFIG_elem_C2(cfg,CFG_WWL_FILE),
				      TRUE,TRUE);

    free_SYS_ALIGN(align_str);
    free_LEX_SUBSET(lsub);
    free_HOMO(homo,MAX_HOMO_NUM);
    free_code_table(pcs);
    return(0);
}

void set_SYS_ALIGN_from_CONFIG(CONFIG *cfg, SYS_ALIGN *align_str)
{
    char *ptr; 

    if (CONFIG_elem_TGL(cfg,CFG_NO_GR) == TOGGLE_ON)
        sys_no_gr(align_str) = TRUE;
    else
        sys_no_gr(align_str) = FALSE;

    if (CONFIG_elem_TGL(cfg,CFG_ATIS_FILES) == TOGGLE_ON)
        sys_type(align_str) = ATIS_RESULTS;
    else if (CONFIG_elem_TGL(cfg,CFG_RM_FILES) == TOGGLE_ON)
        sys_type(align_str) = RM_RESULTS;
    else if (CONFIG_elem_TGL(cfg,CFG_WSJ_FILES) == TOGGLE_ON)
        sys_type(align_str) = WSJ_RESULTS;
    else if (CONFIG_elem_TGL(cfg,CFG_SWB_FILES) == TOGGLE_ON)
        sys_type(align_str) = SWB_RESULTS;
    else if (CONFIG_elem_TGL(cfg,CFG_TIMIT_FILES) == TOGGLE_ON){
        sys_type(align_str) = TIMIT_RESULTS;
        set_CONFIG_elem_TGL(cfg,CFG_ONE_TO_ONE,TOGGLE_ON);
        set_CONFIG_elem_TGL(cfg,CFG_ONE_TO_MANY,TOGGLE_OFF);
    }
    else if (CONFIG_elem_TGL(cfg,CFG_SPUID_FILES) == TOGGLE_ON){
      sys_type(align_str) = UNKNOWN_RESULTS_SPKRS_UNIQUE_ID;
    }
    else if (CONFIG_elem_TGL(cfg,CFG_NTYPE_FILES) == TOGGLE_ON){
      sys_type(align_str) = UNKNOWN_RESULTS_NO_SPKR;
    }
    else if (CONFIG_elem_TGL(cfg,CFG_NO_ID_FILES) == TOGGLE_ON){
        sys_type(align_str) = UNKNOWN_RESULTS_NO_ID;
        set_CONFIG_elem_TGL(cfg,CFG_ONE_TO_ONE,TOGGLE_ON);
        set_CONFIG_elem_TGL(cfg,CFG_ONE_TO_MANY,TOGGLE_OFF);
    }
    else {
        sys_type(align_str) = UNKNOWN_RESULTS_SPKRS;
        set_CONFIG_elem_TGL(cfg,CFG_ONE_TO_ONE,TOGGLE_ON);
        set_CONFIG_elem_TGL(cfg,CFG_ONE_TO_MANY,TOGGLE_OFF);
    }

    if ((CONFIG_elem_TGL(cfg,CFG_MRK_FILES) == TOGGLE_ON) || 
	(CONFIG_elem_TGL(cfg,CFG_CMRK_FILES) == TOGGLE_ON))
	sys_attrib(align_str) += ATTRIB_TIMEMRK;

    if ((ptr = CONFIG_elem_C(cfg,CFG_SYS_NAME)) != NULL)
        strncpy(sys_name(align_str),ptr,MAX_NAME_LEN);
    if ((ptr = CONFIG_elem_C(cfg,CFG_SYS_DESC)) != NULL)
        strncpy(sys_desc(align_str),ptr,MAX_DESC_LEN);

}

