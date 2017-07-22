/**********************************************************************/
/*                                                                    */
/*             FILENAME:  load_hyps.c                                 */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*             DESC: This procedure reads in the file of hypothesis   */
/*                   sentences into the SYS_ALIGN structure.          */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvf.h>
#include <score/scorelib.h>

#include "alifunc.h"

static void new_load_word_indexes PROTO((char *buff, TABLE_OF_CODESETS *pcs, WCHAR_T *istr, int numi));
static void strip_utt_id PROTO((char *buff, char *id, int results_type));
static void load_sent_id PROTO((char *buff, char *id, int results_type, int speaker_id_present));
int spkr_num_from_id PROTO((SYS_ALIGN *align_str, char *id, int results_type));
static void alloc_SENT_for_SPKR PROTO((SPKR *spk, int n_snt));
static void make_id_list PROTO((SYS_ALIGN *align_str, char **id_list, int *num_unique_sent));
int perform_time_alignment PROTO((TABLE_OF_CODESETS *pcs, WTOKE_STR1 *ref, WTOKE_STR1 *hyp_1chan, SENT *sent, int dbg));
int remove_correct_out_of_chan PROTO((TABLE_OF_CODESETS *pcs, WTOKE_STR1 *ref, WTOKE_STR1 *ref_opp, WTOKE_STR1 *hyp, SENT *sent, int dbg, int ref_begin, int hyp_begin, int ref_opp_begin, int ref_opp_end));

#define ARRAY_EXPANSION_FACTOR	1.5
#define WITH_SPEAKER_ID		TRUE
#define WITHOUT_SPEAKER_ID	FALSE

#define LOAD_DBG                0

/*===================================================================*/
/*        The next three functions are to be for outside access      */


/*********************************************************************/
/*    This routine takes a ref and hyp file then loads them into     */
/*    the SYS_ALIGN structure.  Calling the alignment routine when   */
/*    the sentences have been loaded.                                */
/*********************************************************************/

#define LOAD_BUFF_LEN 2048

void load_and_align_hyp_to_ref_into_SYS_ALIGN(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *hyp_file, char *ref_file, int strip_silence)
{
    FILE *fp_hyp, *fp_ref;
    char hyp_buff[LOAD_BUFF_LEN], ref_buff[LOAD_BUFF_LEN];
    char tmp_hyp_utt_id[LOAD_BUFF_LEN], tmp_ref_utt_id[LOAD_BUFF_LEN];
    int dbg=FALSE, spkr;
    SENT *ts;
  
    if (LOAD_DBG) printf("Entering load_and_align_hyp_to_ref_into_SYS_ALIGN\n");

    if ((hyp_file == NULL) || (*hyp_file == NULL_CHAR) || 
        ((fp_hyp = fopen(hyp_file,"r")) == NULL)){
        fprintf(stderr,"Can't open input hypothesis file %s\n",hyp_file);
        exit(-1);
    }
    if ((ref_file == NULL) || (*ref_file == NULL_CHAR) || 
        ((fp_ref = fopen(ref_file,"r")) == NULL)){
        fprintf(stderr,"Can't open input reference file %s\n",ref_file);
        exit(-1);
    }
 
    while (!feof(fp_hyp) && !feof(fp_ref)){
        *hyp_buff  = *(hyp_buff+1) = *(ref_buff+1) = *ref_buff = COMMENT_CHAR;

        while (!feof(fp_hyp) && (is_comment(hyp_buff) ||
				 is_comment_info(hyp_buff) ||
				 is_empty(hyp_buff)))
	    if (safe_fgets(hyp_buff,LOAD_BUFF_LEN,fp_hyp) == NULL)
	       *hyp_buff = NULL_CHAR;
	while (!feof(fp_ref) && (is_comment(ref_buff) ||
				 is_comment_info(ref_buff) ||
				 is_empty(ref_buff)))
            if (safe_fgets(ref_buff,LOAD_BUFF_LEN,fp_ref) == NULL)
                *ref_buff = NULL_CHAR;

        if (feof(fp_hyp) && !feof(fp_ref)){
            fprintf(stderr,"Error: syncronization error, too many refs\n");
            exit(-1);
	}
        if (!feof(fp_hyp) && feof(fp_ref)){
            fprintf(stderr,"Error: syncronization error, too many hyps\n");
            exit(-1);
	}

        if (feof(fp_hyp) && feof(fp_ref))
            break;

	adjust_type_case(hyp_buff,pcs->cd[1].pc);
	adjust_type_case(ref_buff,pcs->cd[1].pc);

        /* SAVE the sentence id if one exists*/
        strip_utt_id(hyp_buff,tmp_hyp_utt_id,sys_type(align_str));
        strip_utt_id(ref_buff,tmp_ref_utt_id,sys_type(align_str));
        if (dbg) printf("Hyp id: %s\nRef id: %s\n",tmp_hyp_utt_id,
			tmp_ref_utt_id);
#ifdef out_dated
        /* for the first sentence, */
        /* check to see if the id is parsable by the spkr_num utilites */
        if (init_sent++ == 0)
            if (*tmp_hyp_utt_id != NULL_CHAR){
                char *tmp = tmp_hyp_utt_id;
                while ((*tmp != NULL_CHAR) && (*tmp != HYPHEN) &&
                       (*tmp != UND_SCR))
                    tmp++;
                if (*tmp != NULL_CHAR)
                    if ((tmp - tmp_hyp_utt_id) <= 6)
                        sys_type(align_str) = UNKNOWN_RESULTS_SPKRS;
	    }
#endif

        /* check to make sure the id's match */
        if (id_equal(tmp_hyp_utt_id,tmp_ref_utt_id,sys_type(align_str)) !=
                                                 0){
            fprintf(stderr,"Error: sentence id's are not the same: %s %s\n",
                          tmp_hyp_utt_id,tmp_ref_utt_id);
            exit(-1);
	}

        spkr = spkr_num_from_id(align_str,tmp_hyp_utt_id,sys_type(align_str));
        if (dbg) printf("  spkr %d\n",spkr);
        if (strcmp(tmp_hyp_utt_id,"") == 0) /* GIVE it a fake id */
            sprintf(tmp_hyp_utt_id,"%s-%d",spkr_name(align_str,spkr),
                                           num_sents(align_str,spkr)+1);

        alloc_SENT_for_SPKR(spkr_n(align_str,spkr),num_sents(align_str,spkr));
        /* alloc_and_init_SENT(ts,MAX_NUM_WORDS); */
        ts = sent_n(align_str,spkr,num_sents(align_str,spkr));
        num_sents(align_str,spkr)++;

        load_sent_id(s_id(ts),tmp_hyp_utt_id,sys_type(align_str),
		     TRUE);

        if (dbg) printf("loading sentence %s:\n",tmp_hyp_utt_id);
	if (strip_silence) remove_silence(hyp_buff,pcs);
	align_hypcstr_and_refcstr_into_SENT(ts,hyp_buff,ref_buff,pcs);
	
	spkr_nref(align_str,spkr) += s_nref(ts); 
	sys_nref(align_str) += s_nref(ts);
    }
    fclose(fp_hyp);
    fclose(fp_ref);
}

