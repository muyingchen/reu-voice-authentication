/**********************************************************************/
/*                                                                    */
/*             FILENAME:  sys_align.h                                 */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains general typedefs and macros    */
/*                  to access the SYS_ALIGN and SYS_ALIGN_LIST        */
/*                  structures.                                       */
/*                                                                    */
/*      Fix: converted the following variables to int (was short)     */
/*               int max_number_of_sents;                             */
/*               int number_of_sents;                                 */
/*               int max_number_of_speakers;                          */
/*               int number_of_speakers;                              */
/*           date: 911206                                             */
/**********************************************************************/
#define MAX_NAME_LEN		20
#define MAX_DESC_LEN		80
#define MAX_SPEAKERS		20
#define MAX_SPEAKER_NAME_LEN	64
#define MAX_NUM_SENTS		80
#define MAX_NUM_WORDS		300
#define SENT_ID_LENGTH		100
#define END_OF_WORDS		(-1)
#define INSERTION		(-6000)
#define DELETION		(-6100)

#define MAX_SENTENCE_STRING_LEN   2048

#define UNKNOWN_RESULTS_NO_SPKR	0
#define UNKNOWN_RESULTS_SPKRS	1
#define RM_RESULTS		2
#define ATIS_RESULTS		3
#define TIMIT_RESULTS		4
#define WSJ_RESULTS		5
#define SWB_RESULTS             9
#define UNKNOWN_RESULTS_SPKRS_NONUNIQUE_ID	6
#define UNKNOWN_RESULTS_SPKRS_UNIQUE_ID	7
#define UNKNOWN_RESULTS_NO_ID	8

#define BINARY_MAGIC            "NISTBALI"
#define BINARY_VERSION_STRING   "1.0"

#define DIFFERENT		TRUE
#define COMMON			FALSE

/*********************************************************************/
/*    hypothesis word evaluation defines                             */
/*  the bit flags grow from both ends, the low-order bits are        */
/*  can only be defined for a singular classification set, 'Correct' */
/*  and the high order bits a sub classes                            */
#define EVAL_CORR		0x0001
#define EVAL_CORR_HOMO		0x2001

#define EVAL_SUB		0x0002

#define EVAL_DEL		0x0004

#define EVAL_INS		0x0008

#define EVAL_MRG		0x0010

#define EVAL_SPT		0x0020

#define EVAL_SETS_MASK		0x003f

#define is_CORR(_n) ((_n & EVAL_SETS_MASK) & EVAL_CORR) != 0
#define is_SUB(_n)  ((_n & EVAL_SETS_MASK) & EVAL_SUB)  != 0
#define is_DEL(_n)  ((_n & EVAL_SETS_MASK) & EVAL_DEL)  != 0
#define is_INS(_n)  ((_n & EVAL_SETS_MASK) & EVAL_INS)  != 0
#define is_MERGE(_n)  ((_n & EVAL_SETS_MASK) & EVAL_MRG)  != 0
#define is_SPLIT(_n)  ((_n & EVAL_SETS_MASK) & EVAL_SPT)  != 0
 
#define eval_subset_mask(_s)		(_s<<6)
#define eval_set(_s)			(_s>>6)  
#define eval_super_set(_s)		(_s & EVAL_SETS_MASK)

#define ATTRIB_NONE                     0x0000
#define ATTRIB_TIMEMRK                  0x0001

#define has_ATTRIB(_s,_a)                 ((_s & _a) != 0)

typedef struct sent_struct{
    char *sentence_id;                  /* character string for the index */
    short nref;                         /* number of reference words */
    PCIND_T *ref_ind_arr;               /* arr for lex indexes of ref words */
    float *ref_beg_arr;                 /* arr: beginning time of ref words */
    float *ref_dur_arr;                 /* arr: duration time of ref words */
    PCIND_T *hyp_ind_arr;               /* arr for lex indexes of hyp words */
    float *hyp_beg_arr;                 /* arr: beginning time of ref words */
    float *hyp_dur_arr;                 /* arr: duration time of ref words */
    short *eval_arr;                 /*will hold a char telling correctness */
} SENT;

