/********************************************************************/
/*  FILE: scorelib.h                                                */
/*  Desc: The main include file for the score library               */
/*  Date: November 2, 1994                                          */
/********************************************************************/

/**********************************************************************/
/*   all necessary include files                                      */

#ifndef SCORELIB_HEADER
#define SCORELIB_HEADER

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include <util/utillib.h>

#include <score/setup.h>
#include <score/version.h>
#include <score/sysalign.h>
#include <score/lex.h>
#include <score/homo.h>
#include <score/lexdst.h>
#include <score/lexsub.h>
#include <score/score.h>
#include <score/count.h>
#include <score/rank.h>
#include <score/score_t.h>
#include <score/alphanum.h>
#include <score/monosyl.h>
#include <score/msrtind.h>
#include <score/spltmrg.h>
#include <score/sentlist.h>


/* alinterf.c */	int align_close PROTO((SCORE_T *sct));
/* alinterf.c */	int align_strings PROTO((char *hyp, char *ref, SCORE_T *score, int do_phone, int do_sm_rscr));
/* alinterf.c */	void print_SCORE_T PROTO((SCORE_T *score, FILE *fp));
/* alphanum.c */	int is_ALPHA_NUM PROTO((ALPHA_NUM *alpha_num, int num));
/* alphanum.c */	void dump_ALPHA_NUM PROTO((ALPHA_NUM *alpha_num, TABLE_OF_CODESETS *pcs));
/* alphanum.c */	void load_ALPHA_NUM PROTO((ALPHA_NUM **alpha_num, TABLE_OF_CODESETS *pcs, char *filename));
/* count.c */		void count_errors PROTO((SYS_ALIGN_LIST *sa_list, COUNT ****count));
/* count.c */		void count_mtch_snt_err PROTO((SYS_ALIGN_LIST *sa_list, COUNT ****count));
/* count.c */		void dump_COUNT PROTO((SYS_ALIGN_LIST *sa_list, COUNT ****count));
/* frg_rscr.c */	void rescore_align_for_fragments PROTO((TABLE_OF_CODESETS *pcs, SYS_ALIGN *align_str, int dbg));
/* gen.c */		char *num2atis PROTO((int num));
/* gen.c */		float sum_float_singarr PROTO((float *arr, int num));
/* gen.c */		int atis2num PROTO((char *str));
/* gen.c */		int convert_num PROTO((char chr));
/* gen.c */		int convert_num2char PROTO((int num));
/* gen.c */		int is_comment PROTO((char *str));
/* gen.c */		int is_comment_info PROTO((char *str));
/* gen.c */		void make_space PROTO((char **space, int num, char chr));
/* gen.c */		void malloc_died PROTO((int len));
/* gen.c */		void print_substr PROTO((FILE *fp, int num, char *str));
/* homo.c */		int appears_in_HOMO PROTO((HOMO *hom, int ind));
/* homo.c */		void dump_HOMO PROTO((HOMO *hom, TABLE_OF_CODESETS *pcs));
/* homo.c */		void erase_HOMO PROTO((HOMO *hom, TABLE_OF_CODESETS *pcs));
/* homo.c */		int is_HOMO PROTO((HOMO *hom, int ind1, int ind2));
/* homo.c */		void load_HOMO PROTO((HOMO **hom, TABLE_OF_CODESETS *pcs, char *filename));
/* lex.c */		void dump_LEXICON PROTO((TABLE_OF_CODESETS *pcs));
/* lex.c */		int is_LEXICON PROTO((TABLE_OF_CODESETS *pcs, char *str));
/* lex.c */		void get_code_table_from_old_lex PROTO((TABLE_OF_CODESETS *ps1, char *fname, int *perr));
/* lexdst.c */		char file_cell PROTO((LEX_DIST *ldist, int r, int c));
/* lexdst.c */		char mem_cell PROTO((LEX_DIST *ldist, int r, int c));
/* lexdst.c */		char set_file_cell PROTO((LEX_DIST *ldist, int r, int c, int val));
/* lexdst.c */		char set_mem_cell PROTO((LEX_DIST *ldist, int r, int c, int val));
/* lexdst.c */		void close_LEX_DIST_file PROTO((LEX_DIST *ldist));
/* lexdst.c */		void close_LEX_DIST_in_file PROTO((LEX_DIST *ldist));
/* lexdst.c */		void dump_LEX_DIST PROTO((LEX_DIST *ldist, FILE *fp));
/* lexdst.c */		void init_LEX_DIST_in_file_input PROTO((LEX_DIST **ldist, char *fname));
/* lexdst.c */		void init_LEX_DIST_in_file_output PROTO((LEX_DIST **ldist, int size, char *fname));
/* lexdst.c */		void init_LEX_DIST_in_mem PROTO((LEX_DIST **ldist, int size, char *fname, int from_file));
/* lexdst.c */		void open_LEX_DIST_file PROTO((LEX_DIST *ldist, char *format));
/* lexdst.c */		void read_in_LEX_DIST PROTO((LEX_DIST *ldist));
/* lexdst.c */		void rewind_LEX_DIST_file PROTO((LEX_DIST *ldist));
/* lexdst.c */		void write_LEX_DIST_from_mem PROTO((LEX_DIST *ldist));
/* lexsub.c */		int is_LEX_SUBSET PROTO((LEX_SUBSET *lsub, short int ind));
/* lexsub.c */		void dump_LEX_SUBSET PROTO((LEX_SUBSET *lsub, TABLE_OF_CODESETS *pcs));
/* lexsub.c */		void find_first_alpha PROTO((char **ptr));
/* lexsub.c */		void load_LEX_SUBSET PROTO((LEX_SUBSET **lsub, TABLE_OF_CODESETS *pcs, char *filename));
/* lexsub.c */		void print_LEX_SUBSET PROTO((LEX_SUBSET *lsub));
/* lexsub.c */		void store_comment_id PROTO((char *from_buff, char *descriptor, int len, char *to_buff));
/* mcn.c */		int do_McNemar PROTO((int **table, char *name1, char *name2, int verbose));
/* mcn.c */		int perform_peregoy_method PROTO((int **table, int verbosely));
/* monosyl.c */		void dump_MONO_SYL PROTO((MONO_SYL *mono_syl, TABLE_OF_CODESETS *pcs));
/* monosyl.c */		int is_MONO_SYL PROTO((MONO_SYL *mono_syl, int num));
/* monosyl.c */		void load_MONO_SYL PROTO((MONO_SYL **mono_syl, TABLE_OF_CODESETS *pcs, char *filename));
/* msort.c */		int getnextgrp PROTO((INDEX indexstruct, int curindex_i, int left, int right, int *grpsptr, int *grpeptr));
/* msort.c */		int getnextindex PROTO((INDEX indexstruct, int *orderlist, int curindex_i));
/* msort.c */		int multisort_INDEX PROTO((INDEX indexstruct, int o0, int o1, int o2, int o3, int o4));
/* msort.c */		int popstack PROTO((void));
/* msort.c */		int procgrp PROTO((int listleft, int listright, INDEX indexstruct, int curindex_i, int *orderlist));
/* msort.c */		int select_pivot PROTO((int *rnk, int l, int r));
/* msort.c */		void partition_dec PROTO((INDEX indexstruct, int curindex_i, int *llen, int *rlen, int *ll, int *lr, int *rl, int *rr, int p, int l, int r));
/* msort.c */		void partition_inc PROTO((INDEX indexstruct, int curindex_i, int *llen, int *rlen, int *ll, int *lr, int *rl, int *rr, int p, int l, int r));
/* msort.c */		void pushstack PROTO((int position));
/* msort.c */		void qsort_INDEX_dec PROTO((INDEX indexstruct, int curindex_i, int l, int r));
/* msort.c */		void qsort_INDEX_inc PROTO((INDEX indexstruct, int curindex_i, int l, int r));
/* mtchprs.c */		void do_mtch_pairs PROTO((SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, char *min_num_good_str, char *test_name, int print_report, int verbose, int **winner));
/* mtchprs.c */		int do_mtch_pairs_on_sa_list_systems PROTO((SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, int sys1_ind, int sys2_ind, int min_num_good, int verbose));
/* mtchprs.c */		void do_mtch_pairs_seg_analysis PROTO((SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, char *t_name, int seg_ave, int seg_long));
/* mtchprs.c */		void make_readable_seg PROTO((TABLE_OF_CODESETS *pcs, char *ref_str, char **hyp_strs, SENT *snt1, int from1, int to1));
/* mtchprs.c */		void print_compare_matrix_for_sys PROTO((SYS_ALIGN_LIST *sa_list, int **winner, char *matrix_name, char *tname, char *v_desc, float **sys1_pct_arr, float **sys2_pct_arr, int **num_seg, float **Z_stat, int min_num_good));
/* mtchprs.c */		void print_sent_seg_averaged_analysis PROTO((SYS_ALIGN_LIST *sa_list, float ***seg_per_sent, char *tname));
/* mtchprs.c */		void print_sent_seg_long_analysis PROTO((SYS_ALIGN_LIST *sa_list, float ***seg_per_sent, char *tname));
/* mtchprs.c */		void seg_count PROTO((SENT *snt, TABLE_OF_CODESETS *pcs, int from, int to, int *err, int *ref));
/* perf_ali.c */	void SENT_to_ali_int PROTO((SENT *ts, ALIGNMENT_INT *al_int));
/* perf_ali.c */	int align_ali_int_copy_to_SENT PROTO((ALIGNMENT_INT *aln_base, ALIGNMENT_INT *aln_out, SENT *ts));
/* perf_ali.c */	int align_hypistr_refcstr_into_SENT PROTO((SENT *ts, ALIGNMENT_INT *aln_base, ALIGNMENT_INT *aln_best, char *refcstr, TABLE_OF_CODESETS *pcs));
/* perf_ali.c */	int align_hypntistr_and_refcstr_into_SENT PROTO((SENT *ts, WCHAR_T *hypntistr, char *refcstr, TABLE_OF_CODESETS *pcs));
/* perf_ali.c */	int copy_istr PROTO((WCHAR_T *istr1, WCHAR_T *istr2));
/* perf_ali.c */	int copy_ntistr2istr PROTO((WCHAR_T *ntistr, int numnti, WCHAR_T *istr, int numi));
/* perf_ali.c */	int string_has_alternates PROTO((char *str));
/* perf_ali.c */	void ali_int_to_SENT PROTO((SENT *ts, ALIGNMENT_INT *al_int));
/* perf_ali.c */	void align_hypcstr_and_refcstr_into_SENT PROTO((SENT *ts, char *hypcstr, char *refcstr, TABLE_OF_CODESETS *pcs));
/* perf_ali.c */	void copy_ali_int_and_WTOKE_to_SENT PROTO((SENT *ts, ALIGNMENT_INT *al_int, WTOKE_STR1 *ref_segs, WTOKE_STR1 *hyp_segs));
/* qscore.c */    int count_sms PROTO((char *str));
/* qscore.c */    void print_score_for_sm_scoring PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, FILE *fp));
/* qscore.c */    void quick_print_overall_desc PROTO((SYS_ALIGN *align_str, char *, int do_sm, int do_raw));
/* rankutil.c */	char *formula_str PROTO((char *str));
/* rankutil.c */	int formula_index PROTO((char *str));
/* rankutil.c */	void calc_rank_pct PROTO((RANK *rank, COUNT ****count, SYS_ALIGN_LIST *sa_list, int calc_formula));
/* rankutil.c */	void dump_full_RANK_struct PROTO((RANK *rank, char *t_name, char *b_name, char *blk_label, char *trt_label, char *formula_str, char *test_name, char *blk_desc, char *trt_desc));
/* rankutil.c */	void init_RANK_struct_from_SA_LIST PROTO((RANK *rank, SYS_ALIGN_LIST *sa_list));
/* rankutil.c */	void print_composite_significance PROTO((RANK *rank, int pr_width, int ntest, int **w1, char *desc1, char *str1, int **w2, char *desc2, char *str2, int **w3, char *desc3, char *str3, int **w4, char *desc4, char *str4, int **w5, char *desc5, char *str5, int **w6, char *desc6, char *str6, int matrix, int report, char *test_name));
/* rankutil.c */	void print_n_winner_comp_matrix PROTO((RANK *rank, int ***wins, char **win_ids, int win_cnt, int page_width));
/* rankutil.c */	void print_n_winner_comp_report PROTO((RANK *rank, int ***wins, char **win_ids, char **win_str, char **win_desc, int win_cnt, int page_width, char *testname));
/* rankutil.c */	void print_speaker_stats PROTO((SYS_ALIGN_LIST *sa_list, int num_spkrs, int **speaker_matrix, char *title));
/* rankutil.c */	void print_trt_comp_matrix_for_RANK_one_winner PROTO((int **winner, RANK *rank, char *title, char *formula_str, char *block_id));
/* rankutil.c */	void rank_on_pcts PROTO((RANK *rank, int ordering));
/* rm_sil.c */		void remove_silence PROTO((char *buff, TABLE_OF_CODESETS *pcs));
/* score.c */		void score_SA_LIST PROTO((SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, HOMO *homo, LEX_SUBSET *lsub, int no_gram_case));
/* score.c */		void score_SENT PROTO((SENT *sent, HOMO *homo, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int no_gram_case));
/* score.c */		void score_SYS_ALIGN PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, HOMO *homo, LEX_SUBSET *lsub, int no_gram_case));
/* scrprnt.c */		void print_score_SYS_ALIGN PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, ALPHA_NUM *alpha_num, MONO_SYL *mono_syl, SPLT_MRG *sp_mrg, int overall_dtl, int overall, int overall_raw, int spkr_lvl, int sent_lvl, char *output_dir, char *ali_fname));
/* sentlist.c */	int is_in_DB_ST_LIST PROTO((DB_ST_LIST *dbl, int type, int num));
/* sentlist.c */	void make_DB_ST_LIST_fr_SYS_ALIGN PROTO((SYS_ALIGN *align_str, DB_ST_LIST **db_st_list));
/* sentmcn.c */		void McNemar_sent PROTO((SYS_ALIGN_LIST *sa_list, COUNT ****count, int **winner, char *testname, int print_results, int verbose));
/* sentmcn.c */		int do_McNemar_by_sent PROTO((SYS_ALIGN_LIST *sa_list, COUNT ****count, int sys1, int sys2, int verbose));
/* sentmcn.c */		void print_compare_matrix_for_sent_M PROTO((SYS_ALIGN_LIST *sa_list, COUNT ****count, int **winner, int *s_winner, char *tname));
/* signtest.c */	int compute_signtest_for_treatment PROTO((RANK *rank, int treat1, int treat2, char *block_id, char *formula_str, int verbose, int zero_is_best));
/* signtest.c */	void perform_signtest PROTO((RANK *rank, int verbose, int report, char *formula_str, int formula_id, int **winner));
/* signtest.c */	int sign_test_analysis PROTO((int num_a, int num_b, int num_z, char *str_a, char *str_b, int str_z, float alpha, int verbose, char *treat1_str, char *treat2_str, int *tptr, int zero_is_best));
/* sm_rscr.c */		int add_merge_to_lexicon PROTO((TABLE_OF_CODESETS *pcs, WCHAR_T i1, WCHAR_T i2));
/* sm_rscr.c */		int count_words_in_spilts_merges PROTO((char *str));
/* sm_rscr.c */		void rescore_SENT_for_splits_and_merges PROTO((TABLE_OF_CODESETS *pcs, SENT *sent, double fom_thresh, int v, int has_time_marks));
/* sm_rscr.c */		void rescore_align_for_splits_and_merges PROTO((TABLE_OF_CODESETS *pcs, SYS_ALIGN *align_str, double fom_thresh, int v));
/* sm_rscr.c */		double compute_time_sm_fom PROTO((TABLE_OF_CODESETS *pcs, char lorr, char sorm, SENT *sent, int ind1, int ind2));
/* spltmrg.c */		int count_SM_CAND_type_above_thresh PROTO((SM_CAND *sm_cand, int typea, int typeb, float thresh));
/* spltmrg.c */		double compute_sm_fom PROTO((TABLE_OF_CODESETS *pcs, char lorr, char sorm, WCHAR_T ina1, WCHAR_T ina2, WCHAR_T inb1, WCHAR_T inb2));
/* spltmrg.c */		int is_SPLT PROTO((SPLT_MRG *sp_mrg, SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, PCIND_T mrg, PCIND_T spl1, PCIND_T spl2, char lorr));
/* spltmrg.c */		int is_MRG PROTO((SPLT_MRG *sp_mrg, SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, PCIND_T mrg, PCIND_T spl1, PCIND_T spl2, char lorr));
/* spltmrg.c */		void sort_SM_CAND PROTO((SM_CAND *sm_cand));
/* spltmrg.c */		void print_SM_CAND PROTO((SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, FILE *fp));
/* spltmrg.c */		void print_SM_CAND_type PROTO((SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, int type, FILE *fp));
/* spltmrg.c */		void load_SPLT_MRG PROTO((SPLT_MRG **sp_mrg, TABLE_OF_CODESETS *pcs, char *filename));
/* spltmrg.c */		void dump_SPLT_MRG PROTO((SPLT_MRG *sp_mrg, TABLE_OF_CODESETS *pcs));
/* spltmrg.c */		void expand_SM_CAND PROTO((SM_CAND *sm_cand));
/* spltmrg.c */		void free_SM_CAND PROTO((SM_CAND *sm_cand));
/* spltmrg.c */		void init_SM_CAND PROTO((SM_CAND *sm_cand));