void load_and_align_timemarked_hyp_to_ref_into_SYS_ALIGN(SYS_ALIGN *align_str,
							 TABLE_OF_CODESETS *pcs,
							 char *hyp_file,
							 char *ref_file,
							 int strip_silence)
{
    FILE *fp_hyp, *fp_ref;
    char hyp_buff[LOAD_BUFF_LEN], ref_buff[LOAD_BUFF_LEN];
    char tmp_hyp_utt_id[LOAD_BUFF_LEN], tmp_ref_utt_id[LOAD_BUFF_LEN];
    int spkr, dbg=FALSE, voyer=FALSE;
    WTOKE_STR1 *ref_segs;
    WTOKE_STR1 *hyp_segs;
    SENT *ts;
    boolean end_of_ref_file, end_of_hyp_file;
    int err_ref, err_hyp;

    end_of_ref_file = end_of_hyp_file = F;

    alloc_singarr(ref_segs,1,WTOKE_STR1);
    alloc_singarr(hyp_segs,1,WTOKE_STR1);
    hyp_segs->s=1; hyp_segs->n=0; hyp_segs->id=(char *)0;
    ref_segs->s=1; ref_segs->n=0; ref_segs->id=(char *)0;

    if (LOAD_DBG) printf("load_and_align_timemarked_hyp_to_ref_into_SYS_ALIGN\n");

    if ((hyp_file == NULL) || (*hyp_file == NULL_CHAR) || 
        ((fp_hyp = fopen(hyp_file,"r")) == NULL)){
        fprintf(stderr,"Can't open input hypothesis file %s\n",hyp_file);
        exit(-1);
    }
    if ((ref_file == NULL) || (*ref_file == NULL_CHAR) || 
        ((fp_ref = fopen(ref_file,"r")) == NULL)){
        fprintf(stderr,"Can't open input reference file %s\n",ref_file);
        exit(-1);
    }
 
    while (!feof(fp_hyp) && !feof(fp_ref)){
        *hyp_buff  = *(hyp_buff+1) = *(ref_buff+1) = *ref_buff = COMMENT_CHAR;
	if (dbg) printf("Beginning an utt\n");

	get_next_mark_utt(fp_ref,ref_segs,ref_file,&end_of_ref_file,&err_ref);
	get_next_mark_utt(fp_hyp,hyp_segs,hyp_file,&end_of_hyp_file,&err_hyp);
	if (err_ref != err_hyp || err_ref != 0) {
	    fprintf(stderr,"  *ERR: get_next_mark_utt on file %s returns err=%d\n",
		    ref_file,err_ref);
	    fprintf(stderr,"  *ERR: get_next_mark_utt on file %s returns err=%d\n",
		    hyp_file,err_hyp);
	    exit(-1);
	}

        if (feof(fp_hyp) && !feof(fp_ref)){
            fprintf(stderr,"Error: syncronization error, too many refs\n");
            exit(-1);
	}
        if (!feof(fp_hyp) && feof(fp_ref)){
            fprintf(stderr,"Error: syncronization error, too many hyps\n");
            exit(-1);
	}

        if (feof(fp_hyp) && feof(fp_ref))
            break;

	strcpy(tmp_ref_utt_id,ref_segs->id);
	strcpy(tmp_hyp_utt_id,hyp_segs->id);

        /* check to make sure the id's match */
        if (id_equal(tmp_hyp_utt_id,tmp_ref_utt_id,sys_type(align_str)) !=
                                                 0){
            fprintf(stderr,"Error: sentence id's are not the same: %s %s\n",
                          tmp_hyp_utt_id,tmp_ref_utt_id);
            exit(-1);
	}

        spkr = spkr_num_from_id(align_str,tmp_hyp_utt_id,sys_type(align_str));
        if (dbg) printf("  spkr %d  %s - %d\n",spkr,spkr_name(align_str,spkr),
			sys_type(align_str));
        if (strcmp(tmp_hyp_utt_id,"") == 0) /* GIVE it a fake id */
            sprintf(tmp_hyp_utt_id,"%s-%d",spkr_name(align_str,spkr),
                                           num_sents(align_str,spkr)+1);

	if (voyer) printf("Time Aligning: spkr:%s   id:%s",
			  spkr_name(align_str,spkr),tmp_ref_utt_id);
	if (voyer) printf("     %d hyp_segs   %d ref_segs\n",
			  hyp_segs->n, ref_segs->n);

        alloc_SENT_for_SPKR(spkr_n(align_str,spkr),num_sents(align_str,spkr));
        /* alloc_and_init_SENT(ts,MAX_NUM_WORDS); */
        ts = sent_n(align_str,spkr,num_sents(align_str,spkr));
        num_sents(align_str,spkr)++;

        load_sent_id(s_id(ts),tmp_hyp_utt_id,sys_type(align_str),
		     TRUE);

        if (dbg) printf("loading sentence %s:\n",tmp_hyp_utt_id);
	/* if (strip_silence) remove_silence(hyp_buff,pcs); */
	align_WTOKE_into_SENT(ts, ref_segs, hyp_segs, pcs);

	sort_time_marked_SENT(ts);

	spkr_nref(align_str,spkr) += s_nref(ts); 
	sys_nref(align_str) += s_nref(ts);

	free_mark_file(ref_segs);
	free_mark_file(hyp_segs);
    }

    /* Tell the function to clean itself up */
    align_WTOKE_into_SENT((SENT *)0,(WTOKE_STR1 *)0,(WTOKE_STR1 *)0,pcs);

    free_singarr(ref_segs,WTOKE_STR1);
    free_singarr(hyp_segs,WTOKE_STR1);

    fclose(fp_hyp);
    fclose(fp_ref);
}


/**********************************************************************/
/*   load the file of hypothesis sentences into the SYS_ALIGN struct  */
/*     fix: made n_snt a pointer to an integer, date: 911206          */
/*     Updated: 940210 to use the adjust_type_case                    */
/**********************************************************************/
void load_hyp_into_SYS_ALIGN(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *filename, int strip_silence)
{
    FILE *fp;
    char buff[LOAD_BUFF_LEN],id[LOAD_BUFF_LEN];
    int spkr;
    register int *n_snt;
    register SENT *ts;

    if (LOAD_DBG) printf("Entering load_hyp_into_SYS_ALIGN\n");
    printf("Loading hypothesis strings\n");

    if ((filename == NULL) || ((fp=fopen(filename,"r")) == NULL)){
	fprintf(stderr,"No hypothesis file to load > %s\n",filename);
	exit(-1);
    }


    /* get a sentence */
    while (safe_fgets(buff,LOAD_BUFF_LEN,fp) != NULL){
      if (!is_comment(buff ) && !is_empty(buff)){
        /* extract  the sentence id, ie. speaker, sent type, and number */
        if (LOAD_DBG) printf("-------------- new sentence ---------------\n");

	adjust_type_case(buff,pcs->cd[1].pc);

        strip_utt_id(buff,id,sys_type(align_str));
        spkr = spkr_num_from_id(align_str,id,sys_type(align_str));

        if (LOAD_DBG) printf("load_hyp_into_SYS_ALIGN:  buff %s\n",buff);
        if (LOAD_DBG) printf("load_hyp_into_SYS_ALIGN:  id   %s\n",id);
        if (LOAD_DBG) printf("load_hyp_into_SYS_ALIGN:  spkr %d\n",spkr);
        /* since the sentences may not be arranged by speaker set a pointer */
        /* to the number of sentences for the current speaker               */
        n_snt = &(num_sents(align_str,spkr));
        alloc_SENT_for_SPKR(spkr_n(align_str,spkr),*n_snt);

        /* to avoid many internal C pointer resolution, point to the */
        /* sentence in the SYS_ALIGN structure  (to make it faster) */
        ts = sent_n(align_str,spkr,*n_snt);

        load_sent_id(s_id(ts),id,sys_type(align_str),WITH_SPEAKER_ID);
	
	if (strip_silence) remove_silence(buff, pcs);
	new_load_word_indexes(buff,pcs,s_hyp(ts),MAX_NUM_WORDS);

        (*n_snt)++;
      }
    }
    fclose(fp);
}
/*                 END OF EXTERNAL ACCESS FUNCTIONS                  */
/*===================================================================*/


