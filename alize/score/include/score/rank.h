/**********************************************************************/
/*                                                                    */
/*           FILE: rank.h                                             */
/*           WRITTEN BY: Jonathan G. Fiscus                           */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           USAGE: for definition of the RANK structure and          */
/*                  all it's macros for appropriate access            */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*   sizes of the rank structure                                      */
#define MAX_BLOCKS	60
#define MAX_TREATMENTS  60
#define FOR_BLOCKS	TRUE
#define FOR_TREATMENTS  FALSE

/**********************************************************************/
/*  command line values to change the means of calculating percentages*/
#define PER_CORR_REC		'R'
#define TOTAL_ERROR		'E'
#define WORD_ACCURACY		'W'

#define CORR_REC_STR		"Speaker Percent Correctly Recognized"
#define TOT_ERR_STR		"Speaker Word Error Rate (%)"
#define WORD_ACC_STR		"Speaker Word Accuracy Rate (%)"

/**********************************************************************/
/*   typedef for the RANK structure                                   */
typedef struct rank_struct{
   int blocks;                 /* number of blocks */ 
   int treatments;             /* number of treatments */
   char **block_names;         /* string names of blocks */
   char **treatment_names;     /* string names of treatments */
   float *overall_blk_ranks;   /* ranks after ANOVAR */
   float *overall_trt_ranks;   /* ranks after ANOVAR */
   float **trt_ranks;          /* 2DIM array for ranks for trts over blocks */
   float **blk_ranks;          /* 2DIM array for ranks for blocks over trts */
   float **pcts;               /* the actual percentages for trts and blks */
   int *blk_sort_index;        /* indexes that to blocks to sort them */
   int *trt_sort_index;        /* indexes that sorts trts into orders */
} RANK;

/**********************************************************************/
/*   RANK structure access macros                                     */
#define rnk_blks(_r)	_r->blocks
#define rnk_trt(_r)	_r->treatments

#define rnk_t_rank(_r)	_r->trt_ranks
#define rnk_t_rank_arr(_r,_n)	rnk_t_rank(_r)[_n]
#define Vrnk_t_rank(_r,_b,_t)	rnk_t_rank(_r)[_b][_t]

#define rnk_b_rank(_r)	_r->blk_ranks
#define rnk_b_rank_arr(_r,_n)	rnk_b_rank[_n]
#define Vrnk_b_rank(_r,_b,_t)	rnk_b_rank(_r)[_b][_t]

#define rnk_pcts(_r)	_r->pcts
#define rnk_pcts_arr(_r,_n)	_r->pcts[_n]
#define Vrnk_pcts(_r,_b,_t)	rnk_pcts(_r)[_b][_t]

#define rnk_b_name(_r)	_r->block_names
#define Vrnk_b_name(_r,_b)	rnk_b_name(_r)[_b]

#define rnk_t_name(_r)	_r->treatment_names
#define Vrnk_t_name(_r,_t)	rnk_t_name(_r)[_t]

#define ovr_t_rank(_r)		_r->overall_trt_ranks
#define Vovr_t_rank(_r,_t)	ovr_t_rank(_r)[_t]

#define ovr_b_rank(_r)		_r->overall_blk_ranks
#define Vovr_b_rank(_r,_b)	ovr_t_rank(_r)[_b]

#define srt_t_rank(_r)		_r->blk_sort_index
#define Vsrt_t_rank(_r,_t)	srt_t_rank(_r)[_t]

#define srt_b_rank(_r)		_r->trt_sort_index
#define Vsrt_b_rank(_r,_b)	srt_b_rank(_r)[_b]

