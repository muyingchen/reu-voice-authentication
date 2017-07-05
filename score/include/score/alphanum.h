/**********************************************************************/
/*                                                                    */
/*             FILENAME:  alpha_num.h                                 */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains structure definitions and      */
/*                  macros for accessing the ALPHA_NUM structure      */
/*                                                                    */
/**********************************************************************/
#define MAX_ALPHA_NUM		500
#define WORD_NOT_ALPHA_NUM	0
#define IS_ALPHA_NUM		1

typedef struct alp_n_struct{
   int num;            /*  the number of alpha-numeric words              */
   PCIND_T *words;       /*  a list of indexes into the LEXICON for alp_num */
} ALPHA_NUM;

#define alp_n_num(_an)		_an->num
#define alp_n_words(_an)	_an->words
#define alp_n_word(_an,_w)	alp_n_words(_an)[_w]

#define alloc_and_init_ALPHA_NUM(_alp,_n) \
   { alloc_singarr(_alp,1,ALPHA_NUM); \
    alp_n_num(_alp) = 0; \
    alloc_singZ(alp_n_words(_alp),_n,PCIND_T,(PCIND_T)0); \
   }


#define free_ALPHA_NUM(_alp) \
   { free_singarr(alp_n_words(_alp),PCIND_T); \
     free_singarr(_alp,ALPHA_NUM); \
   }