static void new_load_word_indexes(char *buff, TABLE_OF_CODESETS *pcs, WCHAR_T *istr, int numi)
{
    int i, perr;
    WCHAR_T tint_str[MAX_SYMBS_IN_STR + 1];

    /* use the encode2 function to get the word indexes */
    encode2(buff,pcs->cd[1].pc,tint_str,tint_str,&perr);
    if (perr != 0){
	fprintf(stderr,"Error: encode5 returned %d, not 0\n",perr);
	exit(-1);
    }

    if (tint_str[0] > numi){
	fprintf(stderr,"Error: too many words loaded %d > %d from string %s\n",
		tint_str[0],numi,buff);
	fprintf(stderr,"       Change MAX_NUM_WORDS in 'include/score/sysalign.h'\n");
	exit(-1);
    }
    /* copy the int string values to the sentence pointer */
    for (i=1; i <= tint_str[0]; i++)
	istr[i-1] = tint_str[i];
    istr[i-1] = END_OF_WORDS;
    
}

/**********************************************************************/
/*  strip off the sentence identifier from the sentence and put it    */
/*  into the char string id                                           */
/*  modified: Feb, 3, 1992                                            */
/*     added new results type: WSJ_RESULTS                            */
/*  modified: Nov 8, 1994                                             */
/*     added new results type: SWB_RESULTS. The Speaker id is the     */
/*     first 2 fields separated be underbars.                         */
/**********************************************************************/
static void strip_utt_id(char *buff, char *id, int results_type)
{
    char *ptr, *err_msg="Error: utterance id not in a valid form";

    /* if there is no ID, do Nothing */
    if (results_type == UNKNOWN_RESULTS_NO_ID){
	*id = '\0';
	return;
    }

    ptr = buff+strlen(buff);
    if (LOAD_DBG) printf("strip_utt_id: buff: %s",buff);
    search_back_for_char(buff,&ptr,LEFT_PAREN);
    if (LOAD_DBG) printf("strip_utt_id: ptr:  %s",ptr);
    strcpy_to_char(id,ptr,RIGHT_PAREN);
    if (LOAD_DBG) printf("strip_utt_id: id:   %s\n",id);
    *ptr = NULL_CHAR;
    if (LOAD_DBG) printf("strip_utt_id: sent: %s\n",buff);

    /* now do a quick verification of the stored id */
    if ((results_type == RM_RESULTS) || (results_type == ATIS_RESULTS) ||
        (results_type == TIMIT_RESULTS) || (results_type == WSJ_RESULTS) ||
        (results_type == SWB_RESULTS) || 
        (results_type == UNKNOWN_RESULTS_SPKRS) || 
        (results_type == UNKNOWN_RESULTS_NO_SPKR) || 
        (results_type == UNKNOWN_RESULTS_SPKRS_NONUNIQUE_ID) || 
        (results_type == UNKNOWN_RESULTS_SPKRS_UNIQUE_ID)){
        if (strlen(id) == 0) {
            fprintf(stderr,"%s\n       no id present.\n",err_msg);
            fprintf(stderr,"       %s\n",buff);
            exit(-1);
	}
        if (*id != LEFT_PAREN){
            fprintf(stderr,"%s\n       missing left paren. %s\n",err_msg,id);
            fprintf(stderr,"       %s\n",buff);
            exit(-1);
	}
        if (id[strlen(id)-1] != RIGHT_PAREN){
            fprintf(stderr,"%s\n       missing right paren. %s\n",err_msg,id);
            fprintf(stderr,"       %s\n",buff);
            exit(-1);
	}
	if (results_type == SWB_RESULTS) {
            if (strlen(id) != 13){
		fprintf(stderr,"%s\n       invalid SWB id length. %s\n",
			err_msg,id);
		fprintf(stderr,"       %s\n",buff);
                exit(-1);
	    }
	    
        } else if ((results_type == ATIS_RESULTS)||
		   (results_type ==WSJ_RESULTS)){
            char *db_name;
            if ((results_type == ATIS_RESULTS))
                db_name = "ATIS" ;
            else
                db_name = "WSJ" ;
            if (strlen(id) != 10){
		fprintf(stderr,"%s\n       invalid %s id length. %s\n",
			err_msg,db_name,id);
		fprintf(stderr,"       %s\n",buff);
                exit(-1);
	    }
	    if (results_type == ATIS_RESULTS) {
		/* standardize the last character of the uttid */
		if (*(id+8) == 's') *(id+8) = 'x';
		if (*(id+8) == 'S') *(id+8) = 'X';
	    }
	}
    }
}

/**********************************************************************/
/*   copy into the buff the sentence id dependent on the results type */
/*   date : july 1990                                                 */
/*  modified: Feb, 3, 1992                                            */
/*     added new results type: WSJ_RESULTS                            */
/*  modified: Nov 8, 1994                                             */
/*     added new results type: SWB_RESULTS. The Speaker id is the     */
/*     first 2 fields separated be underbars.                         */
/**********************************************************************/
static void load_sent_id(char *buff, char *id, int results_type, int speaker_id_present)
{
    char *ptr;

    if ((results_type == RM_RESULTS) ||
	(results_type == UNKNOWN_RESULTS_SPKRS_NONUNIQUE_ID) ||
        (results_type == UNKNOWN_RESULTS_SPKRS)){
	ptr=id;
        while ((*ptr != NULL_CHAR) && (*ptr != HYPHEN) && 
               (*ptr != UND_SCR))
            ptr++;
	if ((*ptr == UND_SCR) || (*ptr == HYPHEN))
	    ptr++;
        if ((*ptr == NULL_CHAR) && (speaker_id_present == WITHOUT_SPEAKER_ID)){
            /* then this must just be an id, use it */
	    ptr = id;
	    if (*ptr == LEFT_PAREN)
		ptr++;
	} else if (*ptr == NULL_CHAR){
            fprintf(stderr,"Error: Unparsable RM utterance id %s\n",id);
            exit(-1);
	}
        strncpy_to_before_char(buff,ptr,RIGHT_PAREN,SENT_ID_LENGTH);
    }
    else if (results_type == SWB_RESULTS) {
        ptr = id;
        while ((*ptr != NULL_CHAR) && (*ptr != HYPHEN) && (*ptr != UND_SCR))
            ptr++;
        if (*ptr == NULL_CHAR){
            fprintf(stderr,"Error: Unparsable SWB utterance id %s\n",id);
            exit(-1);
	}
	ptr++;
        while ((*ptr != NULL_CHAR) && (*ptr != HYPHEN) && (*ptr != UND_SCR))
            ptr++;
        if (*ptr == NULL_CHAR){
            fprintf(stderr,"Error: Unparsable SWB utterance id %s\n",id);
            exit(-1);
	}
        strncpy_to_before_char(buff,id+1,RIGHT_PAREN,SENT_ID_LENGTH);
	
    } else if ((results_type == ATIS_RESULTS) ||
	     (results_type == TIMIT_RESULTS) ||
	     (results_type == UNKNOWN_RESULTS_SPKRS_UNIQUE_ID) ||
	     (results_type == UNKNOWN_RESULTS_NO_SPKR) ||
             (results_type == WSJ_RESULTS)){
        if (strlen(id) >= SENT_ID_LENGTH-2){
            strncpy(buff,id,SENT_ID_LENGTH-1);
            buff[SENT_ID_LENGTH-1] = NULL_CHAR;
	}
        else
            strcpy(buff,id);
	if ((results_type == ATIS_RESULTS) && strlen(id) == 10){
	    /* standardize the last character of the uttid */
	    if (*(id+8) == 's') *(id+8) = 'x';
	    if (*(id+8) == 'S') *(id+8) = 'X';
	}
    } else 
	*buff = '\0';
}
        
