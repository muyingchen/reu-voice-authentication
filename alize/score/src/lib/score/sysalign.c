/**********************************************************************/
/*                                                                    */
/*             FILENAME:  sys_align.c                                 */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains general routines used          */
/*                  to load, write, manipulate a SYS_ALIGN structure  */
/*                                                                    */
/*           NOTE:  Look in ../include/sysalign.h for a more          */
/*                  verbose description                               */
/*                                                                    */
/**********************************************************************/
#define MAX_PR_SNT		10
#define CORR_MARKER		""
#define CORR_MARKER_AUX		"C"
#define INS_MARKER		"I"
#define SUB_MARKER		"S"
#define DEL_MARKER		"D"
#define MERGE_MARKER		"M"
#define SPLIT_MARKER		"T"

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/* A global variable */
int global_use_phonetic_alignments = F;


#define add_blank_to_end(ptr)  *(ptr) = SPACE; *(++ptr) = NULL_CHAR;

/**********************************************************************/
/*  given a list of filenames of ALIGNMENT files, and a pointer to    */
/*  a SYS_ALIGN_LIST, alloc memory and read in the files              */
/*      Change: This routine return -1 if it fails to read in a file, */
/*              a zero otherwise.                                     */
/**********************************************************************/
int load_in_SYS_ALIGN_LIST(SYS_ALIGN_LIST *sa_list, char **name_list, int num_names, int max_num_sys, TABLE_OF_CODESETS *pcs)
{
    int i, fail=FALSE;
    
    init_SYS_ALIGN_LIST(sa_list,max_num_sys);
    for (i=0;i<num_names;i++){
       if (read_SYS_ALIGN(&(sys_i(sa_list,i)),name_list[i]) < 0)
           return(-1);
       if (0) dump_SYS_ALIGN_by_num(sys_i(sa_list,i));
       if (! pass_checksum(sys_i(sa_list,i),pcs)){
           fprintf(stderr,"Error: Alignment file '%s' fails checksum\n",
                          name_list[i]);
           fail=TRUE;
       }
    }
    if (fail)
        exit(-1);
    num_sys(sa_list) = num_names;
    return(0);
}

/**********************************************************************/
/*  given a filename of an ALIGNMENT file, and a pointer to           */
/*  a SYS_ALIGN_LIST, and the pcodeset, read in the files, then check */
/*  the checksum.                                                     */
/*      Change: This routine return -1 if it fails to read in a file, */
/*              a zero otherwise.                                     */
/**********************************************************************/
int load_in_SYS_ALIGN(SYS_ALIGN **sys_align, char *filename, TABLE_OF_CODESETS *pcs)
{
    if (read_SYS_ALIGN(sys_align,filename) < 0)
        return(-1);
    if (! pass_checksum(*sys_align,pcs)){
        fprintf(stderr,"Error: Alignment file '%s' fails checksum\n",
                       filename);
        exit(-1);
    }
    return(0);
}

/**********************************************************************/
/*  Write the SYS_ALIGN structure to the ALIGNMENT file               */
/*      Fix: converted the following variables to be written as int   */
/*           (was short)                                              */
/*               int max_number_of_sents;                             */
/*               int number_of_sents;                                 */
/*               int max_number_of_speakers;                          */
/*               int number_of_speakers;                              */
/*           date: 911206                                             */
/*      Change: Making size for ref and hyp arrays to PCIND_T         */
/*           date: 920702                                             */
/**********************************************************************/
void write_SYS_ALIGN(SYS_ALIGN *sys_align, char *filename)
{
    FILE *fp;
    int spkr, st, wd, sh_sz, chr_sz, int_sz, pcind_sz, fl_sz;

    int_sz = sizeof(int);  
    pcind_sz = sizeof(PCIND_T);  
    sh_sz = sizeof(short);
    chr_sz = sizeof(char);
    fl_sz = sizeof(float);

    if ((filename == NULL) || (*(filename) == NULL_CHAR) || 
        ((fp=fopen(filename,"w")) == NULL)){
        fprintf(stderr,"Could not open aligned output file > %s\n",filename);
        exit(-1);
    }
    /* write on the magic number, version and attributes */
    fprintf(fp,"%s\n",BINARY_MAGIC);
    fprintf(fp,"%s\n",sys_ali_rev(sys_align));
    fwrite((char *)&(sys_attrib(sys_align)),1,int_sz,fp);
    
    fprintf(fp,"%s\n",sys_name(sys_align));
    fprintf(fp,"%s\n",sys_desc(sys_align));
    fwrite((char *)&(sys_no_gr(sys_align)),1,sh_sz,fp);
    fwrite((char *)&(sys_type(sys_align)),1,sh_sz,fp);
    fwrite((char *)&(sys_chksum(sys_align)),1,int_sz,fp);
    fwrite((char *)&(sys_nref(sys_align)),1,int_sz,fp);
    fwrite((char *)&(max_spkr(sys_align)),1,int_sz,fp);
    fwrite((char *)&(num_spkr(sys_align)),1,int_sz,fp);
    for (spkr=0; spkr < num_spkr(sys_align); spkr++){
        fprintf(fp,"%s\n",spkr_name(sys_align,spkr));
        fwrite((char *)&(max_sents(sys_align,spkr)),1,int_sz,fp);
        fwrite((char *)&(num_sents(sys_align,spkr)),1,int_sz,fp);
        fwrite((char *)&(spkr_nref(sys_align,spkr)),1,int_sz,fp);
        for (st=0; st<num_sents(sys_align,spkr); st++){
            fprintf(fp,"%s\n",sent_id(sys_align,spkr,st));
	    fwrite((char *)&(sent_nref(sys_align,spkr,st)),1,sh_sz,fp);
            wd=0;
            do{
               fwrite((char *)&(hyp_word(sys_align,spkr,st,wd)),1,pcind_sz,fp);
               fwrite((char *)&(ref_word(sys_align,spkr,st,wd)),1,pcind_sz,fp);
               fwrite((char *)&(eval_wrd(sys_align,spkr,st,wd)),1,sh_sz,fp);
	       if (has_ATTRIB(sys_attrib(sys_align),ATTRIB_TIMEMRK)){
		   fwrite((char *)&(hyp_beg_word(sys_align,spkr,st,wd)),1,
			  fl_sz,fp);
		   fwrite((char *)&(hyp_dur_word(sys_align,spkr,st,wd)),1,
			  fl_sz,fp);
		   fwrite((char *)&(ref_beg_word(sys_align,spkr,st,wd)),1,
			  fl_sz,fp);
		   fwrite((char *)&(ref_dur_word(sys_align,spkr,st,wd)),1,
			  fl_sz,fp);
	       }

            } while (hyp_word(sys_align,spkr,st,wd++) != END_OF_WORDS);
	}
    }
    fclose(fp);
}

