/**********************************************************************/
/*                                                                    */
/*             FILENAME:  alinterf.c                                  */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This is a functional interface to the alignment   */
/*                  packages.                                         */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvf.h>
#include <score/scorelib.h>


static TABLE_OF_CODESETS static_codesets, *static_pcs = &(static_codesets);
static HOMO *static_homo;
static LEX_SUBSET *static_lsub;
static char *static_hyp_buff=(char *)0, *static_ref_buff=(char *)0;
static int static_buff_size=100;
int static_homo_correct;

/* defined in src_lib/score/sysalign.c */
extern int global_use_phonetic_alignments;

int align_close(SCORE_T *sct)
{
    free_code_table(static_pcs);
    free_HOMO(static_homo,MAX_HOMO_NUM);
    free_LEX_SUBSET(static_lsub);

    /* free score struct */
    free_SENT(sct->sent);
    free_singarr(sct,SCORE_T);
    if (static_hyp_buff != (char *)0) free_singarr(static_hyp_buff, char);
    if (static_ref_buff != (char *)0) free_singarr(static_ref_buff, char);

    return(0);
}

SCORE_T *align_init(char *pcdt_file, char *homo_file, int homo_correct)
{
    int error, *perr = &error;
    SCORE_T *tmp_sc;

    static_homo_correct = homo_correct;

    /* load the table of codesets */
    get_code_table2(static_pcs,pcdt_file,TRUE /*USE Add File*/,perr);
    if (*perr != 0){
	fprintf(stderr,"Error: Unable to load codeset %s, error code %d\n",
		pcdt_file,*perr);
	exit(-1);
    }

    /* load the homophone file */
    load_HOMO(&static_homo,static_pcs,homo_file);
    load_LEX_SUBSET(&static_lsub,static_pcs,"");

    alloc_singarr(tmp_sc,1,SCORE_T);
    alloc_and_init_SENT(tmp_sc->sent,200);

    alloc_singarr(static_hyp_buff,static_buff_size,char);
    alloc_singarr(static_ref_buff,static_buff_size,char);

    return(tmp_sc);
}

int align_strings(char *hyp, char *ref, SCORE_T *score, int do_phone, int do_sm_rscr)
{
    int wrd, maxh=0, maxr=0;

    global_use_phonetic_alignments = (do_phone) ? T : F;

    /* first make a copy of the strings */
    maxh = strlen(hyp);
    maxr = strlen(ref);
    if (maxr > static_buff_size || maxh > static_buff_size){
	/* increase the buffer size */
	free_singarr(static_ref_buff,char);
	free_singarr(static_hyp_buff,char);
	static_buff_size = 10 + ((maxr > maxh) ? maxr : maxh);
	alloc_singarr(static_hyp_buff,static_buff_size,char);
	alloc_singarr(static_ref_buff,static_buff_size,char);
    }
    strcpy(static_ref_buff,ref);
    strcpy(static_hyp_buff,hyp);

    align_hypcstr_and_refcstr_into_SENT(score->sent,
					static_hyp_buff,static_ref_buff,
					static_pcs);

    score_SENT(score->sent,static_homo,static_pcs,
	       static_lsub,static_homo_correct);

    if (do_sm_rscr)
	rescore_SENT_for_splits_and_merges(static_pcs,score->sent,
					   2.0, FALSE, FALSE);

    score->num_ref = score->sent->nref;
    score->num_correct = score->num_inserts = score->num_deletes = 0;
    score->num_subs = score->num_merges = score->num_splits = 0;
    
    for (wrd = 0; s_ref_wrd(score->sent,wrd) != END_OF_WORDS; wrd ++){
	if (is_CORR(s_eval_wrd(score->sent,wrd)))
	    score->num_correct++;
	else if (is_INS(s_eval_wrd(score->sent,wrd)))
	    score->num_inserts++;
	else if (is_DEL(s_eval_wrd(score->sent,wrd)))
	    score->num_deletes++;
	else if (is_SUB(s_eval_wrd(score->sent,wrd)))
	    score->num_subs++;
	else if (is_MERGE(s_eval_wrd(score->sent,wrd)))
	    score->num_merges++;	
	else if (is_SPLIT(s_eval_wrd(score->sent,wrd)))
	    score->num_splits++;	
	else {
	    fprintf(stderr,"Error: unknown evaluation type %x\n",
		    s_eval_wrd(score->sent,wrd));
	    exit(-1);
	}
    }
    score->word_error = pct((score->num_inserts + score->num_deletes +
			     score->num_subs + score->num_merges +
			     score->num_splits),
			    score->num_ref);
    return(0); 
}

void print_SCORE_T(SCORE_T *score, FILE *fp)
{
    char ref_str[1000], hyp_str[1000], *h_strs[3];
    char eval_str[1000], *e_strs[3];

    e_strs[0] = eval_str;
    h_strs[0] = hyp_str;

    fprintf(fp,"         Sentence Recognition Summary\n\n");
    fprintf(fp,"               Word Scores              |      \n");
    fprintf(fp,"   Corr  Ins  Del  Sub  Splits  Merges  |  Word Error\n");
    fprintf(fp,"   %3d   %3d  %3d  %3d   %3d     %3d    |     %5.2f%%\n",
	    score->num_correct,score->num_inserts,score->num_deletes,
	    score->num_subs,score->num_splits,score->num_merges,
	    score->word_error);
    
    make_readable_n_SENT(static_pcs,1,ref_str,h_strs,e_strs,1000,
			 score->sent, (SENT *)0, (SENT *)0, (SENT *)0,
			   (SENT *)0, (SENT *)0, (SENT *)0, (SENT *)0,
			   (SENT *)0, (SENT *)0);
    fprintf(fp,"\nTranscript:\n");
    fprintf(fp,"REF:  %s\n",ref_str);
    fprintf(fp,"HYP:  %s\n",h_strs[0]);
    fprintf(fp,"EVAL: %s\n",e_strs[0]);
    fprintf(fp,"\n");
}