/**********************************************************************/
/*  return the speaker number from the sentence identifier.   If the  */
/*  speaker is a new speaker, alloc a SPKR structure for the speaker  */
/*  and insert the name into the SYS_ALIGN                            */
/*  speaker list and initialize that speakers sentence information    */
/*  if results_type is ATIS_RESULTS, get the speaker number for an    */
/*  ATIS utt                                                          */
/*   modified July 12 , 1990                                          */
/*  1. if there is no more room is the speaker list, then a new larger*/
/*     list is created, it's info copied into the new list, then the  */
/*     old list is freed.                                             */
/*     date : july 1990                                               */
/*  2. ATIS id's now need to include the speaking mode field to       */
/*     differentiate speakers.                                        */
/*     date : January 14, 1992                                        */
/*  modified: Feb, 3, 1992                                            */
/*     added new results type: WSJ_RESULTS, the speaker code is the   */
/*     first three characters in the utterance id.                    */
/*  modified: Nov 8, 1994                                             */
/*     added new results type: SWB_RESULTS. The Speaker id is the     */
/*     first 2 fields separated be underbars.                         */
/**********************************************************************/
int spkr_num_from_id(SYS_ALIGN *align_str, char *id, int results_type)
{
    char s_str[LOAD_BUFF_LEN], *ptr;
    int i, dbg=FALSE;
  
    if (dbg) printf("spkr_num_from_id: id = %s  type %d\n",
		    id,results_type);
    if ((results_type == RM_RESULTS) ||
        (results_type == TIMIT_RESULTS) ||
        (results_type == UNKNOWN_RESULTS_SPKRS_NONUNIQUE_ID) || 
        (results_type == UNKNOWN_RESULTS_SPKRS_UNIQUE_ID) ||
        (results_type == UNKNOWN_RESULTS_SPKRS)){
        /* copy the spkr name into s_str */
        ptr = s_str;
        id++;  /* skip the "(" in the id */
        while ((*id != NULL_CHAR) && (*id != HYPHEN) && (*id != UND_SCR)){
            *ptr++ = *id++;
            *ptr = NULL_CHAR;
	}
        if (*id == NULL_CHAR){
            fprintf(stderr,"Error: Could not extract a proper speaker id");
            fprintf(stderr,"       from the form (SPKR_ID) %s\n",id);
            exit(-1);
	}
    }
    else if (results_type == SWB_RESULTS) {
        /* copy the spkr name into s_str */
        ptr = id;
        ptr++;  /* skip the "(" in the id */
        while ((*ptr != NULL_CHAR) && (*ptr != HYPHEN) && (*ptr != UND_SCR))
	    ptr++;
	if (dbg) printf("    First underbar: '%s'\n",ptr);
	if (*ptr == NULL_CHAR) {
            fprintf(stderr,"Error: Could not extract a proper SWB speaker id");
            fprintf(stderr,"       from the form (SPKR_CHAN_ID) %s\n",id);
            exit(-1);
	}
	ptr++;
        while ((*ptr != NULL_CHAR) && (*ptr != HYPHEN) && (*ptr != UND_SCR))
	    ptr++;
	if (dbg) printf("    second underbar: '%s'\n",ptr);
	if (*ptr == NULL_CHAR) {
            fprintf(stderr,"Error: Could not extract a proper Channel id");
            fprintf(stderr,"       from the form (SPKR_CHAN_ID) %s\n",id);
            exit(-1);
	}
	ptr --;
	strncpy(s_str,id+1,ptr - id);
	s_str[ptr-id] = NULL_CHAR;
	if (dbg) printf("    Final speaker: '%s'\n",s_str);
    }
    else if (results_type == ATIS_RESULTS){
        /* copy the spkr name into s_str */
        ptr = s_str;
        id++;  /* skip the "(" in the id */
        /* load in the 3 char speaker id */
        *ptr++ = *id++;
        *ptr++ = *id++;
        *ptr++ = *id++;
        *ptr = NULL_CHAR;
        if (dbg) printf("     spkr id: = %s\n",s_str);
    }
    else if (results_type == WSJ_RESULTS){
        /* copy the spkr name into s_str */
        ptr = s_str;
        id++;  /* skip the "(" in the id */
        /* load in the 2 char speaker id */
        *ptr++ = *id++;
        *ptr++ = *id++;
        *ptr++ = *id++;
        *ptr = NULL_CHAR;
        if (dbg) printf("     spkr id: = %s\n",s_str);
    } else if ((results_type == UNKNOWN_RESULTS_NO_SPKR) ||
	       (results_type == UNKNOWN_RESULTS_NO_ID))
        strcpy(s_str,"NONE");

    for (i=0;i<num_spkr(align_str);i++){
        if (dbg) printf("     comparing id to speaker = %d",i);
        if (dbg) printf(" of %d speakers\n",num_spkr(align_str));
        if (strncmp(s_str,spkr_name(align_str,i),MAX_SPEAKER_NAME_LEN) == 0)
            return(i);
    }
    if (num_spkr(align_str) == max_spkr(align_str)){
        SPKR **old_spkr_list;
        int old_spkr_size;

        old_spkr_list = spkr_list(align_str);
        old_spkr_size = max_spkr(align_str);
        max_spkr(align_str) *= ARRAY_EXPANSION_FACTOR;
#ifdef ANNOYING_MESSAGES
        fprintf(stderr,"Warning: Expanding number of speakers from %d to %d\n",
                       old_spkr_size, max_spkr(align_str));
        fflush(stderr);
#endif
        alloc_singarr(spkr_list(align_str),max_spkr(align_str),SPKR *);
        memcpy(spkr_list(align_str),old_spkr_list,
	       sizeof(SPKR *)*(num_spkr(align_str)));
        free_singarr(old_spkr_list, SPKR *);
    }

    alloc_and_init_SPKR(spkr_n(align_str,i));
    strncpy(spkr_name(align_str,i),s_str,MAX_SPEAKER_NAME_LEN);
    return(num_spkr(align_str)++);
}

