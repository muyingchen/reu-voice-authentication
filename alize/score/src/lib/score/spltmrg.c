/**********************************************************************/
/*                                                                    */
/*             FILENAME:  splt_mrg.c                                  */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains utilities to load and          */
/*                  manipulate a file containing SPLITS AND MERGES    */
/*                  Currently, only the scoring program               */
/*                  uses these words.                                 */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>


/**********************************************************************/
/*   Given a pointer to SPLT_MRG structure, and a filename, allocate  */
/*   the SPLT_MRG_SET list and load in the file                       */
/**********************************************************************/
void load_SPLT_MRG(SPLT_MRG **sp_mrg, TABLE_OF_CODESETS *pcs, char *filename)
{
    FILE *fp;
    char buff[MAX_BUFF_LEN], *word1, *word2, *word3, *tmp;
    SPLT_MRG *t_spmg;

    alloc_and_init_SPLT_MRG(t_spmg,MAX_SPLT_MRG);
    *sp_mrg = t_spmg;

    if (*(filename) == NULL_CHAR){
        return;
    }
    if ((fp=fopen(filename,"r")) == NULL){
        fprintf(stderr,"Warning: Could not open Split/Merge file: %s",
		filename);
        fprintf(stderr," No words loaded ! ! ! !\n"); 
        return;
    }
    
    while (safe_fgets(buff,MAX_BUFF_LEN,fp) != NULL){
        if (!is_comment(buff) && !is_comment_info(buff)){
            if (sp_mg_num(t_spmg) == MAX_SPLT_MRG){
               fprintf(stderr,"Too many words in the Split/Merge file > %d\n",
                              MAX_SPLT_MRG);
               exit(-1);
	    }
            /*find first word in the buffer */
            word1 = buff;
            skip_white_space(word1);
            tmp = word1;
            find_end_of_word(tmp);
            *(tmp) = NULL_CHAR;

            /*find second word in the buffer */
            word2 = tmp+1;
            skip_white_space(word2);
            tmp = word2;
            find_end_of_word(tmp);
            *(tmp) = NULL_CHAR;

            /*find third word in the buffer */
            word3 = tmp+1;
            skip_white_space(word3);
            tmp = word3;
            find_end_of_word(tmp);
            *(tmp) = NULL_CHAR;

            /* get the index into the lexicon list and store it   */
            /* if one word is not found in the lexicon, do not    */
            /* count the pair as a split/merge set                */

            if ((sp_mg_mrg(t_spmg,sp_mg_num(t_spmg)) =
                   is_LEXICON(pcs,word1)) ==  WORD_NOT_IN_LEX)
                fprintf(stderr,"Spilt/Merge word not in lexicon: %s\n",word1);
            else if ((sp_mg_spl1(t_spmg,sp_mg_num(t_spmg)) = 
                            is_LEXICON(pcs,word2)) == WORD_NOT_IN_LEX)
                fprintf(stderr,"Split/Merge word not in lexicon: %s\n",word2);
            else if ((sp_mg_spl2(t_spmg,sp_mg_num(t_spmg)) = 
                            is_LEXICON(pcs,word3)) == WORD_NOT_IN_LEX)
                fprintf(stderr,"Split/Merge word not in lexicon: %s\n",word2);
            else
                ++sp_mg_num(t_spmg);
	}
    }
    *sp_mrg = t_spmg;
}

/**********************************************************************/
/*  dump the contents of the SPLT_MRG structure to stdout             */
/**********************************************************************/
void dump_SPLT_MRG(SPLT_MRG *sp_mrg, TABLE_OF_CODESETS *pcs)
{
    int i;
   
    printf("\n\n\tDUMP OF SPLIT/MERGE SETS\n\n");
    for (i=0;i<sp_mg_num(sp_mrg);i++)
        printf("set: %2d  %-15s  -->  %-15s  %-15s\n",i,
           lex_word(pcs,sp_mg_mrg(sp_mrg,i)),
           lex_word(pcs,sp_mg_spl1(sp_mrg,i)),
           lex_word(pcs,sp_mg_spl2(sp_mrg,i)));
}

/**********************************************************************/
/*   Initialize the Split and Merge array                             */
/**********************************************************************/
void init_SM_CAND(SM_CAND *sm_cand)
{
    max_cand(sm_cand) = MAX_NUM_CAND;
    num_cand(sm_cand) = 0;
    base_cand(sm_cand) = 0;

    alloc_2dimarr(cand_list(sm_cand),MAX_NUM_CAND,1,SPLT_MRG_CAND)
}

