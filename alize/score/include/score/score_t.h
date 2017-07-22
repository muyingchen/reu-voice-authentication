/*********************************************************************/
/*  File:  score_t.h                                                 */
/*  Date:  August 2, 1994                                            */
/*  Desc:  This structure is the C-Language interface to the         */
/*         NIST alignment code.                                      */
/*********************************************************************/

typedef struct score_t_struct{
    float word_error;          /* = (num_err / num_ref) * 100.0       */ 
    int num_ref;               /* the number of reference words       */
    int num_correct;           /* the number of correct words         */
    int num_deletes;           /* the number of deleted words         */
    int num_inserts;           /* the number of inserted words        */
    int num_subs;              /* the number of substituted words     */
    int num_merges;            /* the number of words merged together */
    int num_splits;            /* the number of words split apart     */
    SENT *sent;                /* An internal representation of the   */
                               /* aligned hyp and ref strings         */
} SCORE_T ;