/************************************************************************/
/*  this procedure allocates the next available sentence for the        */
/*  next speaker.  if thereis now more available space, a new sentence  */
/*  list is allocated to accomodate the larger number of sentences      */
/*  date: july 1990                                                     */
/************************************************************************/
static void alloc_SENT_for_SPKR(SPKR *spk, int n_snt)
{
    /* check to see if there is enough space to alloc another sentence */
    if (n_snt == sp_max_num_st(spk)){
        SENT **old_sent_list;
        int old_size;

        old_sent_list = sp_st_l(spk);
        old_size = sp_max_num_st(spk);
        sp_max_num_st(spk) *= ARRAY_EXPANSION_FACTOR;

#ifdef ANNOYING_MESSAGES
        fprintf(stderr,"Warning: Expanding number of sentences for ");
        fprintf(stderr,"speaker %s from %d to %d\n",sp_name(spk),
		old_size, sp_max_num_st(spk));
        fflush(stderr);
#endif
        alloc_singarr(sp_st_l(spk),sp_max_num_st(spk),SENT *);
        memcpy(sp_st_l(spk),old_sent_list,sizeof(SENT *)*(sp_num_st(spk)));
        free_singarr(old_sent_list, SENT *);
    }
    /* now alloc the sentence */
    alloc_and_init_SENT(sp_st(spk,n_snt),MAX_NUM_WORDS);
}

/************************************************************************/
/************************************************************************/
/*
       The rest of this file is dedicated to procedures to load in
       reference sentences based on the sentences needed for the
       SYS_ALIGN structure.
*/
#define LARGE_BUFFER	LOAD_BUFF_LEN

/* this set of structures are used to load in any number of needed   */
/* reference sentences                                            */
typedef struct reference1_struct{
    char *id_str;         
    char *sent_string;
} REF1_SENT;

typedef struct ref1_struct{    int num;                    /* number of reference sentences  */
    REF1_SENT **sents;        /* reference sentence list        */
} REF1;

#define alloc_REF1_SENT(_ots) \
  { alloc_singarr(_ots,1,REF1_SENT); \
    alloc_singarr(_ots->id_str,SENT_ID_LENGTH,char); \
    alloc_singarr(_ots->sent_string,LARGE_BUFFER,char); \
  }
    
#define free_REF1_SENT(_ots) \
  { free_singarr(_ots->sent_string, char); \
    free_singarr(_ots->id_str, char); \
    free_singarr(_ots, REF1_SENT); \
  }

static void do_alignments PROTO((SYS_ALIGN *align_str, REF1 *ref, TABLE_OF_CODESETS *pcs, char **id_list));
static void load_ref PROTO((char **id_list, int num_unique_sent, char *ref_file, TABLE_OF_CODESETS *pcs, int stype, REF1 *ref));

/************************************************************************/
/*   This routine will take an align structure with hypothesis sentences*/
/*   pre loaded then raed in the necessary reference transcriptions     */
/*   then load the proper sentences in the into the SYS_ALIGN struct    */
/*   date : july 1990                                                   */
/************************************************************************/
void load_and_align_ref_from_SYS_ALIGN(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *ref_file)
{
    int poss_sent_count=0;                /* the total num of sents in */
                                          /* the current SYS_ALIGN str */
    int num_unique_sent=0;
    char **id_list;                       /* the unique list of sentences */
    int spkr, i, dbg=FALSE;
    REF1 ref_str, *ref = &ref_str;

    if (LOAD_DBG) {
        printf("Entering load_and_align_ref_from_SYS_ALIGN\n");
        dbg=TRUE;
    }
    fprintf(stderr,"Loading the reference sentences \n");

    /* first count the total number of possibly unique sentences */
    for (spkr=0;spkr < num_spkr(align_str); spkr++)
        poss_sent_count += num_sents(align_str,spkr);
    
    /* allocate room for the list of sentences */
    alloc_2dimarr(id_list,poss_sent_count,SENT_ID_LENGTH,char);

    make_id_list(align_str,id_list,&num_unique_sent);
    sort_strings_in_place(id_list,num_unique_sent,INCREASING);

    if (dbg){
        printf("The developed  ID list\n");
        for (i=0; i<num_unique_sent; i++) 
            printf("%d: %s\n",i,id_list[i]);
    }

    /* load in the necessarry reference sentences */
    load_ref(id_list,num_unique_sent,ref_file,pcs,sys_type(align_str),ref);

    fprintf(stderr,"Aligning the sentences \n");

    /* align_the hyps to the refs */
    do_alignments(align_str,ref,pcs,id_list);
    
    free_2dimarr(id_list,num_unique_sent,char);
    /* now free up all the memory used to read in the reference */
    /* sentences */
    {
        int ref_ind;
        for (ref_ind=0;ref_ind < ref->num+1; ref_ind++){
	    free_singarr(ref->sents[ref_ind]->id_str,char);
	    free_singarr(ref->sents[ref_ind]->sent_string,char);
	    free_singarr(ref->sents[ref_ind],REF1_SENT);
	}
	free_singarr(ref->sents, REF1_SENT *);
    }
}

static void do_alignments(SYS_ALIGN *align_str, REF1 *ref, TABLE_OF_CODESETS *pcs, char **id_list)
{
    /* now copy all the loaded sentences into the proper places in the */
    /* alignment structure */
    int spkr, snt, ref_ind, i, failure=FALSE;
    
    /* first, load all the id's into a searchable array */
    for (i=0; i < ref->num; i++)
	strcpy(id_list[i],ref->sents[i]->id_str);

    for (spkr=0;spkr < num_spkr(align_str); spkr++){
	fprintf(stdout,"    Aligning sentences for speaker %d, id %s\n",
		spkr,spkr_name(align_str,spkr));
	for (snt=0; snt < num_sents(align_str,spkr); snt++){
	    /* find the reference sentence to load */
	    /* if (snt % 8 == 0)
		fprintf(stdout,"        "); */
	    /* fprintf(stdout,"%s, ",sent_id(align_str,spkr,snt)); fflush(stdout); */
	    ref_ind = strings_search(id_list,ref->num,
				     sent_id(align_str,spkr,snt));
	    if (ref_ind == (-1)){
		fprintf(stderr,"Error: can't find a reference");
		fprintf(stderr," sentence for id %s\n",
			sent_id(align_str,spkr,snt));
		fflush(stderr);
		failure = TRUE;
	    }
	    else {
		adjust_type_case(ref->sents[ref_ind]->sent_string,pcs->cd[1].pc);
		if (align_hypntistr_and_refcstr_into_SENT(
			sent_n(align_str,spkr,snt),
		        hyp_data(align_str,spkr,snt),
			ref->sents[ref_ind]->sent_string,pcs) == 0){
		    spkr_nref(align_str,spkr)+= sent_nref(align_str,spkr,snt); 
		    sys_nref(align_str) += sent_nref(align_str,spkr,snt);
		} else {
		    fprintf(stderr,"Error Occured on Utterance %s\n",sent_id(align_str,spkr,snt));
		    fprintf(stderr,"Sentence Dump\n");
		    dump_SENT_wrds(pcs,sent_n(align_str,spkr,snt));
		    failure = TRUE;
		}
	    }
	    /* if ((snt+1) % 8 == 0)
		fprintf(stdout,"\n"), fflush(stdout); */
	}
	/* if ((snt) % 8 != 0)
	    fprintf(stdout,"\n"), fflush(stdout); */
    }
    /* Clean up the temporary structures */
    align_hypntistr_and_refcstr_into_SENT((SENT *)0,(WCHAR_T *)0,(char *)0,
					  (TABLE_OF_CODESETS *)0);
    if (failure)
	exit(1);
}


