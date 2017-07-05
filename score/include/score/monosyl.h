/**********************************************************************/
/*                                                                    */
/*             FILENAME:  mono_syl.h                                  */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains typedefs and macros for usage  */
/*                  of a MONO_SYL structure                           */
/*                                                                    */
/**********************************************************************/
#define MAX_MONO_SYL		500
#define WORD_NOT_MONO_SYL	0
#define IS_MONO_SYL		1

typedef struct mono_struct{
   int num;                  /* number of mono-syllabel words */
   PCIND_T *words;           /* array of LEXICON indexes for the words */
} MONO_SYL;

#define mono_num(_mn)		_mn->num
#define mono_words(_mn)		_mn->words
#define mono_word(_mn,_w)	mono_words(_mn)[_w]

/******************************************************************/
/*   initialize a MONO_SYL structure pointer                      */
#define alloc_and_init_MONO_SYL(_mon,_num)\
   {alloc_singarr(_mon,1,MONO_SYL); \
    mono_num(_mon) = 0; \
    alloc_singZ(mono_words(_mon),_num,PCIND_T,(PCIND_T)0); \
   }

#define free_MONO_SYL(_mon)\
   {free_singarr(mono_words(_mon),PCIND_T); \
    free_singarr(_mon,MONO_SYL); \
   }
