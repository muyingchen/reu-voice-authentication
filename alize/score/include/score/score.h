/**********************************************************************/
/*                                                                    */
/*             FILENAME:  score.h                                     */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This is procedure scores an ALIGNMENT STRUCTURE   */
/*                                                                    */
/**********************************************************************/
#define OVERALL_EXT		"sys"
#define OVERALL_RAW_EXT		"sys_raw"
#define OVERALL_DETAIL_EXT	"sys_dtl"
#define SPEAKER_SUMMARY_EXT	"sum"
#define SPEAKER_SENT_EXT	"snt"
#define MAX_LEX_SUBSETS		20

/* #define NEW_SM_METHOD */
#define OLD_SM_METHOD
 
typedef struct spkr_score_struct{
    int n_ref;
    int n_hyp;
    int n_hyp_lsub[MAX_LEX_SUBSETS];
    int n_corr;
    int n_corr_lsub[MAX_LEX_SUBSETS];
    int n_subs;
    int n_subs_alphnum;
    int n_subs_mono;
    int n_subs_lsub[MAX_LEX_SUBSETS];
    int n_ins;
    int n_ins_alphnum;
    int n_ins_mono;
    int n_ins_lsub[MAX_LEX_SUBSETS];
    int n_del;
    int n_del_THE;
    int n_del_alphnum;
    int n_del_mono;
    int n_del_lsub[MAX_LEX_SUBSETS];
    int n_align_words;
    int n_ref_mono_syl;
    int n_accept_merge;
    int n_merge;
    int n_accept_split;
    int n_split;
    int n_in_sub_arr;
    int n_sent;
    int n_sent_errors;
    int n_sent_sub;
    int n_sent_sub_alpha;
    int n_sent_sub_mono; 
    int n_sent_sub_lsub[MAX_LEX_SUBSETS];
    int n_sent_del;
    int n_sent_del_THE;
    int n_sent_del_THE_only;
    int n_sent_del_alpha;
    int n_sent_del_mono;
    int n_sent_del_lsub[MAX_LEX_SUBSETS];
    int n_sent_ins;
    int n_sent_ins_alpha;
    int n_sent_ins_mono;
    int n_sent_ins_lsub[MAX_LEX_SUBSETS];
    int n_sent_spl;
    int n_sent_mrg;
    PCIND_T **conf_word_arr; /* [0] ref wrd  [1] hyp wrd for subs */
    PCIND_T *conf_count_arr;
    int conf_max_pairs;
#ifdef NEW_SM_METHOD
    int n_in_split_arr;
    PCIND_T **split_word_arr; /* [0] ref wrd  [1] hyp wrd for subs */
    PCIND_T *split_count_arr;
    int split_max_pairs;
    int n_in_merge_arr;
    PCIND_T **merge_word_arr; /* [0] ref wrd  [1] hyp wrd for subs */
    PCIND_T *merge_count_arr;
    int merge_max_pairs;
#endif
    int n_ins_word;
    int max_ins_word;
    PCIND_T **ins_word_arr;
    int n_del_word;
    int max_del_word;
    PCIND_T **del_word_arr;
    int n_misr_word;
    int max_misr_word;
    PCIND_T **misr_word_arr;
    int n_sub_word;
    int max_sub_word;
    PCIND_T **sub_word_arr;
} SPKR_SCORE;

typedef struct sent_count_struct{
    int s_ref;
    int s_hyp;
    int s_hyp_lsub[MAX_LEX_SUBSETS];
    int s_corr;
    int s_corr_lsub[MAX_LEX_SUBSETS];
    int s_sub;
    int s_sub_alphnum;
    int s_sub_mono;
    int s_sub_lsub[MAX_LEX_SUBSETS];
    int s_ins;
    int s_ins_alphnum;
    int s_ins_mono;
    int s_ins_lsub[MAX_LEX_SUBSETS];
    int s_del;
    int s_del_THE;
    int s_del_alphnum;
    int s_del_mono;
    int s_del_lsub[MAX_LEX_SUBSETS];
    int s_align_words;
    int s_ref_mono_syl;
    int s_accept_merge;
    int s_merge;
    int s_accept_split;
    int s_split;
}SENT_COUNTS;

typedef struct score_struct{
    int n_of_spk;
    SPKR_SCORE **spk;
    SENT_COUNTS snt;
} SCORE;

#ifdef SKIP 

#define sp_n_spkr()		score->n_of_spk
#define sp_spkr_list()		score->spk
#define sp_spkr_n(_sp)		sp_spkr_list()[_sp]

