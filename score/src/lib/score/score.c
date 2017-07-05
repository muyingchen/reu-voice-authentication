/**********************************************************************/
/*                                                                    */
/*             FILENAME:  score.c                                     */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains simple code used to score the  */
/*                  SYS_ALIGN structure.  This version doesn't count  */
/*                  errors of any sort, it justs fills in the eval    */
/*                  arrays for all sentences so other programs can    */
/*                  tell which words are correct or incorrect         */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/**********************************************************************/
/*  This is a front end to 'score_SYS_ALIGN' so that a SYS_ALIGN_LIST */
/*  can be scores with one call                                       */
/**********************************************************************/
void score_SA_LIST(SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, HOMO *homo, LEX_SUBSET *lsub, int no_gram_case)
{
    int sys;
 
    fprintf(stderr,"\nScoring the Rec. systems, this may take a moment\n");

    for (sys=0;sys<num_sys(sa_list);sys++){
        score_SYS_ALIGN(sys_i(sa_list,sys),pcs,homo,lsub,no_gram_case);
    }
}


/**********************************************************************/
/*  this program takes a SYS_ALIGN pointer along with HOMOPHONES      */
/*  and SYNONYM structures and decides correctness of the hypothesis  */
/*  sentences.  if no_gram_case is TRUE, before a                     */
/*  SUB is labelled, their respective lists are searched to see if    */
/*  the hypothesis word falls into their catagory, if so, the word    */
/*  is correct                                                        */
/**********************************************************************/
void score_SYS_ALIGN(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, HOMO *homo, LEX_SUBSET *lsub, int no_gram_case)
{
    int spkr, snt;

    for (spkr=0;spkr < num_spkr(align_str); spkr++){
        for (snt=0; snt < num_sents(align_str,spkr); snt++)
            score_SENT(sent_n(align_str,spkr,snt),homo,pcs,lsub,no_gram_case);
    }
}

/************************************************************************/
/*   This procedure scores and aligned sentence                         */
/************************************************************************/
void score_SENT(SENT *sent, HOMO *homo, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int no_gram_case)
{
    int wrd,set;

    for (wrd=0;s_ref_wrd(sent,wrd)!=END_OF_WORDS;wrd++){
        if (s_ref_wrd(sent,wrd) ==  s_hyp_wrd(sent,wrd))
            if ((set=is_LEX_SUBSET(lsub,s_hyp_wrd(sent,wrd))) ==
                NOT_IN_SUBSET)
                s_eval_wrd(sent,wrd) = EVAL_CORR;
            else
                s_eval_wrd(sent,wrd) =  EVAL_CORR + eval_subset_mask(set);
        else if (s_ref_wrd(sent,wrd) == INSERTION)
            if ((set=is_LEX_SUBSET(lsub,s_hyp_wrd(sent,wrd))) ==
                NOT_IN_SUBSET)
                s_eval_wrd(sent,wrd) = EVAL_INS;
            else
                s_eval_wrd(sent,wrd) = EVAL_INS + eval_subset_mask(set);
        else if (s_hyp_wrd(sent,wrd) == DELETION)
            if ((set=is_LEX_SUBSET(lsub,s_ref_wrd(sent,wrd))) ==
                NOT_IN_SUBSET)
                s_eval_wrd(sent,wrd) = EVAL_DEL;
            else
                s_eval_wrd(sent,wrd) = EVAL_DEL + eval_subset_mask(set);
        else{
           if (no_gram_case && 
                is_HOMO(homo,s_ref_wrd(sent,wrd),
                                 s_hyp_wrd(sent,wrd))){
               /* MARK IT as a homophone */
               s_eval_wrd(sent,wrd) = EVAL_CORR_HOMO;
               /* check to see if it's part of a subset */
               if ((set=is_LEX_SUBSET(lsub,s_ref_wrd(sent,wrd))) !=
                            NOT_IN_SUBSET)
                   s_eval_wrd(sent,wrd) += eval_subset_mask(set);
           }
           else if ((set=is_LEX_SUBSET(lsub,s_ref_wrd(sent,wrd))) ==
                NOT_IN_SUBSET)
               s_eval_wrd(sent,wrd) = EVAL_SUB;
           else
               s_eval_wrd(sent,wrd) = EVAL_SUB + eval_subset_mask(set);;
	}
    }
}

 