/**********************************************************************/
/*   Expand the Split and Merge array automatically                   */
/**********************************************************************/
void expand_SM_CAND(SM_CAND *sm_cand)
{
    SPLT_MRG_CAND **tcand;  /* candidate set list */
    int i;

    max_cand(sm_cand) *= 1.5;
    alloc_singarr(tcand,max_cand(sm_cand),SPLT_MRG_CAND *);
    memcpy(tcand,cand_list(sm_cand), num_cand(sm_cand) * sizeof(SPLT_MRG_CAND *));
    free_singarr(cand_list(sm_cand),SPLT_MRG_CAND *);
    cand_list(sm_cand) = tcand;
    for (i=num_cand(sm_cand); i<max_cand(sm_cand); i++)
	alloc_singarr(cand_n(sm_cand,i),1,SPLT_MRG_CAND);
}

/**********************************************************************/
/*   free Split and Merge array                                       */
/**********************************************************************/
void free_SM_CAND(SM_CAND *sm_cand)
{
    free_2dimarr(cand_list(sm_cand),max_cand(sm_cand),SPLT_MRG_CAND);
}

/**********************************************************************/
/*  given the a possible mrg and it's two split candidates, return    */
/*  TRUE if they are found in the SPLT_MRG structure and place the    */
/*  set in the candidate list and label it's result                   */
/**********************************************************************/
int is_SPLT(SPLT_MRG *sp_mrg, SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, PCIND_T mrg, PCIND_T spl1, PCIND_T spl2, char lorr)
{
    int i;

    if (num_cand(sm_cand) >= max_cand(sm_cand)){
	expand_SM_CAND(sm_cand);
    }
    db_level = 0;
    if (pcs->cd[1].pc->compositional)
	cand_fom(sm_cand,num_cand(sm_cand)) =
	    compute_sm_fom(pcs,lorr,'s',
			   (lorr == 'l') ? mrg : 0, 
			   (lorr == 'r') ? mrg : 0, 
			   spl1, spl2);
    else
	cand_fom(sm_cand,num_cand(sm_cand)) = 0.0;
    db_level = 0;
    cand_mrg(sm_cand,num_cand(sm_cand)) = mrg;
    cand_spl1(sm_cand,num_cand(sm_cand)) = spl1;
    cand_spl2(sm_cand,num_cand(sm_cand)) = spl2;
    cand_type(sm_cand,num_cand(sm_cand)) = SPLIT;
    /* search the SPLT_MRG structure */
    for (i=0;i<sp_mg_num(sp_mrg);i++)
        if ((sp_mg_mrg(sp_mrg,i) == mrg) && (sp_mg_spl1(sp_mrg,i) == spl1) &&
            (sp_mg_spl2(sp_mrg,i) == spl2))
            /* an ACCEPTABLE SPLIT, mark it */
            cand_type(sm_cand,num_cand(sm_cand)) = ACC_SPLIT;
            
    if (cand_type(sm_cand,num_cand(sm_cand)++) == ACC_SPLIT)
        return(TRUE);
    return(FALSE);
}

/**********************************************************************/
/*  given the a possible mrg and it's two split candidates, return    */
/*  TRUE if they are found in the SPLT_MRG structure and place the    */
/*  set in the candidate list and label it's result                   */
/**********************************************************************/
int is_MRG(SPLT_MRG *sp_mrg, SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, PCIND_T mrg, PCIND_T spl1, PCIND_T spl2, char lorr)
{
    int i;

    if (num_cand(sm_cand) >= max_cand(sm_cand)){
	expand_SM_CAND(sm_cand);
    }
    db_level = 0;
    if (pcs->cd[1].pc->compositional)
	cand_fom(sm_cand,num_cand(sm_cand)) = 
	    compute_sm_fom(pcs,lorr,'m',
			   spl1, spl2,
			   (lorr == 'l') ? mrg : 0, 
			   (lorr == 'r') ? mrg : 0);
    else
	cand_fom(sm_cand,num_cand(sm_cand)) = 0.0;
    db_level = 0;
    cand_mrg(sm_cand,num_cand(sm_cand)) = mrg;
    cand_spl1(sm_cand,num_cand(sm_cand)) = spl1;
    cand_spl2(sm_cand,num_cand(sm_cand)) = spl2;
    cand_type(sm_cand,num_cand(sm_cand)) = MERGE;
    /* search the SPLT_MRG structure */
    for (i=0;i<sp_mg_num(sp_mrg);i++)
        if ((sp_mg_mrg(sp_mrg,i) == mrg) && (sp_mg_spl1(sp_mrg,i) == spl1) &&
            (sp_mg_spl2(sp_mrg,i) == spl2))
            /* an ACCEPTABLE MERGE mark it */
            cand_type(sm_cand,num_cand(sm_cand)) = ACC_MERGE;
           
    if (cand_type(sm_cand,num_cand(sm_cand)++) == ACC_MERGE)
        return(TRUE);
    return(FALSE);
}