/* building the id_list */
static void make_id_list(SYS_ALIGN *align_str, char **id_list, int *num_unique_sent)
{
    int dup_found, spkr, snt, i, dbg=0;

    for (spkr=0;spkr < num_spkr(align_str); spkr++)
        for (snt=0; snt < num_sents(align_str,spkr); snt++){
            if (dbg) printf("make_id_list is %s unique\n",
                            sent_id(align_str,spkr,snt));
            /* search all previous ids to make sure it is unique */
            dup_found = FALSE;
            for (i=0; (i<(*num_unique_sent)) && !dup_found; i++){
                if (strcmp(id_list[i],sent_id(align_str,spkr,snt)) == 0)
                    dup_found = TRUE;
	    }
            if (!dup_found)
               strcpy(id_list[(*num_unique_sent)++],
                      sent_id(align_str,spkr,snt));
	}
    if (dbg){
        printf("unique sentence id's\n");
        for (i=0; i<(*num_unique_sent); i++)
            printf("id %s\n",id_list[i]);
    }
}


 
/*************************************************************************/
/*   general alogorithm:
        1: given a list of sentence ids, read in each line from the ref
           file.
           a. if the sentence is needed, AND the id has not already been 
	      loaded, store it in the structure
              else continue until the whole file is read
        2: go through the SYS_ALIGN structure and load the sentences into
           the proper reference index lists.
        3: free the REF1 structure
     Fixes:
        Jan 1992, Changed load procedure to print missing ortho utt's
	Feb 1994, Changed to just load the sentences into the ref struct
*/
static void load_ref(char **id_list, int num_unique_sent, char *ref_file, TABLE_OF_CODESETS *pcs, int stype, REF1 *ref)
{
    int dbg=LOAD_DBG, srch, exit_flag=FALSE;

    ref->num = 0;
    if (dbg) printf("load_ref, alloc'd space for %d sents\n",
		    num_unique_sent+1);
    alloc_singarr(ref->sents,num_unique_sent+1,REF1_SENT *);

    /* load all of the needed reference sentences */    
    {
        FILE *fp;
        REF1_SENT *ts; 
        char tmp_utt_buff[LOAD_BUFF_LEN];

        if ((ref_file == NULL) || ((fp = fopen(ref_file,"r")) == NULL)){
            fprintf(stderr,"Error: Could not open reference transcrip");
            fprintf(stderr,"tion file %s\n",ref_file);
            exit(-1);
        }

	if (dbg) printf("allocating first sentence\n");
        alloc_REF1_SENT(ref->sents[ref->num]);
        ts = ref->sents[ref->num];
        ref->num++;
        while ((safe_fgets(ts->sent_string,LARGE_BUFFER,fp) != NULL) &&
               (ref->num-1 != num_unique_sent))

            if (!is_comment(ts->sent_string) &&
                !is_comment_info(ts->sent_string) &&
		!is_empty(ts->sent_string)){
		
		/* make sure the reference string is in the right case */
		adjust_type_case(ts->sent_string,pcs->cd[1].pc);

               /* remove the id from the sentence without the id , point */
               /* the tmp_utt_buff to the sentence id */
               strip_utt_id(ts->sent_string,tmp_utt_buff,stype);
               
               load_sent_id(ts->id_str,tmp_utt_buff,stype,WITHOUT_SPEAKER_ID);
               if (dbg) printf(" id %s - ",ts->id_str);

               /* see if this one string is needed */
               if ((srch=strings_search(id_list,num_unique_sent,ts->id_str))
                                    >= 0){
		   int i, id_loaded_n_times = 0;
		   for (i=0; i < ref->num-1; i++)
		       if (strcmp(ref->sents[i]->id_str,ts->id_str) == 0)
			   id_loaded_n_times++;
                   if (id_loaded_n_times > 0){
		       if (dbg) printf(" needed, BUT too many occurences\n");
                       fprintf(stderr,"Error: There are too many occurances of id %s, > 1\n",ts->id_str);
		       exit_flag=TRUE;
		   } else {
		       if (dbg) printf(" need it , creating next sentence %d",ref->num);
		       alloc_REF1_SENT(ref->sents[ref->num]);
		       ts = ref->sents[ref->num];
		       ref->num++;
		       if (dbg) printf("\n");
		   }
               }
               else if (dbg) printf("\n");
	    }
        ref->num--;
        fclose(fp);

        /* make sure there is the correct number of ref sentences loaded */
        if (ref->num != num_unique_sent){
            fprintf(stderr,"Error: Not enough reference sentences loaded,");
            fprintf(stderr," %d should be %d\n",ref->num,num_unique_sent);
            exit_flag=TRUE;
	}

	/* sort what was just loaded */
	{
	    char **idtab;
	    int *indtab, i;
	    REF1_SENT **reftab;
	    alloc_singarr(idtab,ref->num,char *);
	    alloc_singarr(indtab,ref->num,int);
	    alloc_singarr(reftab,ref->num,REF1_SENT *);
	    /* first, load all the id's into a searchable array */
	    for (i=0; i < ref->num; i++){
		idtab[i] = ref->sents[i]->id_str;
		reftab[i] = ref->sents[i];
	    }

	    sort_strings_using_index(idtab,indtab,ref->num,INCREASING);
	    /* change the order of the reference structure */
	    for (i=0; i < ref->num; i++)
		ref->sents[i] = reftab[indtab[i]];
	    free_singarr(reftab, REF1_SENT *);
	    free_singarr(indtab, int);
	    free_singarr(idtab, char *);
	}	    
    }
    if (dbg)
       { int i;
         printf("Printing the loaded reference sentences(%d).\n",ref->num);
         for (i=0; i < ref->num; i++)
             printf("ORT id %s-\n      %s\n",ref->sents[i]->id_str,
                                         ref->sents[i]->sent_string);
       }
}

int is_word_match_in_WTOKE PROTO((WTOKE_STR1 *ref, int begin, int end, char * str,double t1,double dur));

