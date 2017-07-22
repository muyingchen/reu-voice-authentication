/**********************************************************************/
/*                                                                    */
/*             FILENAME:  syn.h                                       */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains typedefs and macros for usage  */
/*                  of a SYN structure                                */
/*                                                                    */
/**********************************************************************/

#define MAX_SYN_NUM	50

typedef struct syn_set_struct{
   int syn1;             /* lexicon index for the first homophone */
   int syn2;             /* lexicon index for the second homophone */
} SYN_SET;

typedef struct syn_struct{
   int num;              /* number of synonym sets */
   SYN_SET **set_list;   /* synonym set list       */
} SYN;

#define syn_num(_h)	_h->num
#define syn_slist(_h)	_h->set_list
#define syn_set(_h,_s)	syn_slist(_h)[_s]
#define syn_1(_h,_s)	syn_set(_h,_s)->syn1
#define syn_2(_h,_s)	syn_set(_h,_s)->syn2

/******************************************************************/
/*    initialize the SYN structure                                */
#define alloc_and_init_SYN(_s,_num_sets) \
    { int _n; \
      alloc_singarr(_s,1,SYN); \
      syn_num(_s) = 0; \
      alloc_2dimarr(syn_slist(_s),_num_sets,1,SYN_SET); \
    }