/* sysalign.c */	int a_diff_in_sent_exists PROTO((SYS_ALIGN_LIST *sa_list, int *spkr_ptr, int *sent_ptr));
/* sysalign.c */	int a_diff_score_in_sent_exists PROTO((SYS_ALIGN_LIST *sa_list, int *spkr_ptr, int *sent_ptr));
/* sysalign.c */	void copy_marker PROTO((short int eval, char *eval_buff, char *marker, int len));
/* sysalign.c */	void count_sent_score PROTO((SENT *sent, int *corr, int *sub, int *ins, int *del, int *spl, int *mrg));
/* sysalign.c */	void dump_SA_LIST PROTO((SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int all, int diff, int score_diff, int errors, int word_indexes));
/* sysalign.c */	void copy_SENT PROTO((SENT *to, SENT *from));
/* sysalign.c */	void dump_SENT PROTO((SENT *sent));
/* sysalign.c */	void dump_SENT_wrds PROTO((TABLE_OF_CODESETS *pcs, SENT *sent));
/* sysalign.c */	void dump_SENT_wwt PROTO((TABLE_OF_CODESETS *pcs, SENT *sent));
/* sysalign.c */	void dump_SYS_ALIGN PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int show_scores, int show_times));
/* sysalign.c */	void dump_SYS_ALIGN_by_num PROTO((SYS_ALIGN *align_str));
/* sysalign.c */	int errors_exist PROTO((SYS_ALIGN_LIST *sa_list, int *spkr_ptr, int *sent_ptr));
/* sysalign.c */	void fgetln PROTO((char *str, int len, FILE *fp));
/* sysalign.c */	void find_lexeme_occurance PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, PCIND_T lex_id));
/* sysalign.c */	int id_equal PROTO((char *hyp_id, char *ref_id, int results_type));
/* sysalign.c */	int compute_checksum PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, unsigned int *checksum));
/* sysalign.c */	int pass_checksum PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs));
/* sysalign.c */	int load_in_SYS_ALIGN PROTO((SYS_ALIGN **sys_align, char *filename, TABLE_OF_CODESETS *pcs));
/* sysalign.c */	int load_in_SYS_ALIGN_LIST PROTO((SYS_ALIGN_LIST *sa_list, char **name_list, int num_names, int max_num_sys, TABLE_OF_CODESETS *pcs));
/* sysalign.c */	void make_readable_2_seg PROTO((TABLE_OF_CODESETS *pcs, char *ref_str, char **hyp_strs, SENT *snt1, int from1, int to1, SENT *snt2, int from2, int to2));
/* sysalign.c */	void make_readable_SENT PROTO((char *ref, char *hyp, char *eval, int buff_len, SENT *sent, TABLE_OF_CODESETS *pcs));
/* sysalign.c */	int make_readable_n_SENT PROTO((TABLE_OF_CODESETS *pcs, int num_snt, char *ref_str, char **h_strs, char **e_strs, int buff_len, SENT *snt0, SENT *snt1, SENT *snt2, SENT *snt3, SENT *snt4, SENT *snt5, SENT *snt6, SENT *snt7, SENT *snt8, SENT *snt9));
/* sysalign.c */	int max_sents_SYS_ALIGN_LIST PROTO((SYS_ALIGN_LIST *sa_list));
/* sysalign.c */	int max_spkrs_SYS_ALIGN_LIST PROTO((SYS_ALIGN_LIST *sa_list));
/* sysalign.c */	void print_readable_n_SENT PROTO((TABLE_OF_CODESETS *pcs, int num_snt, SENT *snt0, SENT *snt1, SENT *snt2, SENT *snt3, SENT *snt4, SENT *snt5, SENT *snt6, SENT *snt7, SENT *snt8, SENT *snt9));
/* sysalign.c */	void print_sent PROTO((SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, int *spkr_ptr, int *sent_ptr, int all, int diff, int score_diff, int errors, int word_indexes));
/* sysalign.c */	int read_SYS_ALIGN PROTO((SYS_ALIGN **sys_align, char *filename));
/* sysalign.c */	void rm_HOMO_CORR_from_SYS_ALIGN PROTO((SYS_ALIGN *align_str));
/* sysalign.c */	void rm_HOMO_CORR_from_SYS_ALIGN_LIST PROTO((SYS_ALIGN_LIST *sa_list));
/* sysalign.c */	void set_checksum PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs));
/* sysalign.c */	void write_SYS_ALIGN PROTO((SYS_ALIGN *sys_align, char *filename));
/* sysalign.c */	SCORE_T *align_init PROTO((char *pcdt_file, char *homo_file, int homo_correct));
/* sysalign.c */	void sort_time_marked_SENT PROTO((SENT *sent));
/* wilcoxon.c */	int compute_wilcoxon_for_treatment PROTO((RANK *rank, int treat1, int treat2, char *block_id, char *formula_str, int verbose, int zero_is_best));
/* wilcoxon.c */	float wilcoxon_stat PROTO((int n, int rank_sum));
/* wilcoxon.c */	void perform_wilcoxon PROTO((RANK *rank, int verbose, int report, char *formula_str, int formula_id, int **winner));
/* wilcoxon.c */	int wilcoxon_test_analysis PROTO((int num_a, int num_b, float sum_a, float sum_b, char *treat1_str, char *treat2_str, float alpha, int verbose, int zero_is_best));
/* wwscr.c */           int perform_word_weighted_scoring PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *, int do_weight_one, int do_phone_weight, int do_syllables, int n_wwlf, char **wwl_files, int make_sum, int make_ovrall));


#endif