int is_word_match_in_WTOKE(WTOKE_STR1 *ref, int begin, int end, char * str,double t1,double dur)
{
    char *proc="is_word_match_in_WTOKE";
    double mid=t1+(dur/2.0), end_srch = t1+dur;
    int w, mid_ref = (begin+end)/2;
    int dbg = 0;
    if (dbg) {
	printf("%s: Searching for word match, from %d to %d, string %s\n",
	       proc,begin,end,str);
	printf("%s: Time %.2f dur %.2f\n",proc,t1,dur);
    }
    /* a quick speed up, first check to see if the times are in the */
    /* second half of the ref WTOKE.  If so, don't search the beginning */
    w = begin;
    if (t1 > (ref->word[mid_ref].t1+ref->word[mid_ref].dur))
	w = mid_ref;
    for (; w<end; w++){
/*	if (!(mid>ref->word[w].t1 && mid<(ref->word[w].t1+ref->word[w].dur)))
	    continue; */
	/* STOP SEARCHING if the end of the target is less than the ref beginning */
	if (end_srch < ref->word[w].t1)
	    return(-1);
	if (!(mid>ref->word[w].t1 && mid<(ref->word[w].t1+ref->word[w].dur)))
	    continue;


	if (dbg) printf("%s: Passes midpoint test\n",proc);

	if (strcasecmp(str,ref->word[w].sp) != 0)
	    continue;

	if (ref->word[w].overlapped)
	    continue;

	if (dbg) printf("%s: Passes mulitple use test\n",proc);

	ref->word[w].overlapped = TRUE;

	if (dbg) printf("I FOUND ONE\n");
	return(1);
    }

    return(-1);
}

int load_and_align_timemarked_conversations(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *hyp_file, char *ref_file, int rm_correct_out_of_chan)
{
    WTOKE_STR1 *hyp;
    WTOKE_STR1 *ref;
    WTOKE_STR1 *hyp_1chan;
    WTOKE_STR1 *ref_1chan;
    FILE *fp_hyp, *fp_ref;
    int hyp_conv_end, hyp_eof, hyp_err;
    int ref_conv_end, ref_eof, ref_err;
    int hyp_chop_end, ref_chop_end;
    int just_read;
    int i;
    SENT *sent;
    int ref_end_chan1, hyp_end_chan1, ref_end_chan2, hyp_end_chan2;
    int ref_end, hyp_end, ref_begin, hyp_begin;
    int ref_opp_end, hyp_opp_end, ref_opp_begin, hyp_opp_begin;
    int dbg = TRUE;
    int chunk;
    char uttid[100];
    int spkr;
    int number_of_channels;
    
    alloc_singarr(hyp,1,WTOKE_STR1);
    alloc_singarr(ref,1,WTOKE_STR1);
    alloc_singarr(ref_1chan,1,WTOKE_STR1);
    alloc_singarr(hyp_1chan,1,WTOKE_STR1);

    /* init the segment structure */
    hyp->s=1; hyp->n=0; hyp->id=(char *)0;
    ref->s=1; ref->n=0; ref->id=(char *)0;
    hyp_1chan->s=1; hyp_1chan->n=0; hyp_1chan->id=(char *)0;
    ref_1chan->s=1; ref_1chan->n=0; ref_1chan->id=(char *)0;

    dbg=0;

    if ((fp_hyp = fopen(hyp_file,"r")) == NULL){
        fprintf(stderr,"Can't open input hypothesis file %s\n",hyp_file);
        exit(-1);
    }
    if ((fp_ref = fopen(ref_file,"r")) == NULL){
        fprintf(stderr,"Can't open input reference file %s\n",ref_file);
	exit(-1);
    }

    hyp_eof = ref_eof = FALSE;
    do {
	do {
	    locate_boundary(hyp, hyp->s, TRUE, FALSE, &hyp_conv_end);
	    just_read = FALSE;
	    if ((hyp_conv_end == hyp->n) && !hyp_eof){
		fill_mark_struct(fp_hyp,hyp,hyp_file,&hyp_eof,&hyp_err);
		if (hyp_err != 0){
		    fprintf(stdout,"; *Err: Error detected in hyp file '%s'\n",
			    hyp_file);
		    exit(-1);
		}
		just_read = TRUE;
	    }
	} while (just_read);

	do {
	    locate_boundary(ref, ref->s, TRUE, FALSE, &ref_conv_end);
	    just_read = FALSE;
	    if ((ref_conv_end == ref->n) && !ref_eof){
		fill_mark_struct(fp_ref,ref,ref_file,&ref_eof,&ref_err);
		reset_WTOKE_flag(ref,"overlapped");
		if (ref_err != 0){
		    fprintf(stdout,"; *Err: Error detected in ref file '%s'\n",
			    ref_file);
		    exit(-1);
		}
		just_read = TRUE;
	    }
	} while (just_read);
	
	locate_boundary(ref, ref->s, TRUE, TRUE, &ref_end_chan1);
	locate_boundary(hyp, hyp->s, TRUE, TRUE, &hyp_end_chan1);

	if (ref_end_chan1+1 < ref->n &&
	    strcmp(ref->word[ref->s].conv,ref->word[ref_end_chan1+1].conv)==0){
	    locate_boundary(ref, ref_end_chan1+1, TRUE, TRUE, &ref_end_chan2);
	    locate_boundary(hyp, hyp_end_chan1+1, TRUE, TRUE, &hyp_end_chan2);
	    number_of_channels = 2;
	} else {
	    /* There was no second channel */
	    ref_end_chan2 = ref_end_chan1;
	    hyp_end_chan2 = hyp_end_chan1;
	    number_of_channels = 1;
	}

	if (dbg > 5){
	    printf("Located REF boundary: for %s %s -> %d,%d\n",
		   ref->word[ref->s].conv,
		   ref->word[ref->s].turn,ref->s,ref_conv_end);
	    printf("Located HYP boundary: for %s %s -> %d,%d\n",
		   hyp->word[hyp->s].conv,
		   hyp->word[hyp->s].turn,hyp->s,hyp_conv_end);

	    printf("Located REF Channel 1 boundary: for %d - %d\n",
		   ref->s,ref_end_chan1);
	    printf("Located HYP Channel 1 boundary: for %d - %d\n",
		   hyp->s,hyp_end_chan1);	
	    printf("Located REF Channel 2 boundary: for %d - %d\n",
		   ref_end_chan1+1,ref_end_chan2);
	    printf("Located HYP Channel 2 boundary: for %d - %d\n",
		   hyp_end_chan1+1,hyp_end_chan2);
	}


	/* Start the alignment */
	/* Side A */
	for (i=0; i<number_of_channels; i++){
	    int base_len;
	    chunk = 0;	    
	    if (i==0){
		ref_end = ref_end_chan1;
		hyp_end = hyp_end_chan1;
		ref_begin = ref->s;
		hyp_begin = hyp->s;

		ref_opp_end = ref_end_chan2;
		hyp_opp_end = hyp_end_chan2;
		ref_opp_begin = ref_end_chan1 + 1;
		hyp_opp_begin = hyp_end_chan1 + 1;
	    } else {
		ref_end = ref_end_chan2;
		hyp_end = hyp_end_chan2;
		ref_begin = ref_end_chan1 + 1;
		hyp_begin = hyp_end_chan1 + 1;

		ref_opp_end = ref_end_chan1;
		hyp_opp_end = hyp_end_chan1;
		ref_opp_begin = ref->s;
		hyp_opp_begin = hyp->s;
	    }
	    if (dbg > 5){
		printf("Starting channel %d\n",i);
		printf("Ref Range: %d - %d\n",ref_begin,ref_end);
		printf("Hyp Range: %d - %d\n",hyp_begin,hyp_end);
	    }
	    while (ref_begin<=ref_end || hyp_begin<=hyp_end){
		int removals;

		base_len = 50;
		while (! chop_WTOKE_2(ref, hyp, ref_begin, hyp_begin,
				      ref_end, hyp_end, base_len,
				      &ref_chop_end,&hyp_chop_end)){
		    /* printf("Expanding len from %d to %d\n",base_len,
		       base_len*2); */
		    base_len *= 2;
		}

		removals = 0;
		sprintf(uttid,"(%s-%s-%04d)",ref->word[ref_begin].conv,
			ref->word[ref_begin].turn,chunk);

		spkr = spkr_num_from_id(align_str,uttid,sys_type(align_str));
		alloc_SENT_for_SPKR(spkr_n(align_str,spkr),num_sents(align_str,spkr));
		sent = sent_n(align_str,spkr,num_sents(align_str,spkr));
		strncpy(s_id(sent),uttid,SENT_ID_LENGTH-1);
		num_sents(align_str,spkr) ++;

		if (dbg > 5){
		    printf("Chunk %d: Starting for channel %d  id=%s",
			   chunk,i,uttid);
		    printf("    Ref Range: %d - %d",ref_begin,ref_chop_end);
		    printf("    Hyp Range: %d - %d\n",hyp_begin,hyp_chop_end);
		}
		
		/* Copy the segment to the staging area */
		copy_WTOKE(hyp_1chan,hyp,hyp_begin,hyp_chop_end);
		copy_WTOKE(ref_1chan,ref,ref_begin,ref_chop_end);
		
	        perform_time_alignment(pcs, ref_1chan, hyp_1chan, sent, dbg);


		if (rm_correct_out_of_chan)
		    remove_correct_out_of_chan(pcs,ref_1chan,ref,hyp_1chan,
					       sent,dbg,ref_begin,hyp_begin,
					       ref_opp_begin,ref_opp_end);

		sort_time_marked_SENT(sent);

		if (dbg > 10){
		    printf("After Sorting\n");
		    dump_SENT_wwt(pcs,sent);
		}


		spkr_nref(align_str,spkr) += s_nref(sent); 
		sys_nref(align_str) += s_nref(sent);

		free_mark_file(ref_1chan);
		free_mark_file(hyp_1chan);

		ref_begin = ref_chop_end+1;
		hyp_begin = hyp_chop_end+1;

		chunk++;
	    }
	}


	hyp->s = hyp_conv_end+1;
	ref->s = ref_conv_end+1;

    } while (hyp->s <= hyp->n && ref->s <= ref->n);

    free_mark_file(ref);
    free_mark_file(hyp);

    /* Tell the function to clean itself up */
    align_WTOKE_into_SENT((SENT *)0,(WTOKE_STR1 *)0,(WTOKE_STR1 *)0,pcs);

    fclose(fp_hyp);
    fclose(fp_ref);

    free_singarr(hyp,WTOKE_STR1);
    free_singarr(ref,WTOKE_STR1);
    free_singarr(ref_1chan,WTOKE_STR1);
    free_singarr(hyp_1chan,WTOKE_STR1);

    return (0);
}

