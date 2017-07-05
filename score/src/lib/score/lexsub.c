/**********************************************************************/
/*                                                                    */
/*             FILENAME:  lexsub.c                                    */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains utilities to load and          */
/*                  manipulate a file containing a subset of lexical  */
/*                  items                                             */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/**********************************************************************/
/*    this should go into gen.c                                       */

void find_first_alpha(char **ptr)
{
    while ((**ptr != NULL_CHAR) && !isalpha(**ptr))
        (*ptr)++;
}

void store_comment_id(char *from_buff, char *descriptor, int len, char *to_buff)
{
    char *tmp_buff;

    alloc_char_singarr(tmp_buff,(int)strlen(from_buff));
    
    find_first_alpha(&from_buff);
    wrdcpy(tmp_buff,from_buff);
    if (strcmp(tmp_buff,descriptor) == 0){
        search_for_char(&from_buff,EQUALS);
        if (*from_buff == EQUALS){
            search_for_char(&from_buff,DOUBLE_QUOTE);
            if (*from_buff == DOUBLE_QUOTE){
                from_buff++;
                strcpy_to_before_char(to_buff,from_buff,DOUBLE_QUOTE);
            }
	}
    }
    free_singarr(tmp_buff,char);
}

void load_LEX_SUBSET(LEX_SUBSET **lsub, TABLE_OF_CODESETS *pcs, char *filename)
{
    char buff[MAX_BUFF_LEN], buff2[MAX_BUFF_LEN], *b_ptr;
    int ind, setnum, debug=0;
    LEX_SUBSET *t_lsub;
    FILE *fp;

    alloc_and_init_LEX_SUBSET(t_lsub);

    if (*(filename) == NULL_CHAR){
        *lsub = t_lsub;
        return;
    }

    if ((fp=fopen(filename,"r")) == NULL){
        fprintf(stderr,"Could not open Lexicon Subset file: %s\n",filename);
        exit(-1);
    }
    
    /* the words till the end of the marker are non-lexical words */

    while (safe_fgets(buff,MAX_BUFF_LEN,fp) != NULL)
      if (strstr1(buff,FILE_BEGIN_SUBSET) != NULL){
           if (debug) printf("Beginning of subset found string %s \n",buff);
           if (lsub_num_set(t_lsub) == lsub_max_set(t_lsub)){
               fprintf(stderr,"Error: Unable to load subsets, too many > %d\n",lsub_max_set(t_lsub));
               exit(-1);
           }
           setnum = lsub_num_set(t_lsub);

           while ((safe_fgets(buff,MAX_BUFF_LEN,fp) != NULL) &&
                  (strstr1(buff,FILE_END_SUBSET) == NULL)){
               if (!is_comment(buff))
                   if (is_comment_info(buff)){ /* is it the description */
                       if (strstr1(buff,FILE_SUBSET_DESC) != NULL){
                           if (debug) printf("Desc found in %s",buff);
                           b_ptr = buff;
                           search_for_char(&b_ptr,'=');
 	                   if (*b_ptr == '=')
			       b_ptr++; /* move over the equals sign */ 
 		           skip_white_space(b_ptr);
		           if (*b_ptr == '"')
 			       b_ptr++;
 		           /* allocate some memory */
	    	           alloc_singarr(lsub_set_desc(t_lsub,setnum),
					 (int)strlen(b_ptr),char);
                           strcpy_to_before_char(lsub_set_desc(t_lsub,setnum),b_ptr,'"');
                           if (debug) printf("stored desc %s\n",lsub_set_desc(t_lsub,setnum));
	 	       }
                   } else{
 		       b_ptr = buff;
		       skip_white_space(b_ptr);
                       strip_newline(b_ptr);
		       wrdcpy(buff2,b_ptr);
		       if ((ind = is_LEXICON(pcs,buff2)) == WORD_NOT_IN_LEX){
		            fprintf(stderr,"Error: Non-lexical word not predefined %s\n",
			                    buff2);
  		            exit(-1);
  		       }
                       if (debug) printf("Adding word %s index %d\n",buff2, ind);
		       lsub_set_elm(t_lsub,setnum,lsub_set_num(t_lsub,setnum)++) = ind;
		   }
           }
           lsub_num_set(t_lsub)++;
     }
     *lsub = t_lsub;
}

/******************************************************************/
/*    print to stdout the contents of the LEX_SUBSET struct       */
/******************************************************************/
void dump_LEX_SUBSET(LEX_SUBSET *lsub, TABLE_OF_CODESETS *pcs)
{
    int i,s;

    if (lsub == (LEX_SUBSET *)NULL){
        printf("Lexicon subset NOT loaded\n");
        return;
    }
    printf("\n\n\tDUMP OF THE LEXICON SUBSET LIST\n");
    printf("Max num Subsets: %d\n",lsub_max_set(lsub));
    printf("Loaded num Subsets: %d\n",lsub_num_set(lsub));
    for (s=0; s<lsub_num_set(lsub); s++){
        printf("Subset %d:\n",s);
        printf("\tdesc: %s\n",lsub_set_desc(lsub,s));
        printf("\tMax Elem: %d\n",lsub_set_max(lsub,s));
        printf("\tLoaded Elem: %d\n",lsub_set_num(lsub,s));
        for (i=0;i<lsub_set_num(lsub,s);i++)
            printf("\t\t%4d:  %s\n",i,lex_lc_word(pcs,lsub_set_elm(lsub,s,i)));
        printf("\n");
    }
    printf("\n\n");
}

void print_LEX_SUBSET(LEX_SUBSET *lsub)
{
    int i;

    if (lsub == (LEX_SUBSET *)NULL){
        printf("Lexicon subset NOT loaded\n");
        return;
    }
    printf("Lexicon subset descriptions:\n");
    
    for (i=0; i<lsub_num_set(lsub); i++)
	printf("     %1d: %s\n",i,lsub_set_desc(lsub,i));
    printf("     9: Homophones\n\n");
    printf("     *: Multiple sets\n\n");
}



/***********************************************************/
/*  search through the lexicon subset list to see if the   */
/*  index was marked as being set                          */
int is_LEX_SUBSET(LEX_SUBSET *lsub, short int ind)
{
    int set, i, mask=0;

    if (lsub == (LEX_SUBSET *)NULL)
        return(NOT_IN_SUBSET);

    for (set=0; set<lsub_num_set(lsub); set++)
        for (i=0; i<lsub_set_num(lsub,set); i++)
            if (lsub_set_elm(lsub,set,i) == ind){
                mask += 1<<set;
            }
    return((mask == 0) ? NOT_IN_SUBSET : mask);
    
}