/**********************************************************************/
/*   printout the entire SM_CAND list to fp                           */
/*   this routine prints out only the sets from base_cand() to        */
/*   the number of sets in the list                                   */
/**********************************************************************/
void print_SM_CAND(SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, FILE *fp)
{
    int i;

    if ((num_cand(sm_cand) - base_cand(sm_cand)) > 0){
        for (i=base_cand(sm_cand);i<num_cand(sm_cand);i++){
            switch (cand_type(sm_cand,i)){
                case ACC_SPLIT:
                    fprintf(fp,"Acc Spl  ");
                    break;
                case SPLIT:
                    fprintf(fp,"Spl      ");
                    break;
                case ACC_MERGE:
                    fprintf(fp,"Acc Mrg  ");
                    break;
                case MERGE:
                    fprintf(fp,"Mrg      ");
                    break;
            }
            switch (cand_type(sm_cand,i)){
                case ACC_SPLIT:
                case SPLIT:
                    fprintf(fp,"%5.2f %s ==> %s %s\n",
			                cand_fom(sm_cand,i),
                                        lex_word(pcs,cand_mrg(sm_cand,i)),
                                        lex_word(pcs,cand_spl1(sm_cand,i)),
                                        lex_word(pcs,cand_spl2(sm_cand,i)));
                    break;
                case ACC_MERGE:
                case MERGE:
                   fprintf(fp,"%5.2f %s %s ==> %s\n",
			                cand_fom(sm_cand,i),
                                        lex_word(pcs,cand_spl1(sm_cand,i)),
                                        lex_word(pcs,cand_spl2(sm_cand,i)),
                                        lex_word(pcs,cand_mrg(sm_cand,i)));
                    break;
	    }
        }
        fprintf(fp,"\n");
    }
}

/**********************************************************************/
/*   printout only the sets in a SM_CAND list to fp that are of a     */
/*   certain type from base_cand() to the number of sets              */
/**********************************************************************/
void print_SM_CAND_type(SM_CAND *sm_cand, TABLE_OF_CODESETS *pcs, int type, FILE *fp)
{
    int i,exists=0,n;


    if ((num_cand(sm_cand) - base_cand(sm_cand)) > 0){
        /* count how many of the type exists */
        for (i=base_cand(sm_cand);i<num_cand(sm_cand);i++)
            if (cand_type(sm_cand,i) == type)
                exists++;
        if (exists == 0)
            /* if none exists, say so */
            switch (type){
                case ACC_SPLIT:
                    fprintf(fp,"NO ACCEPTABLE SPLITS\n\n");
                    return;
                case SPLIT:
                    fprintf(fp,"NO SPLITS\n\n");
                    return;
                case ACC_MERGE:
                    fprintf(fp,"NO ACCEPTABLE MERGES\n\n");
                    return;
                case MERGE:
                    fprintf(fp,"NO MERGES\n\n");
                    return;
	    }

        switch (type){
            case ACC_SPLIT:
                fprintf(fp,"OCCURRENCES OF ACCEPTABLE SPLITS (%2d)\n\n",
                             exists);
                break;
            case SPLIT:
                fprintf(fp,"OCCURRENCES OF SPLITS (%2d)\n\n",exists);
                break;
            case ACC_MERGE:
                fprintf(fp,"OCCURRENCES OF ACCEPTABLE MERGES (%2d)\n\n",
                            exists);
                break;
            case MERGE:
                fprintf(fp,"OCCURRENCES OF MERGES (%2d)\n\n",exists);
                break;
	}

	fprintf(fp," ID    FOM    \n"); 
        for (i=base_cand(sm_cand),n=1;i<num_cand(sm_cand);i++){
            if (cand_type(sm_cand,i) == type)
                switch (type){
                  case ACC_SPLIT:
                  case SPLIT:
                    fprintf(fp,"%3d:  %5.2f   %s  ==>  %s  %s\n",n++,
			                cand_fom(sm_cand,i),
                                        lex_word(pcs,cand_mrg(sm_cand,i)),
                                        lex_word(pcs,cand_spl1(sm_cand,i)),
                                        lex_word(pcs,cand_spl2(sm_cand,i)));
                    break;
                  case ACC_MERGE:
                  case MERGE:
                    fprintf(fp,"%3d:  %5.2f   %s  %s  ==>  %s\n",n++,
			                cand_fom(sm_cand,i),
                                        lex_word(pcs,cand_spl1(sm_cand,i)),
                                        lex_word(pcs,cand_spl2(sm_cand,i)),
                                        lex_word(pcs,cand_mrg(sm_cand,i)));
                    break;
	        }
        }
        fprintf(fp,"\n");
    }
}