int perform_time_alignment(TABLE_OF_CODESETS *pcs, WTOKE_STR1 *ref, WTOKE_STR1 *hyp, SENT *sent, int dbg)
{
    char *pname="perform_time_alignment";
    int ret;

    if (dbg > 10){
	printf("%s: The ref SET\n",pname);
	dump_word_tokens2(ref,1,ref->n);
	printf("%s: The Hyp SET\n",pname);
	dump_word_tokens2(hyp,1,hyp->n); 
    }

    if ((ret=align_WTOKE_into_SENT(sent,ref,hyp,pcs)) != 0){
	printf("; Err: align_WTOKE_into_SENT returned %d\n",ret);
	exit(1);
    }			   
    
    if (dbg > 10){
	printf("After Alignment\n");
	dump_SENT_wwt(pcs,sent);
    }
    return(0);
}

int remove_correct_out_of_chan(TABLE_OF_CODESETS *pcs, WTOKE_STR1 *ref, WTOKE_STR1 *ref_opp, WTOKE_STR1 *hyp, SENT *sent, int dbg, int ref_begin, int hyp_begin, int ref_opp_begin, int ref_opp_end)
{
    int w, removals=0, ret;
    
    /* Remove any error which are caused by true hits in the */
    /* opposite channel */
    if (dbg > 5) 
	printf("Forgiving correct, out-of-channel words\n");
    
    /* Loop through the words */
    for (w=0; s_eval_wrd(sent,w) != END_OF_WORDS; w++){
	if (is_SUB(s_eval_wrd(sent,w)) || is_INS(s_eval_wrd(sent,w))){
	    char *str = lex_lc_word(pcs,s_hyp_wrd(sent,w));
	    
	    if (is_word_match_in_WTOKE(ref_opp,ref_opp_begin, ref_opp_end,str,
				       s_hyp_beg_wrd(sent,w),
				       s_hyp_dur_wrd(sent,w)) > 0){
		removals ++;
		if (dbg > 10) printf("Removing word %d '%s'\n",w,str);
		s_hyp_wrd(sent,w) = DELETION;
		s_hyp_beg_wrd(sent,w) = 0.00;
		s_hyp_dur_wrd(sent,w) = 0.00;
	    }
	}
    }
    
    if (removals > 0){
	free_mark_file(ref);
	free_mark_file(hyp);
	
	ref->n = 0;
	hyp->n = 0;
	ref->id = mtrf_strdup(s_id(sent));
	hyp->id = mtrf_strdup(s_id(sent));
	
	for (w=0; s_eval_wrd(sent,w) != END_OF_WORDS; w++){
	    if (s_ref_wrd(sent,w) > 0) {
		ref->n++;	
		
		ref->word[ref->n].turn  = 
		    mtrf_strdup(ref_opp->word[ref_begin].turn);
		ref->word[ref->n].conv  = 
		    mtrf_strdup(ref_opp->word[ref_begin].conv);
		ref->word[ref->n].sp  = 
		    mtrf_strdup(lex_lc_word(pcs,s_ref_wrd(sent,w)));
		ref->word[ref->n].t1  =	    s_ref_beg_wrd(sent,w);
		ref->word[ref->n].dur =     s_ref_dur_wrd(sent,w);
	    }
	    if (s_hyp_wrd(sent,w) > 0) {
		hyp->n++;
		hyp->word[hyp->n].turn  = 
		    mtrf_strdup(ref_opp->word[ref_begin].turn);
		hyp->word[hyp->n].conv  = 
		    mtrf_strdup(ref_opp->word[ref_begin].conv);
		hyp->word[hyp->n].sp  = 
		    mtrf_strdup(lex_lc_word(pcs,s_hyp_wrd(sent,w)));
		hyp->word[hyp->n].t1  =	    s_hyp_beg_wrd(sent,w);
		hyp->word[hyp->n].dur =	    s_hyp_dur_wrd(sent,w);
	    }
	}
	
	if ((ret=align_WTOKE_into_SENT(sent,ref,hyp,pcs))!=0){
	    printf("; Err: align_WTOKE_into_SENT returned %d\n",ret);
	    exit(1);
	}			   
	
	if (dbg > 10){ 
	    printf("After Re-Alignment\n");
	    dump_SENT_wwt(pcs,sent); 
	}
    }
    return(0);
}