typedef struct speaker_struct{
    char speaker_name[MAX_SPEAKER_NAME_LEN+2]; /* spkr name string */
    int max_number_of_sents;                   /* max num of sents for spkr */
    int number_of_sents;                       /* number of sents for spkr */
    int nref;                                  /* number of reference words*/
    SENT **sentence_list;                      /* list of sentences */
} SPKR;

typedef struct system_align_struct{
    char system_name[MAX_NAME_LEN+2];         /* 4 char name of the system */
    char system_desc[MAX_DESC_LEN+2];         /* 40 char system description*/
    short no_grammar_case;                    /* TRUE if sys is no grammar */
    short type_of_results;                    /* TRUE if sys is ATIS res */
    int checksum;                             /* an integer checksum against */
                                              /* the lexicon*/
    int nref;                                 /* number of reference words*/
    int max_number_of_speakers;               /* max num of spkrs for the sys*/
    int number_of_speakers;                   /* number of spkrs for the sys*/
    SPKR **speaker_list;
    char alignment_rev[10];                   /* version identifier */
    int attributes;                           /* a bit field describing */
                                              /* attrib the system's */
} SYS_ALIGN;

/*****************************************************************/
/*    macros to access a sentence for an SENT pointer            */
#define s_id(_sent)		_sent->sentence_id
#define s_nref(_sent)		_sent->nref

#define s_hyp(_sent)		_sent->hyp_ind_arr
#define s_hyp_wrd(_sent,_wd)	s_hyp(_sent)[_wd]
#define s_hyp_beg(_sent)		_sent->hyp_beg_arr
#define s_hyp_beg_wrd(_sent,_wd)	s_hyp_beg(_sent)[_wd]
#define s_hyp_dur(_sent)		_sent->hyp_dur_arr
#define s_hyp_dur_wrd(_sent,_wd)	s_hyp_dur(_sent)[_wd]

#define s_ref(_sent)		_sent->ref_ind_arr
#define s_ref_wrd(_sent,_wd)	s_ref(_sent)[_wd]
#define s_ref_beg(_sent)		_sent->ref_beg_arr
#define s_ref_beg_wrd(_sent,_wd)	s_ref_beg(_sent)[_wd]
#define s_ref_dur(_sent)		_sent->ref_dur_arr
#define s_ref_dur_wrd(_sent,_wd)	s_ref_dur(_sent)[_wd]
#define s_eval(_sent)		_sent->eval_arr
#define s_eval_wrd(_sent,_wd)	s_eval(_sent)[_wd]

/*****************************************************************/
/*    macros to access a sentence for an SPKR pointer            */
#define sp_name(_sp)		_sp->speaker_name
#define sp_nref(_sp)            _sp->nref
#define sp_max_num_st(_sp)	_sp->max_number_of_sents
#define sp_num_st(_sp)		_sp->number_of_sents
#define sp_st_l(_sp)		_sp->sentence_list
#define sp_st(_sp,_st)		sp_st_l(_sp)[_st]

/********************************************************************/
/*   Macros to access a SYS_ALIGN structure from a pointer to one   */
/********************************************************************/
#define sys_name(_sys)		_sys->system_name
#define sys_desc(_sys)		_sys->system_desc
#define sys_no_gr(_sys)		_sys->no_grammar_case
#define sys_type(_sys)		_sys->type_of_results
#define sys_chksum(_sys)	_sys->checksum
#define sys_nref(_sys)    	_sys->nref
#define sys_ali_rev(_sys)    	_sys->alignment_rev
#define sys_attrib(_sys)    	_sys->attributes
#define max_spkr(_sys)		_sys->max_number_of_speakers
#define num_spkr(_sys)		_sys->number_of_speakers

#define spkr_list(_sys)		_sys->speaker_list
#define spkr_n(_sys,_sp)	spkr_list(_sys)[_sp]