/**********************************************************************/
/*  Given a filename of an ALIGNMENT FILE and an allocated structure  */
/*  read in the file                                                  */ 
/*      Fix: converted the following variables to be read ing as int  */
/*           (was short)                                              */
/*               int max_number_of_sents;                             */
/*               int number_of_sents;                                 */
/*               int max_number_of_speakers;                          */
/*               int number_of_speakers;                              */
/*           date: 911206                                             */
/*      Change: This routine return -1 if it fails to read in a file, */
/*              a zero otherwise.                                     */
/**********************************************************************/
int read_SYS_ALIGN(SYS_ALIGN **sys_align, char *filename)
{
    FILE *fp;
    int spkr, st, wd, sh_sz, chr_sz, int_sz, pcind_sz, fl_sz;
    SYS_ALIGN *t_sy;

    alloc_and_init_SYS_ALIGN(t_sy);
    int_sz = sizeof(int);
    sh_sz = sizeof(short);
    chr_sz = sizeof(char);
    fl_sz = sizeof(float);
    pcind_sz = sizeof(PCIND_T);  

    /* set the version number first ! */
    if ((*(filename) == NULL_CHAR) || ((fp=fopen(filename,"r")) == NULL)){
            fprintf(stderr,"Could not input align file > %s\n",filename);
        return(-1);
    }

    fgetln(sys_name(t_sy),MAX_NAME_LEN+2,fp);
    if (strncmp(sys_name(t_sy),BINARY_MAGIC,strlen(BINARY_MAGIC)) == 0){
	/* this is a new format file, get the rev id and then the name */
	fgetln(sys_ali_rev(t_sy),MAX_NAME_LEN+2,fp);
	fread(&(sys_attrib(t_sy)),1,int_sz,fp);
	fgetln(sys_name(t_sy),MAX_NAME_LEN+2,fp);
    } else {
	/* this is an old style file, so keep the name, and then set */
	/* the version number*/
	strcpy(sys_ali_rev(t_sy),"0.0");
	sys_attrib(t_sy) = ATTRIB_NONE;
    }

    fgetln(sys_desc(t_sy),MAX_DESC_LEN+2,fp);

    fread(&(sys_no_gr(t_sy)),1,sh_sz,fp);
    fread(&(sys_type(t_sy)),1,sh_sz,fp);
    fread(&(sys_chksum(t_sy)),1,int_sz,fp);
    fread(&(sys_nref(t_sy)),1,int_sz,fp);
    fread(&(max_spkr(t_sy)),1,int_sz,fp);
    fread(&(num_spkr(t_sy)),1,int_sz,fp);
   
    /* check to make sure there is enough in the speaker array */
    /* if there isn't, allocate more memory*/
    if (max_spkr(t_sy) > MAX_SPEAKERS){
        free_singarr(spkr_list(t_sy),SPKR *);
        alloc_singarr(spkr_list(t_sy),max_spkr(t_sy),SPKR *);
    }
    for (spkr=0; spkr < num_spkr(t_sy); spkr++){
        alloc_and_init_SPKR(spkr_n(t_sy,spkr)); 
        fgetln(spkr_name(t_sy,spkr),MAX_SPEAKER_NAME_LEN+2,fp);
        fread(&(max_sents(t_sy,spkr)),1,int_sz,fp);
        fread(&(num_sents(t_sy,spkr)),1,int_sz,fp);
        fread(&(spkr_nref(t_sy,spkr)),1,int_sz,fp);

        /* check to make sure there is enough in the speaker array */
        /* if there isn't, allocate more memory*/
        if (max_sents(t_sy,spkr) > MAX_NUM_SENTS){
            free_singarr(sent_list(t_sy,spkr),SENT *);
            alloc_singarr(sent_list(t_sy,spkr),max_sents(t_sy,spkr),
                          SENT *);
        }
        for (st=0; st<num_sents(t_sy,spkr); st++){
            alloc_and_init_SENT(sent_n(t_sy,spkr,st),MAX_NUM_WORDS);
            fgetln(sent_id(t_sy,spkr,st),SENT_ID_LENGTH,fp);
	    fread(&(sent_nref(t_sy,spkr,st)),1,sh_sz,fp);
            wd=0;
            do{
               fread(&(hyp_word(t_sy,spkr,st,wd)),1,pcind_sz,fp);
               fread(&(ref_word(t_sy,spkr,st,wd)),1,pcind_sz,fp);
               fread(&(eval_wrd(t_sy,spkr,st,wd)),1,sh_sz,fp);
	       if (has_ATTRIB(sys_attrib(t_sy),ATTRIB_TIMEMRK)){
		   fread(&(hyp_beg_word(t_sy,spkr,st,wd)),1,fl_sz,fp);
		   fread(&(hyp_dur_word(t_sy,spkr,st,wd)),1,fl_sz,fp);
		   fread(&(ref_beg_word(t_sy,spkr,st,wd)),1,fl_sz,fp);
		   fread(&(ref_dur_word(t_sy,spkr,st,wd)),1,fl_sz,fp);
	       }
            } while (hyp_word(t_sy,spkr,st,wd++) != END_OF_WORDS);

            if (ref_word(t_sy,spkr,st,wd++) != END_OF_WORDS){
                fprintf(stderr,"ERROR: read_SYS_ALIGN, ");
		fprintf(stderr,"Ref not in sync with Hyp\n");
                fprintf(stderr,"       spkr %d '%s' Sentence %d %s\n",
			spkr,spkr_name(t_sy,spkr),st,sent_id(t_sy,spkr,st));
                dump_SENT(sent_n(t_sy,spkr,st));
                exit(-1);
            }
            if (eval_wrd(t_sy,spkr,st,wd++) != END_OF_WORDS){
                fprintf(stderr,"ERROR: read_SYS_ALIGN, Eval not ");
		fprintf(stderr,"in sync with Hyp\n");
                fprintf(stderr,"       spkr %d '%s' Sentence %d %s\n",
			spkr,spkr_name(t_sy,spkr),st,sent_id(t_sy,spkr,st));
                dump_SENT(sent_n(t_sy,spkr,st));
                exit(-1);
            }
	}
    }
    fclose(fp);
    *sys_align = t_sy;
    return(0);
}

/**********************************************************************/
/*  Set the checksum for the SYS_ALIGN Structure.  This should only   */
/*  be called be the alignment program.                               */
/**********************************************************************/
void set_checksum(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs)
{
    unsigned int new_checksum;
    if (compute_checksum(align_str,pcs,&new_checksum) == 0)
        sys_chksum(align_str) = new_checksum;
    else{
        fprintf(stderr,"Error: Unable to compute AND set checksum\n");
        exit(-1);
    }

}

/**********************************************************************/
/*  Does the stored checksum match the recomputed checksum            */
/**********************************************************************/
int pass_checksum(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs)
{
    unsigned int new_checksum;
    int dbg=FALSE, ret;
    ret = compute_checksum(align_str,pcs,&new_checksum);
    if (dbg) fprintf(stderr,"pass_checksum: Returns %d: new C %d, old C %d\n",
                            ret,new_checksum,sys_chksum(align_str));
    if (ret == 0)
        if (new_checksum == sys_chksum(align_str))
            return(TRUE);
    return(FALSE);
}

/**********************************************************************/
/*  Given the SYS_ALIGN structure and the TABLE_OF_CODESETS, compute  */
/*  the checksum.                                                     */
/*  Algorithm:                                                        */
/*       build a list of all lexical items used                       */
/*       check to see it all used items are NOT null strings, if they */
/*            are, quit with an error                                 */
/*       compute the following checksum formula:                      */
/*                                                                    */
/*               / Used       (each char                 \            */
/*              | lexemes      in lexeme)                 |           */
/*              |   ___           ___                     |           */
/*        c =   |   \             \     ind[l] * Ascii[c] | & 0x3fffffff;  */
/*              |   /__           /__                     |           */
/*               \   l             c                     /            */
/*                                                                    */
/*                    where: ind[l] is the codeset index              */
/*                           Ascii[c] is the integer value of a char  */
/**********************************************************************/
int compute_checksum(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, unsigned int *checksum)
{
    PCIND_T *lex_ind;
    int i, j, wnum, lex_id;
    int dbg=FALSE, quit=FALSE;
    int bad_words=0;
    char *word_ptr;
    unsigned int temp_sum;

    /* allocate the memory to make an index array */
    alloc_singZ(lex_ind,lex_num(pcs),PCIND_T,0);
    
    if (dbg) printf("Compute_checksum: Search through the set of sentences\n");
    /* search through all the sentences marking the used lexical items */
    for (i=0;i<num_spkr(align_str);i++){ /* for all speakers */
        for (j=0;j<num_sents(align_str,i);j++){ /* for every speaker sent */
            for (wnum=0; (wnum < MAX_NUM_WORDS) &&
                         ((lex_id=hyp_word(align_str,i,j,wnum)) != END_OF_WORDS);
                 wnum++)
                if (lex_id != DELETION)
                    lex_ind[lex_id] = 1;
            for (wnum=0; (wnum < MAX_NUM_WORDS) &&
                         ((lex_id=ref_word(align_str,i,j,wnum)) != END_OF_WORDS);
                 wnum++)
                if (lex_id != INSERTION)
                    lex_ind[lex_id] = 1;
        }
    }
    { int count=0;
      for (i=0; i<lex_num(pcs); i++)
          if (lex_ind[i] == 1){
/*              printf("GGG: %s\n",lex_uc_word(pcs,i));*/
              count++;
          }
      if (dbg) printf("Compute_checksum: number of lex items %d\n",count);
    }
    
    if (dbg) printf("Compute_checksum: checking for undefinded words\n");
    for (i=0; i<lex_num(pcs); i++)
        if (lex_ind[i] > 0) {
            word_ptr = lex_word(pcs,i);
            if (*word_ptr == NULL_CHAR){
                bad_words++;
                find_lexeme_occurance(align_str,pcs,i);
	    }
        }
    if (bad_words > 0){
        quit=TRUE;
        fprintf(stderr,"Compute_checksum: %d words missing\n",bad_words);
        *checksum = (-1);        
    }
    if (dbg) printf("Compute_checksum: Beginning Checksum Computation\n");
    if (!quit){
        char *str;
        temp_sum=0;
        for (i=0; i<lex_num(pcs); i++)
            if (lex_ind[i] > 0) {
                char *strb;
                int let;
                strb=str=lex_lc_word(pcs,i);
                let=0;
                while (*str != NULL_CHAR){
                    temp_sum = (temp_sum + i*(*str++)) & 0x3fffffff;
/*                    printf("CSUM-DBG: %d %d %s %c\n",temp_sum,i,strb,*(str-1));*/
                    let++;
                }
                if (let == 0){
                    fprintf(stderr,"Error: Compute_checksum lexeme string (%d) is zero length\n",
                                   lex_ind[i]);
                    find_lexeme_occurance(align_str,pcs,i);
                    exit(-1);
		}
            }
        *checksum = temp_sum;
        if (dbg) printf("Compute_checksum: Checksum is %d\n",*checksum);
    }

    free_singarr(lex_ind,PCIND_T);
    if (quit) 
        return(-1);
    return(0);
}
		  