#define sp_n_ref(_sp)		sp_spkr_n(_sp)->n_ref
#define sp_n_hyp(_sp)		sp_spkr_n(_sp)->n_hyp
#define sp_n_hyp_lsub(_sp,_s)	sp_spkr_n(_sp)->n_hyp_lsub[_s]
#define sp_n_corr(_sp)		sp_spkr_n(_sp)->n_corr
#define sp_n_corr_lsub(_sp,_s)	sp_spkr_n(_sp)->n_corr_lsub[_s]
#define sp_n_sub(_sp)		sp_spkr_n(_sp)->n_subs
#define sp_n_sub_an(_sp)	sp_spkr_n(_sp)->n_subs_alphnum
#define sp_n_sub_mn(_sp)	sp_spkr_n(_sp)->n_subs_mono
#define sp_n_sub_lsub(_sp,_s)	sp_spkr_n(_sp)->n_subs_lsub[_s]
#define sp_n_ins(_sp)		sp_spkr_n(_sp)->n_ins
#define sp_n_ins_an(_sp)	sp_spkr_n(_sp)->n_ins_alphnum
#define sp_n_ins_mn(_sp)	sp_spkr_n(_sp)->n_ins_mono
#define sp_n_ins_lsub(_sp,_s)	sp_spkr_n(_sp)->n_ins_lsub[_s]
#define sp_n_del(_sp)		sp_spkr_n(_sp)->n_del
#define sp_n_del_THE(_sp)	sp_spkr_n(_sp)->n_del_THE
#define sp_n_del_an(_sp)	sp_spkr_n(_sp)->n_del_alphnum
#define sp_n_del_mn(_sp)	sp_spkr_n(_sp)->n_del_mono
#define sp_n_del_lsub(_sp,_s)	sp_spkr_n(_sp)->n_del_lsub[_s]
#define sp_n_ali_wrd(_sp)	sp_spkr_n(_sp)->n_align_words
#define sp_n_ref_mon(_sp)	sp_spkr_n(_sp)->n_ref_mono_syl
#define sp_n_acc_mrg(_sp)	sp_spkr_n(_sp)->n_accept_merge
#define sp_n_mrg(_sp)		sp_spkr_n(_sp)->n_merge
#define sp_n_acc_spl(_sp)	sp_spkr_n(_sp)->n_accept_split
#define sp_n_spl(_sp)		sp_spkr_n(_sp)->n_split
#define sp_n_st(_sp)		sp_spkr_n(_sp)->n_sent
#define sp_n_st_er(_sp)		sp_spkr_n(_sp)->n_sent_errors
#define sp_n_st_sub(_sp)	sp_spkr_n(_sp)->n_sent_sub
#define sp_n_st_sub_an(_sp)	sp_spkr_n(_sp)->n_sent_sub_alpha
#define sp_n_st_sub_mn(_sp)	sp_spkr_n(_sp)->n_sent_sub_mono
#define sp_n_st_sub_lsub(_sp,_s)	sp_spkr_n(_sp)->n_sent_sub_lsub[_s]
#define sp_n_st_del(_sp)	sp_spkr_n(_sp)->n_sent_del
#define sp_n_st_del_THE(_sp) 	sp_spkr_n(_sp)->n_sent_del_THE
#define sp_n_st_del_THE_o(_sp) 	sp_spkr_n(_sp)->n_sent_del_THE_only
#define sp_n_st_del_an(_sp)	sp_spkr_n(_sp)->n_sent_del_alpha
#define sp_n_st_del_mn(_sp)	sp_spkr_n(_sp)->n_sent_del_mono
#define sp_n_st_del_lsub(_sp,_s)	sp_spkr_n(_sp)->n_sent_del_lsub[_s]
#define sp_n_st_ins(_sp)	sp_spkr_n(_sp)->n_sent_ins
#define sp_n_st_ins_an(_sp)	sp_spkr_n(_sp)->n_sent_ins_alpha
#define sp_n_st_ins_mn(_sp)	sp_spkr_n(_sp)->n_sent_ins_mono
#define sp_n_st_ins_lsub(_sp,_s)	sp_spkr_n(_sp)->n_sent_ins_lsub[_s]

#define sp_n_st_spl(_sp)	sp_spkr_n(_sp)->n_sent_spl
#define sp_n_st_mrg(_sp)	sp_spkr_n(_sp)->n_sent_mrg

#define sp_n_conf_arr(_sp)	sp_spkr_n(_sp)->n_in_sub_arr
#define sp_max_conf_pr(_sp)	sp_spkr_n(_sp)->conf_max_pairs
#define sp_conf_arr(_sp)	sp_spkr_n(_sp)->conf_word_arr
#define sp_conf_pr(_sp,_pr)	sp_conf_arr(_sp)[_pr]
#define sp_conf_ref(_sp,_pr)	sp_conf_pr(_sp,_pr)[0]
#define sp_conf_hyp(_sp,_pr)	sp_conf_pr(_sp,_pr)[1]
#define sp_conf_cnt_arr(_sp)	sp_spkr_n(_sp)->conf_count_arr
#define sp_conf_cnt(_sp,_pr)	sp_conf_cnt_arr(_sp)[_pr]

