/**********************************************************************/
/*                                                                    */
/*             FILENAME:  defs.h                                      */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*             DESC: This include file is used by the stats programs  */
/*                   for general definitions and for all of their     */
/*                   necessary include files                          */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*   general sizes and defines                                        */
#define MAX_FILENAME_LEN	80
#define SILENT			FALSE
#define VERBOSE			TRUE

/**********************************************************************/
/*  Default name for driver files to be loaded                        */
#define PROG_NAME		"stats"
#define DEFAULT_MIN_NUM_GOOD	2

#define MTCH_MCN_FILENAME	"MTCH_MCN.tbl"
#define MTCH_FILENAME		"MTCH.tbl"
#define MCN_FILENAME		"MCN.tbl"

#include <score/scorelib.h>

/* anovar.c */ void compute_anovar PROTO((RANK *rank, int verbose));
/* anovar.c */ int analyze_rank_sum PROTO((RANK *rank, int trt_num, int blk_num, float *sum_arr, int orient, int verbose));
/* anovar.c */ void print_result_of_analyze_rank_sum PROTO((int df, float X2_r, float adjust, char *subject));
/* anovar.c */ float fsum_sqr PROTO((float *arr, int len));
/* anovar.c */ float calc_adjust_for_trt PROTO((RANK *rank));
/* anovar.c */ float calc_adjust_for_blks PROTO((RANK *rank));
/* anovar.c */ void do_multi_comp_for_anovar PROTO((int trt_num, int blk_num, char **trt_names, float *sum_arr, float *ovr_rank_arr, char *title, int *srt_ptr, int verbose));
/* anovar.c */ void calc_stat_ranks PROTO((int trt_num, int blk_num, int **stat_sum, float *sum_arr));
/* anovar.c */ float calc_comp_value PROTO((int trt_num, int blk_num));
/* anovar.c */ void print_ANOVAR_comp_matrix PROTO((int trt_num, int **stat_sum, char **trt_names, char *title, int *srt_ptr));

/* troff.c */  void troff_print_mtch_and_MCN PROTO((SYS_ALIGN_LIST *sa_list, int **sent_McN_winner, int **mtch_pr_winner, char *testname));
/* troff.c */  void troff_print_MCN PROTO((SYS_ALIGN_LIST *sa_list, int **sent_McN_winner, char *testname));
/* troff.c */  void troff_print_mtch PROTO((SYS_ALIGN_LIST *sa_list, int **mtch_pr_winner, char *testname));
/* troff.c */  void print_name_desc PROTO((SYS_ALIGN_LIST *sa_list, FILE *fp));

/* range.c */  void print_gnu_rank_ranges PROTO((RANK *rank, char *percent_desc, char *testname, char *basename));
/* range.c */  void print_rank_ranges PROTO((RANK *rank, char *percent_desc, char *testname));
/* range.c */  void do_blk_ranges PROTO((RANK *rank, int scale, char *p_desc, char *tname));
/* range.c */  void do_trt_ranges PROTO((RANK *rank, int scale, char *p_desc, char *tname));
/* range.c */  void print_range_graph PROTO((int scale, float *high, float *low, float *mean, float *std_dev, float *ovr_rank, char **r_names, int num_ranges, char *r_label, int *ptr_arr));

/* 2_samp_Z.c */ int get_full_sys_name PROTO((char *name, int sys_num, CONFIG *cfg));
/* 2_samp_Z.c */ void do_speaker_stat_analysis PROTO((SYS_ALIGN_LIST *sa_list, CONFIG *cfg, int **w_arr, int **s_arr, int *ns));
/* 2_samp_Z.c */ void do_2_samp_z PROTO((SYS_ALIGN_LIST *sa_list, CONFIG *cfg, int **w_winner, int **s_winner));