/**********************************************************************/
/*   search through the alignment structure for the first occurrance  */
/*   of a lexeme.                                                     */
/**********************************************************************/
void find_lexeme_occurance(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, PCIND_T lex_id)
{
    int spkr, out, j, wnum, tmp_id;
    char *str;
    out=FALSE;
    str = lex_word(pcs,lex_id);

    printf("Lexeme search id %d: str '%s'",lex_id,str);
    for (spkr=0;!out && spkr<num_spkr(align_str);spkr++){ /* for all speakers */
        for (j=0;!out && j<num_sents(align_str,spkr);j++){ /* for every speaker sent */
            for (wnum=0; (wnum < MAX_NUM_WORDS) &&
                          ((tmp_id=hyp_word(align_str,spkr,j,wnum)) != END_OF_WORDS);
                         wnum++)
                if (lex_id == tmp_id){
                    out=TRUE;
                    printf(" Hyp %s %s word %d\n",spkr_name(align_str,spkr),
                                                           sent_id(align_str,spkr,j),
                                                           wnum);
                    dump_SENT(sent_n(align_str,spkr,j));
            }
            for (wnum=0; (wnum < MAX_NUM_WORDS) &&
                          ((tmp_id=ref_word(align_str,spkr,j,wnum)) != END_OF_WORDS);
                         wnum++)
                if (lex_id == tmp_id){
                    out=TRUE;
                    printf(" Ref %s %s word %d\n",spkr_name(align_str,spkr),
                                                  sent_id(align_str,spkr,j),
                                                  wnum);
                    dump_SENT(sent_n(align_str,spkr,j));
		}
        }
    }
    if (!out) 
        printf(" ID NOT FOUND\n");
}
		  
/**********************************************************************/
/*   get the next line from the file thats terminated by \n or > len  */
/**********************************************************************/
void fgetln(char *str, int len, FILE *fp)
{
    int i=0;

    do {
        str[i] = (char)fgetc(fp);
    }while ((i<len-1) && (str[i++] != NEWLINE));
    if (str[i-1] == NEWLINE)
       str[i-1] = NULL_CHAR;
    else
       str[i] = NULL_CHAR;
}
    
    
/**********************************************************************/
/*   dump the contents of a SYS_ALIGN structure                       */
/**********************************************************************/
void dump_SYS_ALIGN(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int show_scores, int show_times)
{
    int i,j,wnum;
    char *ref_str, **h_strs, **e_strs;

    alloc_char_singarr(ref_str,MAX_SENTENCE_STRING_LEN);
    alloc_char_2dimarr(h_strs,2,MAX_SENTENCE_STRING_LEN);
    alloc_char_2dimarr(e_strs,2,MAX_SENTENCE_STRING_LEN);

    printf("\n\n\t\tDUMP OF SYSTEM ALIGNMENT STRUCTURE\n\n");
    printf("System name:   %s\n",sys_name(align_str));
    printf("System desc:   %s\n\n",sys_desc(align_str));
    printf("System no_gr:  %d\n",sys_no_gr(align_str));
    printf("System type:   ");
    switch (sys_type(align_str)){
        case UNKNOWN_RESULTS_NO_SPKR: printf("UNKNOWN\n\n"); break;
        case UNKNOWN_RESULTS_NO_ID: printf("UNKNOWN WITHOUT IDs\n\n"); break;
        case UNKNOWN_RESULTS_SPKRS: printf("UNKNOWN WITH SPEAKER IDS\n\n");
             break;
        case UNKNOWN_RESULTS_SPKRS_UNIQUE_ID:
             printf("UNKNOWN WITH SPEAKER IDS AND UNIQUE ID's\n\n");
             break;
        case UNKNOWN_RESULTS_SPKRS_NONUNIQUE_ID:
             printf("UNKNOWN WITH SPEAKER IDS AND NONUNIQUE ID's\n\n");
             break;
        case ATIS_RESULTS: printf("ATIS\n\n"); break;
        case TIMIT_RESULTS: printf("TIMIT\n\n"); break;
        case RM_RESULTS: printf("RM\n\n"); break;
        case WSJ_RESULTS: printf("WSJ\n\n"); break;
        case SWB_RESULTS: printf("SWB\n\n"); break;
    }
    printf("System Binary Version: %s\n",sys_ali_rev(align_str));
    printf("System Attributes: %4x\n",sys_attrib(align_str));
    printf("System ChkSum: %d\n",sys_chksum(align_str));
    printf("System Ref Wrds: %d\n\n",sys_nref(align_str));
    printf("Speakers: \n");
    for (i=0;i<num_spkr(align_str);i++)
        printf("   %2d:  %s\n",i,spkr_name(align_str,i));
    printf("\n");
    print_LEX_SUBSET(lsub);
    for (i=0;i<num_spkr(align_str);i++){ /* for all speakers */
        printf("Speaker sentences  %2d:  %s   #ref = %d\n",
	       i,spkr_name(align_str,i), spkr_nref(align_str,i));
        for (j=0;j<num_sents(align_str,i);j++){ /* for every speaker sent */
            printf("id: %s   #ref = %d",sent_id(align_str,i,j),
		   sent_nref(align_str,i,j));
	    if (!show_scores)
		printf("\n");
	    else {
		int corr, sub, ins, del, spl, mrg;
		count_sent_score(sent_n(align_str,i,j),&corr,&sub,&ins,&del,&spl,&mrg);
		printf("   C=%d  S=%d  I=%d  D=%d  ",corr,sub,ins,del);
		if (spl + mrg > 0)
		    printf("T=%d  M=%d  ",spl,mrg);
		printf("WE=%.2f\n",
		       pct((sub+ins+del+mrg+spl),(corr+sub+del+spl+mrg)));
	    }
	    if (!show_times){
		wnum = 0;
		make_readable_n_SENT(pcs,1,ref_str,h_strs,e_strs,
				 MAX_SENTENCE_STRING_LEN,
				 sent_n(align_str,i,j), (SENT *)0, (SENT *)0,
				 (SENT *)0, (SENT *)0, (SENT *)0, (SENT *)0,
				 (SENT *)0, (SENT *)0, (SENT *)0);
		printf("REF:  %s\n",ref_str);
		printf("HYP:  %s\n",h_strs[0]);
		printf("EVAL: %s\n",e_strs[0]);
	    } else {
		dump_SENT_wwt(pcs,sent_n(align_str,i,j));
	    }
	    printf("\n\n");
	}
    }
}

