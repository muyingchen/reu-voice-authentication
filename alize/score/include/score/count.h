/**********************************************************************/
/*                                                                    */
/*             FILENAME:  count.h                                     */
/*             BY:  Jonathan G. Fiscus                                */
/*             DATE: May 31 1989                                      */
/*                   NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY   */
/*                   SPEECH RECOGNITION GROUP                         */
/*                                                                    */
/*             USAGE:  This file contains structure declarations      */
/*                     and macros for using the COUNT structure.      */
/*                                                                    */
/**********************************************************************/

typedef struct total_struct{
   int correct;              /* num correct words in sent */
   int insertions;           /* num inserted words in sent */
   int deletions;            /* num deleted words in sent */
   int substitutions;        /* num substituted words in sent */
   int merges;               /* num of merges */
   int splits;               /* num of splits */
   int *matched_correct;     /* num systems with this sent correct */
} COUNT;


/**********************************************************************/
/*   macros                                                           */
#define sys_p(_ct,_sy)	 	_ct[_sy]
#define spkr_p(_ct,_sy,_sp) 	sys_p(_ct,_sy)[_sp]
#define sent_p(_ct,_sy,_sp,_st)	spkr_p(_ct,_sy,_sp)[_st]

#define corr(_ct,_sy,_sp,_st)	sent_p(_ct,_sy,_sp,_st)->correct
#define inserts(_ct,_sy,_sp,_st) \
				sent_p(_ct,_sy,_sp,_st)->insertions
#define del(_ct,_sy,_sp,_st)	sent_p(_ct,_sy,_sp,_st)->deletions
#define subs(_ct,_sy,_sp,_st)	sent_p(_ct,_sy,_sp,_st)->substitutions
#define mrgs(_ct,_sy,_sp,_st)	sent_p(_ct,_sy,_sp,_st)->merges
#define spls(_ct,_sy,_sp,_st)	sent_p(_ct,_sy,_sp,_st)->splits
#define mtch_cor_arr(_ct,_sy,_sp,_st)\
 				sent_p(_ct,_sy,_sp,_st)->matched_correct
#define m_corr(_ct,_sy,_sp,_st,_to_t) \
				mtch_cor_arr(_ct,_sy,_sp,_st)[_to_t]

/**********************************************************************/
/*   allocation macros                                                */
#define alloc_COUNT(_ct,_num_sys,_num_spkr,_num_sent) \
    { int _sy, _sp, _st; \
      alloc_singarr(_ct,_num_sys,COUNT ***); \
      for (_sy=0;_sy<_num_sys;_sy++){ \
          alloc_singarr(sys_p(_ct,_sy),_num_spkr,COUNT **); \
          for (_sp=0;_sp < _num_spkr; _sp++){ \
              alloc_singarr(spkr_p(_ct,_sy,_sp),_num_sent,COUNT *); \
              for (_st=0;_st < _num_sent; _st++){ \
                  alloc_singarr(sent_p(_ct,_sy,_sp,_st),1,COUNT); \
                  corr(_ct,_sy,_sp,_st) = 0 ; \
                  inserts(_ct,_sy,_sp,_st) = 0 ; \
                  del(_ct,_sy,_sp,_st) = 0 ; \
                  subs(_ct,_sy,_sp,_st) = 0 ; \
                  mrgs(_ct,_sy,_sp,_st) = 0 ; \
                  spls(_ct,_sy,_sp,_st) = 0 ; \
                  alloc_int_singarr_zero(mtch_cor_arr(_ct,_sy,_sp,_st), \
                                         _num_sys); \
	      } \
	  } \
      } \
    }


      
          