#ifdef NEW_SM_METHOD
#define sp_n_split_arr(_sp)	sp_spkr_n(_sp)->n_in_split_arr
#define sp_split_arr(_sp)	sp_spkr_n(_sp)->split_word_arr
#define sp_max_split_pr(_sp)	sp_spkr_n(_sp)->split_max_pairs
#define sp_split_cnt_arr(_sp)	sp_spkr_n(_sp)->split_count_arr
#define sp_split_cnt(_sp,_pr)	sp_split_cnt_arr(_sp)[_pr]
#define sp_n_merge_arr(_sp)	sp_spkr_n(_sp)->n_in_merge_arr
#define sp_merge_arr(_sp)	sp_spkr_n(_sp)->merge_word_arr
#define sp_max_merge_pr(_sp)	sp_spkr_n(_sp)->merge_max_pairs
#define sp_merge_cnt_arr(_sp)	sp_spkr_n(_sp)->merge_count_arr
#define sp_merge_cnt(_sp,_pr)	sp_merge_cnt_arr(_sp)[_pr]

#define sp_split_pr(_sp,_pr)	sp_split_arr(_sp)[_pr]
#define sp_split_ref(_sp,_pr)	sp_split_pr(_sp,_pr)[0]
#define sp_split_hyp(_sp,_pr)	sp_split_pr(_sp,_pr)[1]

#define sp_merge_pr(_sp,_pr)	sp_merge_arr(_sp)[_pr]
#define sp_merge_ref(_sp,_pr)	sp_merge_pr(_sp,_pr)[0]
#define sp_merge_hyp(_sp,_pr)	sp_merge_pr(_sp,_pr)[1]
#endif

#define sp_del_arr(_sp)		sp_spkr_n(_sp)->deleted_word_arr
#define sp_del_wrd(_sp,_wd)	sp_del_arr(_sp)[_wd]

#define sp_ins_arr(_sp)		sp_spkr_n(_sp)->inserted_word_arr
#define sp_ins_wrd(_sp,_wd)	sp_ins_arr(_sp)[_wd]

#define sp_mis_rec_arr(_sp)	sp_spkr_n(_sp)->misrecognized_word_arr
#define sp_mis_rec_wrd(_sp,_wd)	sp_mis_rec_arr(_sp)[_wd]

#define sp_sub_arr(_sp)		sp_spkr_n(_sp)->substituted_word_arr
#define sp_sub_wrd(_sp,_wd)	sp_sub_arr(_sp)[_wd]

#define st_counts()		score->snt
#define st_ref()		st_counts().s_ref
#define st_hyp()		st_counts().s_hyp
#define st_hyp_lsub_arr()	st_counts().s_hyp_lsub
#define st_hyp_lsub(_s)		st_hyp_lsub_arr()[_s]
#define st_corr()		st_counts().s_corr
#define st_corr_lsub_arr()	st_counts().s_corr_lsub
#define st_corr_lsub(_s)	st_corr_lsub_arr()[_s]
#define st_ins()		st_counts().s_ins
#define st_ins_an()		st_counts().s_ins_alphnum
#define st_ins_mn()		st_counts().s_ins_mono
#define st_ins_lsub_arr()	st_counts().s_ins_lsub
#define st_ins_lsub(_s)		st_ins_lsub_arr()[_s]
#define st_sub()		st_counts().s_sub
#define st_sub_an()		st_counts().s_sub_alphnum
#define st_sub_mn()		st_counts().s_sub_mono
#define st_sub_lsub_arr()	st_counts().s_sub_lsub
#define st_sub_lsub(_s)		st_sub_lsub_arr()[_s]
#define st_del()		st_counts().s_del
#define st_del_THE()		st_counts().s_del_THE
#define st_del_an()		st_counts().s_del_alphnum
#define st_del_mn()		st_counts().s_del_mono
#define st_del_lsub_arr()	st_counts().s_del_lsub
#define st_del_lsub(_s)		st_del_lsub_arr()[_s]
#define st_ali_wrd()		st_counts().s_align_words
#define st_ref_mon()		st_counts().s_ref_mono_syl
#define st_acc_mrg()		st_counts().s_accept_merge
#define st_mrg()		st_counts().s_merge
#define st_acc_spl()		st_counts().s_accept_split
#define st_spl()		st_counts().s_split

#endif