#ifdef TEST_CODE
all_SYS_ALIGN_spkr_sent_match(sa_list)
SYS_ALIGN_LIST *sa_list;
{
    int sys1, sys2;

    for (spk1=0;spk1 < num_spkr(sys1); spk1++){ /* for all speaker sys1 */
        /**** find the matching speaker */
        for (spk2=0;spk2 < num_spkr(sys2); spk2++)
            if (strcmp(spkr_name(sys1,spk1), spkr_name(sys2,spk2)) == 0)
                break;
        /**** the the speakers match, start on the sentences */
        if (spk2 != num_spkr(sys2)){
            /**** for all sents in sys1,spkr1 */
            for (snt1 = 0; snt1 < num_sents(sys1,spk1); snt1++){
                /**** for all sents in sys2,spkr2 */
                for (snt2 = 0; snt2 < num_sents(sys2,spk2); snt2++) 
                   /**** if the sentences are the same, compare them */
                   if(strcmp(sent_id(sys1,spk1,snt1),
                             sent_id(sys2,spk2,snt2)) == 0){
                        test_comp_sents(pcs,sent_n(sys1,spk1,snt1),
                                            sent_n(sys2,spk2,snt2),
                                            seg_l, num_seg, min_num_good);
                        sent_cnt++;
                        break;
                   }
	    }
	} else {
            fprintf(stderr,"Warning: Speaker %s is in system %s but not system %s\n",
                            spkr_name(sys1,spk1),sys_name(sys1),sys_name(sys2));
        }
    }
#endif

/**********************************************************************/
/*   dump the contents of a SYS_ALIGN structure only numerically      */
/**********************************************************************/
void dump_SYS_ALIGN_by_num(SYS_ALIGN *align_str)
{
    int i,j,wnum;

    printf("\n\n\t\tDUMP OF SYSTEM ALIGNMENT STRUCTURE NUMERICALLY\n\n");
    printf("System name:     %s\n",sys_name(align_str));
    printf("System desc:     %s\n",sys_desc(align_str));
    printf("System no_gr:    %d\n",sys_no_gr(align_str));
    printf("System type:     %d\n",sys_type(align_str));
    printf("System ChkSum:   %d\n\n",sys_chksum(align_str));
    printf("System Ref Wrds: %d\n\n",sys_nref(align_str));
    printf("Speakers: \n");
    for (i=0;i<num_spkr(align_str);i++)
        printf("   %2d:  %s\n",i,spkr_name(align_str,i));
    printf("\n");
    for (i=0;i<num_spkr(align_str);i++){ /* for all speakers */
        printf("Speaker sentences  %2d:  %s    #ref = %d\n",
	       i,spkr_name(align_str,i), spkr_nref(align_str,i));
        for (j=0;j<num_sents(align_str,i);j++){ /* for every speaker sent */
            printf("type %s\n",sent_id(align_str,i,j));
            wnum = 0;
            while ((wnum < MAX_NUM_WORDS) &&
                   (hyp_word(align_str,i,j,wnum) != END_OF_WORDS))
                printf("%4d ",hyp_word(align_str,i,j,wnum++));
            printf("\n");
            wnum = 0;
            while ((wnum < MAX_NUM_WORDS) &&
                   (ref_word(align_str,i,j,wnum) != END_OF_WORDS))
                printf("%4d ",ref_word(align_str,i,j,wnum++));
            printf("\n");
            wnum = 0;
            while ((wnum < MAX_NUM_WORDS) &&
                   (eval_wrd(align_str,i,j,wnum) != END_OF_WORDS))
                printf("  %2x ",eval_wrd(align_str,i,j,wnum++));
            printf("\n\n");
	}
    }
}

/*******************************************************************/
/*  THIS procedure removes all homophone that are labeled as       */
/*  correct for a list of SYS_ALIGN structures                     */
/*******************************************************************/
void rm_HOMO_CORR_from_SYS_ALIGN_LIST(SYS_ALIGN_LIST *sa_list)
{
    int sys;
 
    for (sys=0;sys<num_sys(sa_list);sys++)
        rm_HOMO_CORR_from_SYS_ALIGN(sys_i(sa_list,sys));
}

/*******************************************************************/
/*  THIS procedure removes all homophone that are labelled as      */
/*  correct                                                        */
/*******************************************************************/
void rm_HOMO_CORR_from_SYS_ALIGN(SYS_ALIGN *align_str)
{
    int spkr, snt, wrd; 

    fprintf(stderr,"Homophones will be scored as substitutions\n");
    for (spkr=0;spkr < num_spkr(align_str); spkr++){
        for (snt=0;snt < num_sents(align_str,spkr); snt++){
            for (wrd = 0;ref_word(align_str,spkr,snt,wrd) !=
                         END_OF_WORDS; wrd ++){
                if (eval_wrd(align_str,spkr,snt,wrd) == EVAL_CORR_HOMO){
                    eval_wrd(align_str,spkr,snt,wrd)=EVAL_SUB;
		}
            }
	}
    }
}

void copy_SENT(SENT *to, SENT *from)
{
    int wd;

    strncpy(s_id(to),s_id(from),SENT_ID_LENGTH-1);
    s_nref(to) = s_nref(from);
    for (wd = 0; wd < MAX_NUM_WORDS; wd++){
	s_ref_wrd(to,wd)      =   s_ref_wrd(from,wd);
	s_ref_beg_wrd(to,wd)  =   s_ref_beg_wrd(from,wd);
	s_ref_dur_wrd(to,wd)  =   s_ref_dur_wrd(from,wd);
    }
    for (wd = 0; wd < MAX_NUM_WORDS; wd++){
	s_hyp_wrd(to,wd)      =   s_hyp_wrd(from,wd);
	s_hyp_beg_wrd(to,wd)  =   s_hyp_beg_wrd(from,wd);
	s_hyp_dur_wrd(to,wd)  =   s_hyp_dur_wrd(from,wd);
    }
    for (wd = 0; wd < MAX_NUM_WORDS; wd++)
	s_eval_wrd(to,wd)     =   s_eval_wrd(from,wd);

}

/*******************************************************************/
/*  A simple dump of a sentence,   this version only dumps numbers */
/*******************************************************************/
void dump_SENT(SENT *sent)
{
    int wd;

    printf("address %d\n",(int)sent);
    printf("id %s\n",s_id(sent));

    wd = 0;
    printf("Ref:  ");
    while (s_ref_wrd(sent,wd) != END_OF_WORDS)
       printf("%7d ",s_ref_wrd(sent,wd++));
    printf("\n");
    wd = 0;
    printf("Hyp:  ");
    while (s_hyp_wrd(sent,wd) != END_OF_WORDS)
       printf("%7d ",s_hyp_wrd(sent,wd++));
    printf("\n");
    wd = 0;
    printf("Eval: ");
    while (s_hyp_wrd(sent,wd) != END_OF_WORDS)
       printf("%7d ",s_eval_wrd(sent,wd++));
    printf("\n");
}

/*******************************************************************/
/*  A simple dump of a sentence with strings as output             */
/*******************************************************************/
void dump_SENT_wrds(TABLE_OF_CODESETS *pcs, SENT *sent)
{
    int wd, set;

    printf("id %d\n",(int)s_id(sent));
    for (wd = 0;s_ref_wrd(sent,wd) != END_OF_WORDS; wd++)
       if (s_ref_wrd(sent,wd) != INSERTION)
           printf("%7s ",lex_uc_word(pcs,s_ref_wrd(sent,wd)));
       else
           printf("**** ");
    printf("\n");
    for (wd=0; s_hyp_wrd(sent,wd) != END_OF_WORDS; wd++)
       if (s_hyp_wrd(sent,wd) != DELETION)
           printf("%7s ",lex_uc_word(pcs,s_hyp_wrd(sent,wd)));
       else
           printf("**** ");
    printf("\n");
    for (wd=0; s_hyp_wrd(sent,wd) != END_OF_WORDS; wd++){
       set = eval_set(s_eval_wrd(sent,wd));
       switch (eval_super_set(s_eval_wrd(sent,wd))) {
           case EVAL_CORR:	printf("CORR");
                                break;
           case EVAL_SUB:	printf("SUB");
                                break;
           case EVAL_DEL:	printf("DEL");
                                break;
           case EVAL_INS:	printf("INS");
                                break;
           case EVAL_SPT:	printf("SPT");
                                break;
           case EVAL_MRG:	printf("MRG");
                                break;
           default:             printf("OOPS");
       }
       if (set != (-1))
           printf("%c",(char)(set+'1'));
       printf(" ");
    }
    printf("\n");
}

/*******************************************************************/
/*  A simple dump of a sentence with strings, and time marks       */
/*******************************************************************/
void dump_SENT_wwt(TABLE_OF_CODESETS *pcs, SENT *sent)
{
    int wd=0;

    printf("%20s %6s %5s  ->  %20s %6s %5s   Eval\n",
	   "       Ref"," Beg"," Dur","       Hyp"," Beg"," Dur");
    for (wd=0; s_ref_wrd(sent,wd) != END_OF_WORDS; wd++){
	if (s_ref_wrd(sent,wd) == INSERTION)
	    printf("%20s %6s %5s","****","","");
	else 
	    printf("%20s %6.2f %5.2f",
		   s_ref_wrd(sent,wd) != s_hyp_wrd(sent,wd) ? 
		       lex_uc_word(pcs,s_ref_wrd(sent,wd)) : 
		       lex_lc_word(pcs,s_ref_wrd(sent,wd)),
		   s_ref_beg_wrd(sent,wd),s_ref_dur_wrd(sent,wd));
	printf("  ->  ");
	if (s_hyp_wrd(sent,wd) == DELETION)
	    printf("%20s %6s %5s","****","","");
	else 
	    printf("%20s %6.2f %5.2f",
		   s_ref_wrd(sent,wd) != s_hyp_wrd(sent,wd) ? 
		       lex_uc_word(pcs,s_hyp_wrd(sent,wd)) : 
		       lex_lc_word(pcs,s_hyp_wrd(sent,wd)),
		   s_hyp_beg_wrd(sent,wd),s_hyp_dur_wrd(sent,wd));
	printf("     ");
        if (is_CORR(s_eval_wrd(sent,wd)))
            printf("%s",CORR_MARKER);
        else if (is_INS(s_eval_wrd(sent,wd)))
            printf("%s",INS_MARKER);
        else if (is_DEL(s_eval_wrd(sent,wd)))
            printf("%s",DEL_MARKER);
        else if (is_SUB(s_eval_wrd(sent,wd)))
            printf("%s",SUB_MARKER);
        else if (is_SPLIT(s_eval_wrd(sent,wd)))
            printf("%s",SPLIT_MARKER);
        else if (is_MERGE(s_eval_wrd(sent,wd)))
            printf("%s",MERGE_MARKER);
	
	printf("\n");
    }	wd++;
    printf("\n");
}

/*******************************************************************/
/*  a front end to make_readable_n_sents,  this function allocates */
/*  the char strings to pass down, to make_readable_n_sents        */
/*  then prints the aligned sentences                              */
/*******************************************************************/
void print_readable_n_SENT(TABLE_OF_CODESETS *pcs, int num_snt, SENT *snt0, SENT *snt1, SENT *snt2, SENT *snt3, SENT *snt4, SENT *snt5, SENT *snt6, SENT *snt7, SENT *snt8, SENT *snt9)
{
    static char **h_strs=NULL, *ref_str, **e_strs;
    int snt;

    if (h_strs == NULL){
        alloc_char_2dimarr(h_strs,MAX_PR_SNT,MAX_SENTENCE_STRING_LEN);
        alloc_char_2dimarr(e_strs,MAX_PR_SNT,MAX_SENTENCE_STRING_LEN);
        alloc_char_singarr(ref_str,MAX_SENTENCE_STRING_LEN);
    }

    if (make_readable_n_SENT(pcs,num_snt,ref_str,h_strs,e_strs,
			     MAX_SENTENCE_STRING_LEN,snt0,snt1,snt2,
			     snt3,snt4,snt5,snt6,snt7,snt8,snt9) == 0){
	printf("ref:  %s\n\n",ref_str);
	for (snt=0; snt< num_snt; snt++){
	    printf("hyp%1d: %s\n",snt,h_strs[snt]);
	    printf("eval: %s\n\n",e_strs[snt]);
	}
	printf("\n");
    } else {
	SENT *sent;
	char ref[MAX_SENTENCE_STRING_LEN], hyp[MAX_SENTENCE_STRING_LEN];
	char eval[MAX_SENTENCE_STRING_LEN];
	for (snt=0; snt< num_snt; snt++){
            switch (snt) {
                case 0: sent = snt0; break;
                case 1: sent = snt1; break;
                case 2: sent = snt2; break;
                case 3: sent = snt3; break;
                case 4: sent = snt4; break;
                case 5: sent = snt5; break;
                case 6: sent = snt6; break;
                case 7: sent = snt7; break;
                case 8: sent = snt8; break;
                case 9: sent = snt9; break;
	    }
	    make_readable_SENT(ref,hyp,eval,MAX_SENTENCE_STRING_LEN,sent,pcs);

	    printf("REF:  %s\n",ref);
	    printf("HYP:  %s\n",hyp);
	    printf("EVAL: %s\n",eval);
	}    
    }
}

/*******************************************************************/
/*   this is a front end to make_readable_n_sent() to make only    */
/*   one sentence                                                  */
/*******************************************************************/
void make_readable_SENT(char *ref, char *hyp, char *eval, int buff_len, SENT *sent, TABLE_OF_CODESETS *pcs)
{
    char *hyp_strs[1], *eval_strs[1];
    
    hyp_strs[0] = hyp;
    eval_strs[0] = eval;
    make_readable_n_SENT(pcs,1,ref,hyp_strs,eval_strs,buff_len,sent,
			 (SENT *)0,(SENT *)0, (SENT *)0, (SENT *)0, (SENT *)0,
			 (SENT *)0, (SENT *)0, (SENT *)0, (SENT *)0);
}

/*******************************************************************/
/*  this function take n sentences that are the same sent_type     */
/*  and number, then aligns all the sentences to the reference     */
/*  sentence in snt1, making spaces for insertions, deletions and  */
/*  substitutions                                                  */
/*******************************************************************/
int make_readable_n_SENT(TABLE_OF_CODESETS *pcs, int num_snt, char *ref_str, char **h_strs, char **e_strs, int buff_len, SENT *snt0, SENT *snt1, SENT *snt2, SENT *snt3, SENT *snt4, SENT *snt5, SENT *snt6, SENT *snt7, SENT *snt8, SENT *snt9)
{
    int max_ins_len, max_len, snt, err, sents_limit=10;
    int st_of_max_len, size, ns, i, tmp;
    static char **emty_space, **blank_space, *ref, **hyp, **eval;
    static SENT **s_list;
    static int *h_ind, *e_ind;

    if (emty_space == NULL){
         /* make the emty_spaces arr */
        alloc_char_2dimarr(emty_space,MAX_LEX_WORD_LEN,MAX_LEX_WORD_LEN+1);
        alloc_char_2dimarr(blank_space,MAX_LEX_WORD_LEN,MAX_LEX_WORD_LEN+1);
        make_space(emty_space,MAX_LEX_WORD_LEN,ASTERISK);
        make_space(blank_space,MAX_LEX_WORD_LEN,SPACE);
        alloc_SENT_list(s_list,MAX_PR_SNT);
        alloc_singarr(hyp,MAX_PR_SNT,char *);
        alloc_singarr(eval,MAX_PR_SNT,char *);
        alloc_int_singarr(h_ind,MAX_PR_SNT);
        alloc_int_singarr(e_ind,MAX_PR_SNT);
    }

    ref = ref_str;
    *ref  = NULL_CHAR;
    /* store the sent pointers into a SENT list */
    for (i=0; i<sents_limit; i++)
        if (num_snt >= i+1){
            switch (i) {
                case 0: s_list[i] = snt0; break;
                case 1: s_list[i] = snt1; break;
                case 2: s_list[i] = snt2; break;
                case 3: s_list[i] = snt3; break;
                case 4: s_list[i] = snt4; break;
                case 5: s_list[i] = snt5; break;
                case 6: s_list[i] = snt6; break;
                case 7: s_list[i] = snt7; break;
                case 8: s_list[i] = snt8; break;
                case 9: s_list[i] = snt9; break;
	    }
            hyp[i] = h_strs[i];  *(hyp[i]) = NULL_CHAR;  h_ind[i] = 0;
            eval[i] = e_strs[i]; *(eval[i]) = NULL_CHAR; e_ind[i] = 0;
	}

    /* check the reference words of each system to make sure */
    /* the are the same for each system                      */
    for (ns=1; ns<num_snt; ns++){
	int w0, w1;
	for (w0=0, w1=0; s_ref_wrd(s_list[0],w0) != END_OF_WORDS; w0++,w1++){
	    while (s_ref_wrd(s_list[0],w0) == INSERTION)
		w0++;
	    while (s_ref_wrd(s_list[ns],w1) == INSERTION)
		w1++;
	    if (s_ref_wrd(s_list[0],w0) != s_ref_wrd(s_list[ns],w1)){
		printf("Refs of systems %d and %d differ and word %d and %d\n",
		       0,ns,w0,w1);
		return(100);
	    }
	}
    }	

    st_of_max_len = size = 0;
    for (ns=0; ns<num_snt; ns++){
        tmp = 0;
        while (s_eval_wrd(s_list[ns],tmp)!=END_OF_WORDS)
           tmp++;
        if (tmp > size){
            st_of_max_len = ns;
            size = tmp;
        }
    }
        
 
    while (s_hyp_wrd(s_list[st_of_max_len],h_ind[st_of_max_len])!=END_OF_WORDS){
        max_ins_len = 0;
        /*   check to see if the current word of any sentence is an INSERT */
        /*   if there is, find the maximum length of the INSERTIONS        */
        for (snt=0; snt< num_snt; snt++){
            if ((is_INS(s_eval_wrd(s_list[snt],h_ind[snt]))) &&
                (s_eval_wrd(s_list[snt],h_ind[snt]) != END_OF_WORDS)){
                tmp = strlen(lex_word(pcs,s_hyp_wrd(s_list[snt],h_ind[snt])));
                if (max_ins_len < tmp)
                   max_ins_len = tmp;
	    }
        }
        if (max_ins_len > 0){
            /* there is an insertion somewhere */
            strcpy(ref,emty_space[max_ins_len]);
            move_to_null(ref);
            add_blank_to_end(ref);

            for (snt=0; snt< num_snt; snt++){
                if ((! is_INS(s_eval_wrd(s_list[snt],h_ind[snt]))) ||
                    (s_eval_wrd(s_list[snt],h_ind[snt]) == END_OF_WORDS)){
                    /* this word not an insertion, pad with emty_spaces */
                    strcpy(hyp[snt],blank_space[max_ins_len+1]);
                    move_to_null(hyp[snt]);

                    strcpy(eval[snt],blank_space[max_ins_len+1]);
                    move_to_null(eval[snt]);
                }
                else {
                     /* insertion, copy and pad */
                    strncpy_pad(hyp[snt],lex_uc_word(pcs,s_hyp_wrd(s_list[snt],
                                             h_ind[snt])),max_ins_len,SPACE);
                    move_to_null(hyp[snt]);
                    add_blank_to_end(hyp[snt]);
                    copy_marker(s_eval_wrd(s_list[snt],h_ind[snt]),eval[snt],
                                INS_MARKER,max_ins_len);
                    move_to_null(eval[snt]);
                    h_ind[snt]++;
		}
	    }
	}
        else{
            err=0;
          /* take care of subs, dels, and correct words */
            /* find the maximum length of the current words */
            max_len = strlen(lex_uc_word(pcs,s_ref_wrd(s_list[0],h_ind[0])));
            for (snt=0; snt< num_snt; snt++){
              if (! is_DEL(s_eval_wrd(s_list[snt],h_ind[snt]))){
                tmp = wrdlen(lex_uc_word(pcs,s_hyp_wrd(s_list[snt],h_ind[snt])));
                if (max_len < tmp)
                   max_len = tmp;
	      }
            }
            /* copy in all the hyps words.  if CORR, use lower case words */
            for (snt=0; snt< num_snt; snt++){
              if (! is_DEL(s_eval_wrd(s_list[snt],h_ind[snt])))
                if (is_CORR(s_eval_wrd(s_list[snt],h_ind[snt]))){
                    strncpy_pad(hyp[snt],lex_lc_word(pcs,
                         s_hyp_wrd(s_list[snt],h_ind[snt])),max_len+1,SPACE);
                    copy_marker(s_eval_wrd(s_list[snt],h_ind[snt]),eval[snt],
                                CORR_MARKER_AUX,max_len);
		}
                else if (is_SUB(s_eval_wrd(s_list[snt],h_ind[snt]))){
                    strncpy_pad(hyp[snt],lex_uc_word(pcs,
                         s_hyp_wrd(s_list[snt],h_ind[snt])),max_len+1,SPACE);
                    err++;
                    copy_marker(s_eval_wrd(s_list[snt],h_ind[snt]),eval[snt],
                                SUB_MARKER,max_len);
		}
                else if (is_SPLIT(s_eval_wrd(s_list[snt],h_ind[snt]))){
                    strncpy_pad(hyp[snt],lex_uc_word(pcs,
                         s_hyp_wrd(s_list[snt],h_ind[snt])),max_len+1,SPACE);
                    err++;
                    copy_marker(s_eval_wrd(s_list[snt],h_ind[snt]),eval[snt],
                                SPLIT_MARKER,max_len);
		}
                else if (is_MERGE(s_eval_wrd(s_list[snt],h_ind[snt]))){
                    strncpy_pad(hyp[snt],lex_uc_word(pcs,
                         s_hyp_wrd(s_list[snt],h_ind[snt])),max_len+1,SPACE);
                    err++;
                    copy_marker(s_eval_wrd(s_list[snt],h_ind[snt]),eval[snt],
                                MERGE_MARKER,max_len);
		}
                else {
                    fprintf(stderr,"Error: unknown evaluation marker %2x\n",
                                   s_eval_wrd(s_list[snt],h_ind[snt]));
                    exit(-1);
                }
              else{
                strncpy_pad(hyp[snt],emty_space[max_len],max_len+1,SPACE);
                err++;
                copy_marker(s_eval_wrd(s_list[snt],h_ind[snt]),eval[snt],
                                DEL_MARKER,max_len);
	      }
              move_to_null(hyp[snt]);
              h_ind[snt]++;
              move_to_null(eval[snt]);
            }
            if (err > 0)
               strncpy_pad(ref,lex_uc_word(pcs,s_ref_wrd(s_list[0],h_ind[0]-1)),
                        max_len+1,SPACE);
            else
               strncpy_pad(ref,lex_lc_word(pcs,
                        s_ref_wrd(s_list[0],h_ind[0]-1)),max_len+1,SPACE);
            move_to_null(ref);
	}
        /* do a quick check on the size to make sure there's no overflow*/
        if (strlen(ref_str) >= buff_len){
            fprintf(stderr,"Error: There was a overflow when creating the");
            fprintf(stderr," readable sentence string on utt '%s'\n",
		    s_id(s_list[0]));
            exit(-1);
	}
    }
    return(0);
}

void copy_marker(short int eval, char *eval_buff, char *marker, int len)
{
    int set;
    char buff[10];

    set = eval_set(eval);
    if (set > 0){ /* for a set */
        int first_set=(-1), counter=0;
        while (set > 0){
            if ((set & 0x0001) > 0)
                if (first_set == (-1))
                    first_set = counter;
                else
                    first_set = (-2);
            counter++;
            set >>= 1;
        }
        if (first_set >= 0)
            sprintf(buff,"%s%c",marker,(char)(first_set+'0'));
        else
            sprintf(buff,"%s%c",marker,'*');
    }
    else
        if (strcmp(marker,CORR_MARKER_AUX) != 0)
            sprintf(buff,"%s",marker);
        else
            sprintf(buff,"%s","");
    strncpy_pad(eval_buff,buff,len+1,SPACE);
}

void make_readable_2_seg(TABLE_OF_CODESETS *pcs, char *ref_str, char **hyp_strs, SENT *snt1, int from1, int to1, SENT *snt2, int from2, int to2)
{
    int max_ins_len, max_len, snt, tmp;
    static char **space, *ref, **hyp;
    static SENT **s_list;
    static int *h_ind;

    if (space == NULL){
        alloc_char_2dimarr(space,MAX_LEX_WORD_LEN,MAX_LEX_WORD_LEN+1);
        make_space(space,MAX_LEX_WORD_LEN,SPACE);
        alloc_SENT_list(s_list,2);
        alloc_singarr(hyp,2,char *);
        alloc_int_singarr(h_ind,2);
    }

    ref = ref_str;
    *ref  = NULL_CHAR;
    s_list[0] = snt1, hyp[0]=hyp_strs[0], *(hyp[0])=NULL_CHAR, h_ind[0]=from1;
    s_list[1] = snt2, hyp[1]=hyp_strs[1], *(hyp[1])=NULL_CHAR, h_ind[1]=from2;

    while ((h_ind[0] <= to1) && (h_ind[1] <= to2)){
        /*   check to see if the current word of any sentence is an INSERT */
        /*   if there is, find the maximum length of the INSERTIONS        */
        max_ins_len = 0;
        for (snt=0; snt< 2; snt++){
            if (is_INS(s_eval_wrd(s_list[snt],h_ind[snt]))){
                tmp = strlen(lex_uc_word(pcs,s_hyp_wrd(s_list[snt],h_ind[snt])));
                if (max_ins_len < tmp)
                   max_ins_len = tmp;
	    }
        }
        if (max_ins_len > 0){
            /* there is an insertion somewhere */
            strcpy(ref,space[max_ins_len+1]);
            move_to_null(ref);
            for (snt=0; snt< 2; snt++){
                if (! is_INS(s_eval_wrd(s_list[snt],h_ind[snt]))){
                    /* this word not an insertion, pad with spaces */
                    strcpy(hyp[snt],space[max_ins_len+1]);
                    move_to_null(hyp[snt]);
                }
                else {
                    /* insertion, copy and pad */
                    strcpy(hyp[snt],lex_uc_word(pcs,s_hyp_wrd(s_list[snt],
                                                 h_ind[snt])));
                    move_to_null(hyp[snt]);
                    strcpy(hyp[snt],space[1]);
                    move_to_null(hyp[snt]);
                    h_ind[snt]++;
		}
	    }
	}
        else{
          /* take care of subs, dels, and correct words */
            /* find the maximum length of the current words */
            max_len = strlen(lex_uc_word(pcs,s_ref_wrd(s_list[0],h_ind[0])));
            for (snt=0; snt< 2; snt++){
              if (! is_DEL(s_eval_wrd(s_list[snt],h_ind[snt]))){
                tmp = strlen(lex_uc_word(pcs,s_hyp_wrd(s_list[snt],h_ind[snt])));
                if (max_len < tmp)
                   max_len = tmp;
	      }
            }
            /* copy in the reference word */
            strncpy_pad(ref,lex_lc_word(pcs,s_ref_wrd(s_list[0],h_ind[0])),
                        max_len+1,SPACE);
            move_to_null(ref);
            /* copy in all the hyps words.  if CORR, use lower case words */
            for (snt=0; snt< 2; snt++){
              if (! is_DEL(s_eval_wrd(s_list[snt],h_ind[snt])))
                if (is_CORR(s_eval_wrd(s_list[snt],h_ind[snt])))
                    strncpy_pad(hyp[snt],lex_lc_word(pcs,
                         s_hyp_wrd(s_list[snt],h_ind[snt])),max_len+1,SPACE);
                else
                    strncpy_pad(hyp[snt],lex_uc_word(pcs,
                         s_hyp_wrd(s_list[snt],h_ind[snt])),max_len+1,SPACE);
              else
                strcpy(hyp[snt],space[max_len+1]);
              move_to_null(hyp[snt]);
              h_ind[snt]++;
            }
	}
    }
}



/***************************************************************************/
/*   the following three procedures prints the alignments in an SA_LIST    */
/*       diff_or_com is a flag to say whether or not to print the          */
/*             alignments that are different or rather to print all the    */
/*             common sentences                                            */
/***************************************************************************/
void dump_SA_LIST(SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, LEX_SUBSET *lsub, int all, int diff, int score_diff, int errors, int word_indexes)
{
    int *spkr_ptr, *sent_ptr, sys, spkr, tspkr, tsnt, snt1;
    int missing_spkr, missing_snt, results_type;
    char *cur_spkr;

    alloc_int_singarr(spkr_ptr,num_sys(sa_list));
    alloc_int_singarr(sent_ptr,num_sys(sa_list));
    if (diff)
        printf("DUMP OF COMMON SENTENCES WITH DIFFERENCES FOR THE SYSTEMS:\n\n");
    else if (all)
        printf("DUMP OF COMMON SENTENCES FOR THE SYSTEMS:\n\n");
    else if (errors)
        printf("DUMP OF COMMON SENTENCES WITH ERRORS FOR THE SYSTEMS:\n\n");
      
    for (sys=0; sys<num_sys(sa_list); sys++)
        printf("    %s -> %s\n",sys_name(sys_i(sa_list,sys)),
                                sys_desc(sys_i(sa_list,sys)));
    printf("\n");
    if (lsub != (LEX_SUBSET *)NULL)
        print_LEX_SUBSET(lsub);
    printf("\n\n");

    results_type = sys_type(sys_i(sa_list,0));
    /* Set the search speaker */
    for (spkr=0; spkr<num_spkr(sys_i(sa_list,0)); spkr++){
        cur_spkr = spkr_name(sys_i(sa_list,0),spkr);
        spkr_ptr[0] = spkr;
        /* find the matching speaker in the rest of the systems       */
        /* if none exists for a system, go to the next search speaker */
        for (sys=1; sys<num_sys(sa_list); sys++){
            spkr_ptr[sys] = -1;
            for (tspkr=0; tspkr<num_spkr(sys_i(sa_list,sys)); tspkr++)
                if (strcmp(cur_spkr,spkr_name(sys_i(sa_list,0),tspkr)) == 0)
                    spkr_ptr[sys] = tspkr;
	}
        missing_spkr = FALSE;
        for (sys=0; sys<num_sys(sa_list); sys++){
            if (spkr_ptr[sys] == -1)
                missing_spkr = TRUE;
	}
        if (!missing_spkr)
            for (snt1 = 0; snt1 < num_sents(sys_i(sa_list,0),spkr); snt1++){
                sent_ptr[0] = snt1;
                for (sys=1; sys<num_sys(sa_list); sys++){
                    sent_ptr[sys] = -1;
                    for (tsnt=0;
                         tsnt<num_sents(sys_i(sa_list,sys),spkr_ptr[sys]);
                         tsnt++)
                        if(id_equal(sent_id(sys_i(sa_list,0),spkr,snt1),
                            sent_id(sys_i(sa_list,sys),spkr_ptr[sys],tsnt),
                            results_type) == 0)
                            sent_ptr[sys] = tsnt;
		}
                missing_snt = FALSE;
                for (sys=0; sys<num_sys(sa_list); sys++)
                    if (sent_ptr[sys] == -1)
                        missing_snt = TRUE;
                if (!missing_snt)
                    print_sent(sa_list,pcs,spkr_ptr,sent_ptr,
                               all,diff,score_diff,errors,word_indexes);
	    }
    }
    free_singarr(sent_ptr,int);
    free_singarr(spkr_ptr,int);
}

/************************************************************************/
/*   this function takes the sentence pointers from each system and     */
/*   passes them to the "make_readable_n_sent" procedure                */
void print_sent(SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, int *spkr_ptr, int *sent_ptr, int all, int diff, int score_diff, int errors, int word_indexes)
{
    int sys;
    static char **h_strs=NULL, *ref_str, **e_strs;
    SENT *snt0, *snt1, *snt2, *snt3, *snt4, *snt5, *snt6, *snt7, *snt8, *snt9;
    int max_sysname=4, tmp; char name_fmt[20];

    if (h_strs == NULL){
	alloc_char_2dimarr(h_strs,num_sys(sa_list),MAX_SENTENCE_STRING_LEN);
	alloc_char_2dimarr(e_strs,num_sys(sa_list),MAX_SENTENCE_STRING_LEN);
	alloc_char_singarr(ref_str,MAX_SENTENCE_STRING_LEN);
    }
    if ((all) ||
	(diff && a_diff_in_sent_exists(sa_list,spkr_ptr,sent_ptr)) ||
	(score_diff && a_diff_score_in_sent_exists(sa_list,spkr_ptr,sent_ptr)) ||
	(errors && errors_exist(sa_list,spkr_ptr,sent_ptr))){
	snt0 = snt1 = snt2 = snt3 = snt4 = snt5 = snt6 = snt7 = snt8 = snt9 =NULL;
	if (word_indexes){
	    printf("spkr:  %s\nid:    %s\n\n",
		   spkr_name(sys_i(sa_list,0),spkr_ptr[0]),
		   sent_id(sys_i(sa_list,0),spkr_ptr[0],sent_ptr[0]));
	    for (sys=0; sys<num_sys(sa_list); sys++){
		printf("System: %s\n",sys_name(sys_i(sa_list,sys)));
		dump_SENT(sent_n(sys_i(sa_list,sys),spkr_ptr[sys],sent_ptr[sys]));
	    }
	    printf("\n\n");
	} else {
	    if (num_sys(sa_list) > 0)
		snt0 = sent_n(sys_i(sa_list,0),spkr_ptr[0],sent_ptr[0]);
	    if (num_sys(sa_list) > 1) 
		snt1 = sent_n(sys_i(sa_list,1),spkr_ptr[1],sent_ptr[1]);
	    if (num_sys(sa_list) > 2) 
		snt2 = sent_n(sys_i(sa_list,2),spkr_ptr[2],sent_ptr[2]);
	    if (num_sys(sa_list) > 3) 
		snt3 = sent_n(sys_i(sa_list,3),spkr_ptr[3],sent_ptr[3]);
	    if (num_sys(sa_list) > 4) 
		snt4 = sent_n(sys_i(sa_list,4),spkr_ptr[4],sent_ptr[4]);
	    if (num_sys(sa_list) > 5) 
		snt5 = sent_n(sys_i(sa_list,5),spkr_ptr[5],sent_ptr[5]);
	    if (num_sys(sa_list) > 6) 
		snt6 = sent_n(sys_i(sa_list,6),spkr_ptr[6],sent_ptr[6]);
	    if (num_sys(sa_list) > 7) 
		snt7 = sent_n(sys_i(sa_list,7),spkr_ptr[7],sent_ptr[7]);
	    if (num_sys(sa_list) > 8) 
		snt8 = sent_n(sys_i(sa_list,8),spkr_ptr[8],sent_ptr[8]);
	    if (num_sys(sa_list) > 9) 
		snt9 = sent_n(sys_i(sa_list,9),spkr_ptr[9],sent_ptr[9]);

	    if (make_readable_n_SENT(pcs,num_sys(sa_list),ref_str,h_strs,
				     e_strs,MAX_SENTENCE_STRING_LEN,
				     snt0,snt1,snt2,snt3,snt4,snt5,snt6,snt7,snt8,snt9) == 0){
		printf("spkr:  %s\nid:    %s\n\n",
		       spkr_name(sys_i(sa_list,0),spkr_ptr[0]),
		       sent_id(sys_i(sa_list,0),spkr_ptr[0],sent_ptr[0]));
		for (sys=0;sys<num_sys(sa_list);sys++)
		    if (max_sysname < (tmp=strlen(sys_name(sys_i(sa_list,sys)))))
			max_sysname=tmp;
		sprintf(name_fmt," %%%ds: ",max_sysname);
		printf(name_fmt,"ref");
		printf("%s\n\n",ref_str);
		for (sys=0;sys<num_sys(sa_list);sys++){
		    printf(name_fmt,sys_name(sys_i(sa_list,sys)));
		    printf("%s\n",h_strs[sys]);
		    printf(name_fmt,"eval");
		    printf("%s\n",e_strs[sys]);
		}
		printf("\n");
	    } else {
		SENT *sent;
		char ref[MAX_SENTENCE_STRING_LEN], 
		     hyp[MAX_SENTENCE_STRING_LEN], 
		     eval[MAX_SENTENCE_STRING_LEN];
		for (sys=0;sys<num_sys(sa_list);sys++)
		    if (max_sysname < (tmp=strlen(sys_name(sys_i(sa_list,sys)))))
			max_sysname=tmp;
		printf("spkr:  %s\nid:    %s\nReference Transcripts are different: Printing individual systems\n\n",
		       spkr_name(sys_i(sa_list,0),spkr_ptr[0]),
		       sent_id(sys_i(sa_list,0),spkr_ptr[0],sent_ptr[0]));
		sprintf(name_fmt," %%%ds: ",max_sysname);
		for (sys=0;sys<num_sys(sa_list);sys++){
		    switch (sys) {
		      case 0: sent = snt0; break;
		      case 1: sent = snt1; break;
		      case 2: sent = snt2; break;
		      case 3: sent = snt3; break;
		      case 4: sent = snt4; break;
		      case 5: sent = snt5; break;
		      case 6: sent = snt6; break;
		      case 7: sent = snt7; break;
		      case 8: sent = snt8; break;
		      case 9: sent = snt9; break;
		    }

		    make_readable_SENT(ref,hyp,eval,
				       MAX_SENTENCE_STRING_LEN,sent,pcs);
		    
		    printf("System: %s",sys_name(sys_i(sa_list,sys)));
		    printf("\n");
		    printf(name_fmt,"ref");
		    printf("%s\n",ref);
		    printf(name_fmt,"hyp");
		    printf("%s\n",hyp);
		    printf(name_fmt,"eval");
		    printf("%s\n",eval);
		    printf("\n");
		}
	    }
	    printf("\n\n");
	}
    }
}

/*************************************************************************/
/*   count the number of correct, sub, ins and del in the sentence.      */
void count_sent_score(SENT *sent, int *corr, int *sub, int *ins, int *del, int *spl, int *mrg)
{
    int wrd=0;
    short eval;

    *corr = *sub = *ins = *del = *spl = *mrg = 0;
    while ((eval=s_eval_wrd(sent,wrd)) != END_OF_WORDS){
        if (is_CORR(eval))
            (*corr)++;
        else if (is_INS(eval))
            (*ins)++;
        else if (is_DEL(eval))
            (*del)++;
        else if (is_SUB(eval))
            (*sub)++;
        else if (is_MERGE(eval))
            (*mrg)++;
        else if (is_SPLIT(eval))
            (*spl)++;
        else{
            fprintf(stderr,"Error: word eval '%d' is not CORR, DEL, INS, SUB, SPLIT of MERGE\n",
                           eval);
            exit(-1);
        }
	wrd++;
    }
}

/*************************************************************************/
/*   return true if there is any difference between the sentence         */
/*   alignments                                                          */
int a_diff_in_sent_exists(SYS_ALIGN_LIST *sa_list, int *spkr_ptr, int *sent_ptr)
{
    int sys,wrd=0;
    short eval;

    while ((eval=eval_wrd(sys_i(sa_list,0),spkr_ptr[0],sent_ptr[0],wrd)) !=
              END_OF_WORDS){
        for (sys=1; sys<num_sys(sa_list); sys++)
           if (eval_wrd(sys_i(sa_list,sys),spkr_ptr[sys],sent_ptr[sys],wrd)!=
               eval){
/*	       printf("diff at word %d: system %d: %d != %d\n",wrd,sys,
		      eval_wrd(sys_i(sa_list,sys),spkr_ptr[sys],sent_ptr[sys],wrd),
		      eval); 
	       printf("   utts %s and %s\n",sent_id(sys_i(sa_list,sys),spkr_ptr[sys],sent_ptr[0]),
		      sent_id(sys_i(sa_list,sys),spkr_ptr[sys],sent_ptr[sys]));*/
               return(TRUE);
	   }
        wrd++;
    }
    return(FALSE);
}
/*************************************************************************/
/*   return true if the scoring of the sentence is different             */
int a_diff_score_in_sent_exists(SYS_ALIGN_LIST *sa_list, int *spkr_ptr, int *sent_ptr)
{
    int sys,wrd=0,dbg=0;
    char *fname="a_diff_score_in_sent_exists";
    short eval;
    int corr=0, ins=0, del=0, sub=0;
    int ncorr=0, nins=0, ndel=0, nsub=0;

    for (wrd=0; ((eval=eval_wrd(sys_i(sa_list,0),spkr_ptr[0],sent_ptr[0],wrd)) !=
                  END_OF_WORDS); wrd++)
        if (is_CORR(eval))
            corr++;
        else if (is_INS(eval))
            ins++;
        else if (is_DEL(eval))
            del++;
        else if (is_SUB(eval))
            sub++;
        else{
            fprintf(stderr,"Error: in %s, word eval is not CORR, DEL, INS or SUBS\n",
                           fname);
            exit(-1);
        }
    if (dbg) printf("First sentence Stats\n");
    if (dbg) printf("    Corr: %2d, Ins: %2d, Del: %2d, Sub: %2d\n",corr,ins,del,sub);

    for (sys=1; sys<num_sys(sa_list); sys++){
        ncorr = nins = ndel = nsub=0;
        for (wrd=0; ((eval=eval_wrd(sys_i(sa_list,1),spkr_ptr[sys],sent_ptr[sys],wrd)) !=
                  END_OF_WORDS); wrd++){
            if (is_CORR(eval))
                ncorr++;
            else if (is_INS(eval))
                nins++;
            else if (is_DEL(eval))
                ndel++;
            else if (is_SUB(eval))
                nsub++;
            else{
                fprintf(stderr,"Error: in %s, word eval is not CORR, DEL, INS or SUBS\n",
                               fname);
                exit(-1);
            }
        }
        if (dbg) printf(" %1d  Corr: %2d, Ins: %2d, Del: %2d, Sub: %2d\n",sys,ncorr,nins,ndel,nsub);
        if ((ncorr != corr) || (nins != ins) ||(ndel != del) ||(nsub != sub)){
            if (dbg) printf("   Scores are different\n");
            return(TRUE);
        }
    }
    return(FALSE);
}

/*************************************************************************/
/*   return true if there are any errors in any of the sentences         */
/*   alignments                                                          */
int errors_exist(SYS_ALIGN_LIST *sa_list, int *spkr_ptr, int *sent_ptr)
{
    int sys,wrd=0;
    short eval;

    for (sys=0; sys<num_sys(sa_list); sys++){
        wrd=0;
        while ((eval=eval_wrd(sys_i(sa_list,sys),spkr_ptr[sys],
                              sent_ptr[sys],wrd)) != END_OF_WORDS){
            if (! is_CORR(eval))
               return(TRUE);
            wrd++;
        }
    }
    return(FALSE);
}

/**********************************************************************/
/*   Given a pointer to a sys_align_list, return the maximum number   */
/*   of speakers for all system                                       */
/**********************************************************************/
int max_spkrs_SYS_ALIGN_LIST(SYS_ALIGN_LIST *sa_list)
{
    int max_spkrs = 0, sys;

    for (sys=0;sys<num_sys(sa_list);sys++)
        if (num_spkr(sys_i(sa_list,sys)) > max_spkrs)
            max_spkrs = num_spkr(sys_i(sa_list,sys));

    return max_spkrs;
}

/**********************************************************************/
/*   Given a pointer to a sys_align_list, return the maximum number   */
/*   of sentences for all systems                                     */
/**********************************************************************/
int max_sents_SYS_ALIGN_LIST(SYS_ALIGN_LIST *sa_list)
{
    int max_st = 0, sys, spkr;

    for (sys=0;sys<num_sys(sa_list);sys++)
        for (spkr=0;spkr < num_spkr(sys_i(sa_list,sys)); spkr++)
            if (num_sents(sys_i(sa_list,sys),spkr) > max_st)
                max_st = num_sents(sys_i(sa_list,sys),spkr);
    return(max_st);
}

/******************************************************/
/*  return NULL if the id's are the same              */
/******************************************************/
int id_equal(char *hyp_id, char *ref_id, int results_type)
{
    int ind, len, skip_char;
    if (results_type != ATIS_RESULTS)
        return(strcasecmp(hyp_id,ref_id));
 
    if ((len = strlen(hyp_id)) != strlen(ref_id))
        return(-1);

    /* find the right paren */
    for (ind = 0; (ind < len) && (hyp_id[ind] != RIGHT_PAREN); ind++)
        ;

    if (hyp_id[ind] != RIGHT_PAREN)
        return(-1);
    skip_char = ind-1;

    /* now check all character except the one before the right paren */
    for (ind = 0; ind < len; ind++)
        if ((isupper(hyp_id[ind]) ? tolower(hyp_id[ind]) : hyp_id[ind])  !=
	    (isupper(ref_id[ind]) ? tolower(ref_id[ind]) : ref_id[ind]))
            if (ind != skip_char)
                return(-1);
    return(0);
}


/******************************************************/
/*  sort the Insertions and Deletions in the sentence */
/*  by the time in which they occure.                 */
/******************************************************/
#define SW(_a1,_a2,_t) {_t=_a1; _a1=_a2; _a2=_t;}
void sort_time_marked_SENT(SENT *sent)
{
    int x=0, tx, change;
    double tb, ta, db;
    PCIND_T id;
    short ev;
    
    /* Sort the insertions and deletions, so that later down the */
    /* road split and merges are properly done.                  */
    while (s_eval_wrd(sent,x) != END_OF_WORDS){   
	while ((s_eval_wrd(sent,x) != END_OF_WORDS) &&
	       (!(is_DEL(s_eval_wrd(sent,x)) || is_INS(s_eval_wrd(sent,x)))))
	    x++;
	/* printf("Located In/Del  %d\n",x); */
	/* word at x is either EOF or INS or del */
	change = TRUE;
	while ((s_eval_wrd(sent,x)!= END_OF_WORDS) && change){
	    change = FALSE;
	    tx = x+1;
	    while ((s_eval_wrd(sent,tx) != END_OF_WORDS) && 
		   (is_DEL(s_eval_wrd(sent,tx)) ||
		    is_INS(s_eval_wrd(sent,tx)))){
		ta = (is_DEL(s_eval_wrd(sent,tx-1))) ?
		    s_ref_beg_wrd(sent,tx-1) : s_hyp_beg_wrd(sent,tx-1);
		tb = (is_DEL(s_eval_wrd(sent,tx))) ?
		    s_ref_beg_wrd(sent,tx) : s_hyp_beg_wrd(sent,tx);
		/* printf("ta=%f tb=%f\n",ta,tb); */
		if (ta > tb) {
		    /* printf("Swapping \n"); */
		    /* I need to swap */
		    SW(s_eval_wrd(sent,tx),s_eval_wrd(sent,tx-1),ev);
		    SW(s_ref_wrd(sent,tx), s_ref_wrd(sent,tx-1), id);
		    SW(s_ref_dur_wrd(sent,tx),s_ref_dur_wrd(sent,tx-1),db);
		    SW(s_ref_beg_wrd(sent,tx),s_ref_beg_wrd(sent,tx-1),db);
		    SW(s_hyp_wrd(sent,tx),s_hyp_wrd(sent,tx-1),id);
		    SW(s_hyp_dur_wrd(sent,tx),s_hyp_dur_wrd(sent,tx-1),db);
		    SW(s_hyp_beg_wrd(sent,tx),s_hyp_beg_wrd(sent,tx-1),db);
		    change = TRUE;
		}
		tx++;
	    }
	}
	while ((s_eval_wrd(sent,x) != END_OF_WORDS) &&
	       (is_DEL(s_eval_wrd(sent,x)) ||
		is_INS(s_eval_wrd(sent,x))))
	    x++;
    }
}


