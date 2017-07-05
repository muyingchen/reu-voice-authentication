/**********************************************************************/
/*                                                                    */
/*             FILENAME:  homo.c                                      */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains utilities to load and          */
/*                  manipulate a homophone file                       */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/**********************************************************************/
/*  Given a pointer to a HOMO structure and a filename, init          */
/*  the structure and load the file.                                  */
/**********************************************************************/
void load_HOMO(HOMO **hom, TABLE_OF_CODESETS *pcs, char *filename)
{
    FILE *fp;
    char buff[MAX_BUFF_LEN], *word1, *word2, *tmp;
    HOMO *tmp_hom;

    alloc_and_init_HOMO(tmp_hom,MAX_HOMO_NUM);
    *hom = tmp_hom;

    if (filename == (char *)0) {
        return;
    }
    if (*(filename) == NULL_CHAR) {
        return;
    }
    if ((fp=fopen(filename,"r")) == NULL){
        fprintf(stderr,"Warning: Could not open homophone file: %s",
		filename);
        fprintf(stderr," No words loaded ! ! ! !\n");
        return;
    }
    
    while (safe_fgets(buff,MAX_BUFF_LEN,fp) != NULL)
        /* ignore comments */
        if (!is_comment(buff)){
            if (homo_num(tmp_hom) == MAX_HOMO_NUM){
                fprintf(stderr,"Too many words in the homophone file > %d\n",
                               MAX_HOMO_NUM);
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

            /* get the index into the lexicon list and store it   */
            /* if either word is not found in the lexicon, do not */
            /* count the pair as a homophone                      */
            if ((homo_1(tmp_hom,homo_num(tmp_hom)) = is_LEXICON(pcs,word1)) ==
                    WORD_NOT_IN_LEX)
                fprintf(stderr,"Homophone word not in lexicon: %s\n",word1);
            else if ((homo_2(tmp_hom,homo_num(tmp_hom)) = 
                      is_LEXICON(pcs,word2)) == WORD_NOT_IN_LEX)
                fprintf(stderr,"Homophone word not in lexicon: %s\n",word2);
            else
                ++homo_num(tmp_hom);
	}
    *hom = tmp_hom;
}

/**********************************************************************/
/*  Dump the current contents of a HOMO structure                     */
/**********************************************************************/
void dump_HOMO(HOMO *hom, TABLE_OF_CODESETS *pcs)
{
    int i;
   
    printf("\n\n\tDUMP OF HOMOPHONE SETS\n\n");
    if (homo_num(hom) == 0)
        printf("No Homophones\n");
    for (i=0;i<homo_num(hom);i++)
        printf("set: %2d  %-15s  %-15s\n",i,
           lex_word(pcs,homo_1(hom,i)),
           lex_word(pcs,homo_2(hom,i)));
}

/**********************************************************************/
/*  Search the HOMO structure, return TRUE if the two words are       */
/*  homophones                                                        */
/**********************************************************************/
int is_HOMO(HOMO *hom, int ind1, int ind2)
{
    int i;

    if (hom == (HOMO *)0)
	return(FALSE);

    for (i=0;i<homo_num(hom);i++){
        if ((homo_1(hom,i) == ind1) && (homo_2(hom,i) == ind2))
            return(TRUE);
        else if ((homo_1(hom,i) == ind2) && (homo_2(hom,i) == ind1))
            return(TRUE);
    }
    return(FALSE);
}

/**********************************************************************/
/*  Search the HOMO structure, return TRUE the word is in the         */
/*  homophones list                                                   */
/**********************************************************************/
int appears_in_HOMO(HOMO *hom, int ind)
{
    int i;

    for (i=0;i<homo_num(hom);i++){
        if ((homo_1(hom,i) == ind) || (homo_2(hom,i) == ind))
            return(TRUE);
    }
    return(FALSE);
}

/**********************************************************************/
/*  Erase the homophone structure                                     */
/**********************************************************************/
void erase_HOMO(HOMO *hom, TABLE_OF_CODESETS *pcs)
{
    homo_num(hom) = 0;
}