#define spkr_name(_sys,_sp)	sp_name(spkr_n(_sys,_sp))
#define spkr_nref(_sys,_sp)	sp_nref(spkr_n(_sys,_sp))
#define max_sents(_sys,_sp)	sp_max_num_st(spkr_n(_sys,_sp))
#define num_sents(_sys,_sp)	sp_num_st(spkr_n(_sys,_sp))

#define sent_list(_sys,_sp)	sp_st_l(spkr_n(_sys,_sp))
#define sent_n(_sys,_sp,_st)	sp_st(spkr_n(_sys,_sp),_st)

#define sent_id(_sys,_sp,_st)	s_id(sent_n(_sys,_sp,_st))
#define sent_nref(_sys,_sp,_st)	s_nref(sent_n(_sys,_sp,_st))

#define hyp_data(_sys,_sp,_st)		s_hyp(sent_n(_sys,_sp,_st))
#define hyp_word(_sys,_sp,_st,_wd)	s_hyp_wrd(sent_n(_sys,_sp,_st),_wd)
#define hyp_beg_data(_sys,_sp,_st)	s_hyp_beg(sent_n(_sys,_sp,_st))
#define hyp_beg_word(_sys,_sp,_st,_wd)	s_hyp_beg_wrd(sent_n(_sys,_sp,_st),_wd)
#define hyp_dur_data(_sys,_sp,_st)	s_hyp_dur(sent_n(_sys,_sp,_st))
#define hyp_dur_word(_sys,_sp,_st,_wd)	s_hyp_dur_wrd(sent_n(_sys,_sp,_st),_wd)

#define ref_data(_sys,_sp,_st)	        s_ref(sent_n(_sys,_sp,_st))
#define ref_word(_sys,_sp,_st,_wd)	s_ref_wrd(sent_n(_sys,_sp,_st),_wd)
#define ref_beg_data(_sys,_sp,_st)	s_ref_beg(sent_n(_sys,_sp,_st))
#define ref_beg_word(_sys,_sp,_st,_wd)	s_ref_beg_wrd(sent_n(_sys,_sp,_st),_wd)
#define ref_dur_data(_sys,_sp,_st)	s_ref_dur(sent_n(_sys,_sp,_st))
#define ref_dur_word(_sys,_sp,_st,_wd)	s_ref_dur_wrd(sent_n(_sys,_sp,_st),_wd)

#define eval_list(_sys,_sp,_st)		s_eval(sent_n(_sys,_sp,_st))
#define eval_wrd(_sys,_sp,_st,_w)	s_eval_wrd(sent_n(_sys,_sp,_st),_w)

#define alloc_and_init_SENT(_st,_n) \
   { alloc_singarr(_st, 1, SENT); \
     s_nref(_st) = 0; \
     alloc_singarr(s_id(_st),SENT_ID_LENGTH,char); \
     alloc_singZ(s_hyp(_st),_n,PCIND_T,END_OF_WORDS); \
     alloc_singZ(s_ref(_st),_n,PCIND_T,END_OF_WORDS); \
     alloc_singZ(s_eval(_st),_n,short,END_OF_WORDS); \
     alloc_singZ(s_hyp_beg(_st),_n,float,0.0); \
     alloc_singZ(s_hyp_dur(_st),_n,float,0.0); \
     alloc_singZ(s_ref_beg(_st),_n,float,0.0); \
     alloc_singZ(s_ref_dur(_st),_n,float,0.0); \
   }      

#define alloc_and_init_SPKR(_sp) \
   {  alloc_singarr(_sp,1,SPKR); \
      sp_max_num_st(_sp) = MAX_NUM_SENTS; \
      sp_num_st(_sp) = 0; \
      sp_nref(_sp) = 0; \
      alloc_singarr(sp_st_l(_sp),MAX_NUM_SENTS,SENT *); \
   }

/******************************************************************/
/*  SYS_ALIGN inititalization macros                              */
/******************************************************************/
/*        alloc a SYS_ALIGN STRUCTURE                             */
#define alloc_SYS_ALIGN(_sys) alloc_singarr(_sys,1,SYS_ALIGN)

