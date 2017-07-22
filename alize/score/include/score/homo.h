/**********************************************************************/
/*                                                                    */
/*             FILENAME:  homo.h                                      */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains typedefs and macros for usage  */
/*                  of a HOMO structure                               */
/*                                                                    */
/**********************************************************************/

#define MAX_HOMO_NUM	200

typedef struct homo_set_struct{
   PCIND_T homo1;             /* lexicon index for the first homophone */
   PCIND_T homo2;             /* lexicon index for the second homophone */
} HOMO_SET;

typedef struct homo_struct{
   int num;               /* number of homophone sets */
   HOMO_SET **set_list;   /* homophone set list */
} HOMO;

#define homo_num(_h)	_h->num
#define homo_slist(_h)	_h->set_list
#define homo_set(_h,_s)	homo_slist(_h)[_s]
#define homo_1(_h,_s)	homo_set(_h,_s)->homo1
#define homo_2(_h,_s)	homo_set(_h,_s)->homo2

/******************************************************************/
/*    initialize the HOMO structure                               */
#define alloc_and_init_HOMO(_h,_num_sets) \
    { int _n; \
      alloc_singarr(_h,1,HOMO); \
      homo_num(_h) = 0; \
      alloc_singarr(homo_slist(_h),_num_sets,HOMO_SET *); \
      for (_n=0;_n<_num_sets;_n++) \
          alloc_singarr(homo_set(_h,_n),1,HOMO_SET); \
    } 

#define free_HOMO(_h,_num_sets) \
    { int _n; \
      for (_n=0;_n<_num_sets;_n++) \
          free_singarr(homo_set(_h,_n),HOMO_SET); \
      free_singarr(homo_slist(_h),HOMO_SET *); \
      free_singarr(_h,HOMO); \
    }

