/**********************************************************************/
/*                                                                    */
/*             FILENAME:  splt_mrg.h                                  */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains typedefs and macro definitions */
/*                  for using SPLT_MRG and SM_CAND structures         */
/*                                                                    */
/**********************************************************************/

#define MAX_SPLT_MRG	50

/**********************************************************************/
/*   SPLT_MRG declarations and macros                                 */
/**********************************************************************/
typedef struct splt_mrg_set_struct{
   PCIND_T merge;                    /* lex index for the merged word */
   PCIND_T split1;                   /* lex index for the first split word */
   PCIND_T split2;                   /* lex index for the second split word */
} SPLT_MRG_SET;

typedef struct sp_mg_struct{
   int num;                      /* number of SPLT_MRG_SETs */
   SPLT_MRG_SET **set_list;      /* SPLT_MRG_SET list */
} SPLT_MRG;

#define sp_mg_num(_sm)		_sm->num
#define sp_mg_slist(_sm)	_sm->set_list
#define sp_mg_set(_sm,_s)	sp_mg_slist(_sm)[_s]
#define sp_mg_mrg(_sm,_s)	sp_mg_set(_sm,_s)->merge
#define sp_mg_spl1(_sm,_s)	sp_mg_set(_sm,_s)->split1
#define sp_mg_spl2(_sm,_s)	sp_mg_set(_sm,_s)->split2

/*********************************************************************/
/*   init a SPLT_MRG structured                                      */
#define alloc_and_init_SPLT_MRG(_sm,_num_sets) \
    { \
      alloc_singarr(_sm,1,SPLT_MRG); \
      sp_mg_num(_sm) = 0; \
      alloc_2dimarr(sp_mg_slist(_sm),_num_sets,1,SPLT_MRG_SET); \
    } 

#define free_SPLT_MRG(_sm,_num_sets) \
    { \
      free_2dimarr(sp_mg_slist(_sm),_num_sets,SPLT_MRG_SET); \
      free_singarr(_sm,SPLT_MRG); \
    } 

/**********************************************************************/
/*   SM_CAND declarations and macros                                  */
/**********************************************************************/

#define MAX_NUM_CAND	4000

typedef struct spl_mrg_cand_struct{
    int type;                       /* labelled type of set */
    double fom;
    PCIND_T merge;                    /* lex index for the merged word */
    PCIND_T split1;                   /* lex index for the first split word */
    PCIND_T split2;                   /* lex index for the second split word */
} SPLT_MRG_CAND;

typedef struct cand_list{
    int max_candidates;             /* set the max candidate number */
    int num_candidates;             /* number of candidate sets */
    int base_candidates;            /* base candidate set to print from */
    SPLT_MRG_CAND **candidate_list;  /* candidate set list */
} SM_CAND;

#define max_cand(_cd)		_cd->max_candidates
#define num_cand(_cd)		_cd->num_candidates
#define base_cand(_cd)		_cd->base_candidates
#define cand_list(_cd)		_cd->candidate_list
#define cand_n(_cd,_n)		cand_list(_cd)[_n]
#define cand_type(_cd,_n)	cand_n(_cd,_n)->type
#define cand_fom(_cd,_n)	cand_n(_cd,_n)->fom
#define cand_spl1(_cd,_n)	cand_n(_cd,_n)->split1
#define cand_spl2(_cd,_n)	cand_n(_cd,_n)->split2
#define cand_mrg(_cd,_n)	cand_n(_cd,_n)->merge

#define reset_SM_CAND(_cd)	{ base_cand(_cd) = num_cand(_cd) = 0; max_cand(_cd) = MAX_SPLT_MRG;}

/**********************************************************/
/*  type labels for splt_mrg candidates                   */
#define ACC_SPLIT	0
#define SPLIT		1
#define ACC_MERGE	2
#define MERGE		3