int count_SM_CAND_type_above_thresh(SM_CAND *sm_cand, int typea, int typeb, float thresh)
{
    int i, sum=0;
    for (i=base_cand(sm_cand);i<num_cand(sm_cand);i++){
	if (cand_fom(sm_cand,i) > thresh){
	    if (cand_type(sm_cand,i) == typea ||
		cand_type(sm_cand,i) == typeb)
		sum++;
	}
    }
    return(sum);
}

void sort_SM_CAND(SM_CAND *sm_cand)
{
    int i, num_sets, *ptr_arr, *tmp_type;
    PCIND_T  *tmp_merge, *tmp_splt1, *tmp_splt2;
    double *tmp_fom;

    if ((num_sets = (num_cand(sm_cand) - base_cand(sm_cand))) > 0){
        /* alloc the necessary memory */
        alloc_singarr(tmp_merge,num_sets,PCIND_T);
        alloc_singarr(tmp_splt1,num_sets,PCIND_T);
        alloc_singarr(tmp_splt2,num_sets,PCIND_T);
        alloc_singarr(tmp_fom,num_sets,double);
        alloc_int_singarr(tmp_type,num_sets);
        alloc_int_singarr(ptr_arr,num_sets);

        /* copy the merged numbers into a temporary array */
        for (i=0; i<num_sets; i++){
            tmp_fom[i] = cand_fom(sm_cand,i+base_cand(sm_cand));
            tmp_type[i] = cand_type(sm_cand,i+base_cand(sm_cand));
            tmp_merge[i] = cand_mrg(sm_cand,i+base_cand(sm_cand));
            tmp_splt1[i] = cand_spl1(sm_cand,i+base_cand(sm_cand));
            tmp_splt2[i] = cand_spl2(sm_cand,i+base_cand(sm_cand));
            ptr_arr[i] = i;
        }
#ifdef SORT_BY_INDEX
#if PCIND_SHORT
        sort_short_arr(tmp_merge,num_sets,ptr_arr,INCREASING);
#else
        sort_int_arr(tmp_merge,num_sets,ptr_arr,INCREASING);
#endif
#else
        sort_double_arr(tmp_fom,num_sets,ptr_arr,DECREASING);
#endif
        /* copy the sorted merged numbers back into the structure */
        for (i=0; i<num_sets; i++){
            cand_fom(sm_cand,i+base_cand(sm_cand)) = tmp_fom[ptr_arr[i]];
            cand_type(sm_cand,i+base_cand(sm_cand)) = tmp_type[ptr_arr[i]];
            cand_mrg(sm_cand,i+base_cand(sm_cand)) = tmp_merge[ptr_arr[i]];
            cand_spl1(sm_cand,i+base_cand(sm_cand)) = tmp_splt1[ptr_arr[i]];
            cand_spl2(sm_cand,i+base_cand(sm_cand)) = tmp_splt2[ptr_arr[i]];
        }

        /* free the memory */
        free_singarr(ptr_arr, int);
        free_singarr(tmp_type, int);
        free_singarr(tmp_fom, double);
        free_singarr(tmp_splt2, PCIND_T);
        free_singarr(tmp_splt1, PCIND_T);
        free_singarr(tmp_merge, PCIND_T);
    }
}

double compute_sm_fom(TABLE_OF_CODESETS *pcs, char lorr, char sorm, WCHAR_T ina1, WCHAR_T ina2, WCHAR_T inb1, WCHAR_T inb2)
{
    SM_ELEMENT2 sm_mem, *smrep=&sm_mem;
    ALIGNMENT_INT pal_mem, *pal1=&pal_mem;
    WCHAR_T A_int[40], B_int[40];

    smrep->iloc=1;
    if (sorm == 's'){
	if (lorr == 'l'){
	    smrep->ival = ina1; smrep->jval = inb1; smrep->kval = inb2;
	} else {
	    smrep->ival = ina2; smrep->jval = inb1; smrep->kval = inb2;
	    smrep->iloc++;
	}
    } else {
	if (lorr == 'l'){
	    smrep->ival = ina1; smrep->jval = ina2; smrep->kval = inb1;
	} else {
	    smrep->ival = ina1; smrep->jval = ina2; smrep->kval = inb2;
	    smrep->iloc++;
	}
    }

    smrep->l_or_r = lorr;
    smrep->s_or_m = sorm;
    pal1->aligned = TRUE;
    pal1->pcset = pcs->cd[1].pc;
    pal1->aint = A_int;
    pal1->bint = B_int;
    A_int[0] = B_int[0] = 2;
    A_int[1] = ina1; 	A_int[2] = ina2; 
    B_int[1] = inb1; 	B_int[2] = inb2; 
    
    rate_sm(smrep,pal1);
    return(smrep->fom);
}
