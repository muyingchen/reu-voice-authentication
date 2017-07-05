/**********************************************************************/
/*                                                                    */
/*             FILENAME:  lexsub.h                                    */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains typedefs and macros for usage  */
/*                  of a LEXICON_SUBSET structure.  IT just paritions */
/*                  the lexicon into a subset of items                */
/*                                                                    */
/**********************************************************************/
#define INIT_WORD_LEX_SUBSET		100
#define INIT_NUM_SUBSETS		6
#define NOT_IN_LEX_SUBSET		(-1)
#define MAX_LEX_SUBSET_DESC_LEN		80
#define LEX_SUBSET_COMMENT_ID_DESC	"list_desc"
#define FILE_BEGIN_SUBSET		"BEGIN_SUBSET"
#define FILE_END_SUBSET			"END_SUBSET"
#define FILE_SUBSET_DESC		"SUBSET_DESC"
#define IN_SUBSET			1
#define NOT_IN_SUBSET			(-1)

typedef struct subset_struct{
   int maximum;                 /* maximum number of words in the subset */
   int num;                     /* number of words in the set */
   char *list_desc;             /* ascii string to describe the list of wrds*/
   PCIND_T *word_index;         /* the lexicon indexes for each word */
 } SUBSET;

typedef struct lexicon_subset_struct{
   int max_sets;
   int num_sets;
   SUBSET *set;
} LEX_SUBSET;
#ifdef OLD
#define lsub_num(_l)		_l->num
#define lsub_desc(_l)		_l->list_desc
#define lsub_word_list(_l)	_l->word_index
#define lsub_word(_l,_n)	lsub_word_list(_l)[_n]

#endif
#define lsub_max_set(_l)        _l->max_sets
#define lsub_num_set(_l)        _l->num_sets
#define lsub_sets(_l)           _l->set
#define lsub_set_max(_l,_i)	lsub_sets(_l)[_i].maximum
#define lsub_set_num(_l,_i)	lsub_sets(_l)[_i].num
#define lsub_set_desc(_l,_i)	lsub_sets(_l)[_i].list_desc
#define lsub_set_elms(_l,_i)	lsub_sets(_l)[_i].word_index
#define lsub_set_elm(_l,_i,_e)	lsub_set_elms(_l,_i)[_e]


/******************************************************************/
/*    This macro takes a pointer to a LEX_SUBSET structure and    */
/*    allocates memory for the entire structure and               */
/*    initialize the OCCUR structure                              */
#define alloc_and_init_LEX_SUBSET(_l) \
  { int _x; \
    alloc_singarr(_l,1,LEX_SUBSET); \
    lsub_max_set(_l) = INIT_NUM_SUBSETS; \
    lsub_num_set(_l) = 0; \
    alloc_singarr(lsub_sets(_l),INIT_NUM_SUBSETS,SUBSET); \
    for (_x=0; _x<lsub_max_set(_l); _x++){ \
         lsub_set_max(_l,_x) = INIT_WORD_LEX_SUBSET; \
         lsub_set_num(_l,_x) = 0; \
         lsub_set_desc(_l,_x) = ""; \
         alloc_singarr(lsub_set_elms(_l,_x),INIT_WORD_LEX_SUBSET,PCIND_T); \
         lsub_set_desc(_l,_x) = (char *)NULL; \
	 }\
 }

#define free_LEX_SUBSET(_l) \
  { int _x; \
    for (_x=0; _x<lsub_max_set(_l); _x++) \
         free_singarr(lsub_set_elms(_l,_x),PCIND_T); \
    free_singarr(lsub_sets(_l),SUBSET); \
    free_singarr(_l,LEX_SUBSET); \
 }



