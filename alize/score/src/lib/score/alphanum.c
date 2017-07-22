/**********************************************************************/
/*                                                                    */
/*             FILENAME:  alpha_num.c                                 */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains utilities to load and          */
/*                  manipulate a file containing the ALPHA-NUMERIC    */
/*                  words within the LEXICON.  Currently, only the    */
/*                  scoring program uses these words.                 */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/*********************************************************************/
/*   Given a pointer to an ALPHA_NUM structure and the lexicon       */
/*   load the ALPHA-NUMERICS in from the filename                    */
/*********************************************************************/
void load_ALPHA_NUM(ALPHA_NUM **alpha_num, TABLE_OF_CODESETS *pcs, char *filename)
{
    FILE *fp;
    char buff[MAX_BUFF_LEN], *word1, *tmp;
    ALPHA_NUM *t_alp;

    alloc_and_init_ALPHA_NUM(t_alp,MAX_ALPHA_NUM);
    *alpha_num = t_alp;

    if (*(filename) == NULL_CHAR)
        return;
    if ((fp=fopen(filename,"r")) == NULL){
        fprintf(stderr,"Warning: Could not open Alpha Numeric file: %s",
                       filename);
        fprintf(stderr," No words loaded ! ! ! !\n");
        return;
    }
    
    while (safe_fgets(buff,MAX_BUFF_LEN,fp) != NULL){
        if (!is_comment(buff)){
           if (alp_n_num(t_alp) == MAX_ALPHA_NUM){
             fprintf(stderr,"Too many words in the Alpha Numeric file > %d\n",
                             MAX_ALPHA_NUM);
             exit(-1);
	   }
           word1 = buff;
           skip_white_space(word1);
           tmp = word1;
           find_end_of_word(tmp);
            *(tmp) = NULL_CHAR;

           /* get the index into the lexicon list and store it   */
           /* if either word is not found in the lexicon, do not */
           /* count the pair as an ALPHA NUMERIC WORD            */
           if ((alp_n_word(t_alp,alp_n_num(t_alp)) = 
                is_LEXICON(pcs,word1)) == WORD_NOT_IN_LEX)
              fprintf(stderr,"Alpha Numeric word not in lexicon: %s\n",word1);
           else
              ++alp_n_num(t_alp);
	}
      }
    *alpha_num = t_alp;
}

/******************************************************************/
/*    print to stdout the contents of the ALPHA_NUM struct        */
/******************************************************************/
void dump_ALPHA_NUM(ALPHA_NUM *alpha_num, TABLE_OF_CODESETS *pcs)
{
    int i;

    printf("\n\n\tDUMP OF THE ALPHA NUMERIC WORDS\n\n");
    for (i=0;i<alp_n_num(alpha_num);i++)
        printf("%4d:  %s\n",i,lex_word(pcs,alp_n_word(alpha_num,i)));
}

/******************************************************************/
/*    Given an index of a lexicon word, return TRUE if the the    */
/*    word is ALPHA_NUMERIC                                       */
/******************************************************************/
int is_ALPHA_NUM(ALPHA_NUM *alpha_num, int num)
{
    int low, high, mid;

    low = 0, high = alp_n_num(alpha_num)-1;

    do { 
        mid = (low + high)/2;
        if (alp_n_word(alpha_num,mid) > num)
            high = mid-1;
        else if (alp_n_word(alpha_num,mid) < num)
            low = mid+1;
    } while ((low <= high) &&
             (alp_n_word(alpha_num,mid) != num));
    if (low > high)
        return(WORD_NOT_ALPHA_NUM);
    else
        return(IS_ALPHA_NUM);

}
