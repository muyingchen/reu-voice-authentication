

/* loadhyps.c */ void load_and_align_hyp_to_ref_into_SYS_ALIGN PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *hyp_file, char *ref_file, int strip_silence));

/* loadhyps.c */ void load_and_align_timemarked_hyp_to_ref_into_SYS_ALIGN PROTO((SYS_ALIGN *align_str,TABLE_OF_CODESETS *pcs,char *hyp_file,char *ref_file,int rm_correct_out_of_chan));

/* loadhyps.c */ void load_hyp_into_SYS_ALIGN PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *filename, int strip_silence));

/* loadhyps.c */ void load_and_align_ref_from_SYS_ALIGN PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *ref_file));

/* loadhyps.c */  int load_and_align_timemarked_conversations PROTO((SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *hyp_file, char *ref_file, int strip_silence));


/* timemark.c */  int align_WTOKE_into_SENT PROTO((SENT *sent, WTOKE_STR1 *ref_segs, WTOKE_STR1 *hyp_segs, TABLE_OF_CODESETS *pcs));



void dump_word_tokens2 PROTO((WTOKE_STR1 *word_tokens, int, int lim));

int chop_WTOKE_2(WTOKE_STR1 *seg1, WTOKE_STR1 *seg2, int start1, int start2, int endconv1, int endconv2, int max_words, int *end1, int *end2);

void copy_WTOKE(WTOKE_STR1 *to_seg,WTOKE_STR1 *from_seg, int from_w, int to_w);

void locate_boundary(WTOKE_STR1 *seg, int start, int by_turn, int by_channel, int *end);

double overlap(double s1_t1, double s1_t2, double s2_t1, double s2_t2);

void reset_WTOKE_flag(WTOKE_STR1 *seg,char *flag_name);