/******************************************************************/
/*    alloc a pointer to an array of SYS_ALIGN pointers           */
#define alloc_SYS_ALIGN_ptr_arr(_sys,_num) \
      alloc_singarr(_sys,_num,SYS_ALIGN *);

/******************************************************************/
/*        initialize a SYS_ALIGN STRUCTURE                        */
#define alloc_and_init_SYS_ALIGN(_sys) \
    { alloc_SYS_ALIGN(_sys); \
      *(sys_name(_sys)) = NULL_CHAR; \
      *(sys_desc(_sys)) = NULL_CHAR; \
      sys_no_gr(_sys) = FALSE; \
      sys_type(_sys) = RM_RESULTS; \
      sys_chksum(_sys) = 0; \
      sys_nref(_sys) = 0; \
      max_spkr(_sys) = MAX_SPEAKERS; \
      num_spkr(_sys) = 0; \
      alloc_singarr(spkr_list(_sys), MAX_SPEAKERS,SPKR *); \
      strcpy(sys_ali_rev(_sys),BINARY_VERSION_STRING); \
      sys_attrib(_sys) = ATTRIB_NONE; \
    }

/***********************************************************************/
/*    typedef of a SYS_ALIGN_LIST, which is a structure containing     */
/*    pointers to a list of SYS_ALIGN s                                */
/***********************************************************************/
typedef struct system_align_struct_list{
    int number_of_systems;   /* number of SYS_ALIGN structures */
    SYS_ALIGN **align_list;  /* a list of SYS_ALIGN structures */ 
} SYS_ALIGN_LIST;	


#define num_sys(_alist)		_alist->number_of_systems
#define ali_list(_alist)	_alist->align_list
#define sys_i(_alist,_sys)	ali_list(_alist)[_sys]

/*****************************************************************/
/*   inititalize a SYS_ALIGN_LIST                                */
#define init_SYS_ALIGN_LIST(_alist,_num) \
    num_sys(_alist) = 0; \
    alloc_SYS_ALIGN_ptr_arr(ali_list(_alist),_num)


/*****************************************************************/
/*  defines for using sentence pointers                          */    
/*****************************************************************/
/*    alloc a list of pointer to SENT structures                 */
#define alloc_SENT_list(_s_list,_num_snt) \
    alloc_singarr(_s_list,_num_snt,SENT *)



/*****************************************************************/
/*  defines for FREE-ing a sysalign structure                    */
/*****************************************************************/
/*    free a SENT structure                                      */
#define free_SENT(_st) \
   {   free_singarr(s_ref_dur(_st),float); \
       free_singarr(s_ref_beg(_st),float); \
       free_singarr(s_hyp_dur(_st),float); \
       free_singarr(s_hyp_beg(_st),float); \
       free_singarr(s_eval(_st),short); \
       free_singarr(s_ref(_st), PCIND_T) ; \
       free_singarr(s_hyp(_st), PCIND_T) ; \
       free_singarr(s_id(_st), char); \
       free_singarr(_st, SENT); \
   }

/*    free a SPKR structure                                      */
#define free_SPKR(_sp) \
   {   int _j; \
       for (_j=0; _j<sp_num_st(_sp); _j++) \
	   { free_SENT(sp_st(_sp,_j)); } \
       free_singarr(sp_st_l(_sp), SENT *); \
       free_singarr(_sp, SPKR); \
   }
       
/*    free a SYS_ALIGN structure                                 */
#define free_SYS_ALIGN(_sys) \
   {   int _i; \
       for (_i=0; _i<num_spkr(_sys); _i++) \
	   free_SPKR(spkr_n(_sys,_i)); \
       free_singarr(spkr_list(_sys), SPKR *); \
       free_singarr(_sys,SYS_ALIGN); \
   }

/*    free a SYS_ALIGN_LIST structure                            */
#define free_SYS_ALIGN_LIST(_alist) \
    {   int _k; \
	for (_k=0; _k<num_sys(_alist); _k++) \
	    free_SYS_ALIGN(sys_i(_alist,_k)); \
    }
