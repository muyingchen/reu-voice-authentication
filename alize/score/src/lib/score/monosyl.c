/**********************************************************************/
/*                                                                    */
/*             FILENAME:  mono_syl.c                                  */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains utilities to load and          */
/*                  manipulate a list of mono_syllable words          */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/**********************************************************************/
/*  Given a pointer to a MONO_SYL structure and a filename, init      */
/*  the structure and load all the mono-syllable words                */
/**********************************************************************/
void load_MONO_SYL(MONO_SYL **mono_syl, TABLE_OF_CODESETS *pcs, char *filename)
{
    FILE *fp;
    char buff[MAX_BUFF_LEN], *word1, *tmp;
    MONO_SYL *t_mono;

    alloc_and_init_MONO_SYL(t_mono,MAX_MONO_SYL);
    *mono_syl = t_mono;

    if (*(filename) == NULL_CHAR)
        return;
    if ((fp=fopen(filename,"r")) == NULL){
        fprintf(stderr,"Warning: Could not load Mono Syllable file: %s",
		filename);
        fprintf(stderr," No words loaded ! ! ! !\n");
        return;
    }
    
    while (safe_fgets(buff,MAX_BUFF_LEN,fp) != NULL)
        if (!is_comment(buff) && !is_comment_info(buff)){
           if (mono_num(t_mono) == MAX_MONO_SYL){
             fprintf(stderr,"Too many words in the Alpha Numeric file > %d\n",
                           MAX_MONO_SYL);
             exit(-1);
	   }
           word1 = buff;
           skip_white_space(word1);
           tmp = word1;
           find_end_of_word(tmp);
           *(tmp) = NULL_CHAR;

           /* get the index into the lexicon list and store it   */
           /* if either word is not found in the lexicon, do not */
           /* count the pair as a MONO_SYLLABLE word             */
           if ((mono_word(t_mono,mono_num(t_mono)) = 
                is_LEXICON(pcs,word1)) == WORD_NOT_IN_LEX)
              fprintf(stderr,"Mono-syllable word not in lexicon: %s\n",word1);
           else
              ++mono_num(t_mono);
	}
    *mono_syl = t_mono;
}

/**********************************************************************/
/*  Dump the current contents of a MONO_SYL structure                 */
/**********************************************************************/
void dump_MONO_SYL(MONO_SYL *mono_syl, TABLE_OF_CODESETS *pcs)
{
    int i;

    printf("\n\n\tDUMP OF THE MONO SYLLABLE WORDS\n\n");
    for (i=0;i<mono_num(mono_syl);i++)
        printf("%4d:  %s\n",i,lex_word(pcs,mono_word(mono_syl,i)));
}

/**********************************************************************/
/*   if the num is a mono-syllable word, SAY  it is                   */
/*  FIX:  high should be initialized to the number-1 of things to     */
/*        search Oct 23, 1990                                         */
/**********************************************************************/
int is_MONO_SYL(MONO_SYL *mono_syl, int num)
{
    int low, high, mid;

    low = 0, high = mono_num(mono_syl)-1;

    do { 
        mid = (low + high)/2;
        if (mono_word(mono_syl,mid) > num)
            high = mid-1;
        else if (mono_word(mono_syl,mid) < num)
            low = mid+1;
    } while ((low <= high) &&
             (mono_word(mono_syl,mid) != num));
    if (low > high)
        return(WORD_NOT_MONO_SYL);
    else
        return(IS_MONO_SYL);

}
