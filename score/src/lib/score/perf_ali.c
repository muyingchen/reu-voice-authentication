/**********************************************************************/
/*                                                                    */
/*             FILENAME:  perf_ali.c                                  */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*             DESC: This procedure set performs alignments on        */
/*                   specific types of objects.                       */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/* defined in src/lib/score/sysalign.c */
extern int global_use_phonetic_alignments;


/*===================================================================*/
/*                STRING ALIGNMENT FUNCTIONS CALLS                   */

void align_hypcstr_and_refcstr_into_SENT(SENT *ts, char *hypcstr, char *refcstr, TABLE_OF_CODESETS *pcs)
{
    char *proc = "align_hypcstr_and_refcstr_into_SENT";
    ALIGNMENT_INT *aln_base,*aln_best;
    pcodeset *pcode1;
    int err, *perr = &err;

    /*   code   */
    /* get pcode table */
    pcode1 = pcs->cd[1].pc;

    if (db_level > 0) {
	fprintf(stdout,"\n The highest level codeset defined is %s.\n",
		pcs->cd[1].pfname);
	fprintf(stdout," Type is '%s', table length= %d ,  ncodes=%d.\n",
		pcode1->element_type,pcode1->pctab_length,pcode1->ncodes);
	dump_code_table(pcs);
    }

    /* make alignment data objects */
    aln_base = make_al_int(pcode1);
    aln_best = make_al_int(pcode1);

    encode2(hypcstr,pcode1,&(aln_base->bint[0]),&(aln_base->bint[0]),perr);
    if (*perr > 0) printf("%s:*ERR: encode2(hypstr) returns %d\n",proc,*perr);

    align_hypistr_refcstr_into_SENT(ts,aln_base,aln_best,refcstr,pcs);

    free_al_int(aln_best);
    free_al_int(aln_base);
}


int align_hypntistr_and_refcstr_into_SENT(SENT *ts, WCHAR_T *hypntistr, char *refcstr, TABLE_OF_CODESETS *pcs)
{

    static ALIGNMENT_INT *aln_base=(ALIGNMENT_INT *)0;
    static ALIGNMENT_INT *aln_best=(ALIGNMENT_INT *)0;
    pcodeset *pcode1;
    int ret;

    /*   code   */

    /* DEALLOCATION CODE    only used if the passed in sentencec is NULL */
    if (ts == (SENT *)0){
	if (aln_base == (ALIGNMENT_INT *)0) {
	    free_al_int(aln_base); aln_base = (ALIGNMENT_INT *)0;
	}
	if (aln_best == (ALIGNMENT_INT *)0) {
	    free_al_int(aln_best); aln_best = (ALIGNMENT_INT *)0;
	}
	return(0);
    }

    pcode1 = pcs->cd[1].pc;

    if (db_level > 0) {
	fprintf(stdout,"\n The highest level codeset defined is %s.\n",
		pcs->cd[1].pfname);
	fprintf(stdout," Type is '%s', table length= %d ,  ncodes=%d.\n",
		pcode1->element_type,pcode1->pctab_length,pcode1->ncodes);
	dump_code_table(pcs);
    }

    /* make alignment data objects */
    if (aln_base == (ALIGNMENT_INT *)0) aln_base = make_al_int(pcode1);
    if (aln_best == (ALIGNMENT_INT *)0) aln_best = make_al_int(pcode1);

    if ((ret = copy_ntistr2istr(hypntistr,MAX_NUM_WORDS,
				&(aln_base->bint[0]),MAX_SYMBS_IN_STR)) != 0){
	fprintf(stderr,"Error: Unable to copy and null-term int str to and istr");
	fprintf(stderr,", returns %d\n",ret);
    }

    ret = align_hypistr_refcstr_into_SENT(ts,aln_base,aln_best,refcstr,pcs);
    return(ret);
}
 
int align_hypistr_refcstr_into_SENT(SENT *ts, ALIGNMENT_INT *aln_base, ALIGNMENT_INT *aln_best, char *refcstr, TABLE_OF_CODESETS *pcs)
{
    char *proc = "align_hypistr_refcstr_into_SENT";
    pcodeset *pcode1;
    int err, *perr = &err;

    pcode1 = pcs->cd[1].pc;

    if (! string_has_alternates(refcstr)){
	encode2(refcstr,pcode1,&(aln_base->aint[0]),&(aln_base->aint[0]),perr);
	s_nref(ts) = aln_base->aint[0];
	if (*perr > 0) {
	    printf("%s:*ERR: encode2(refcstr) returns %d\n",proc,*perr);
	    printf("%s: refcstr = %s\n",proc,refcstr);
	    return(-1);
	}
	if (align_ali_int_copy_to_SENT(aln_base,aln_best,ts) != 0){
	    printf("%s: align_ali_int_copy_to_SENT Failed\n",proc);
	    return(-1);
	}
    } else { /* do the network stuff */
	boolean old_compositional;
	boolean return_alignment=T;
	int err, *perr = &err, distance, pathn, ndel;
	ALIGNMENT_CHR *ps1;
	NETWORK *net;

	net = init_network2("REF NETWORK",refcstr,perr);
	s_nref(ts) = 0;
	if (*perr > 0){
	    printf("%s:*ERR: init_network2 returns %d\n",proc,*perr);
	    return(-1);
	}
	if (db_level > 0) {
	    printf("%safter init_network2(), net=\n",pdb);
	    dump_network(net);
	}

	deflag1_node_r(net->start_node);
	literalize_network(net,net->start_node,perr);
	if (*perr > 0) {
	    printf("%s:*ERR: lit network returns %d\n",proc,*perr);
	    return(-1);
	}

	if (db_level > 0){
	    printf("%safter literalize_network(), net=\n",pdb);
	    dump_network(net);
	}
	
	deflag1_node_r(net->start_node);
	encode_arcs_r(net->start_node,pcode1,perr);
	if (*perr > 0) {
	    printf("%s:*ERR: encode_network returns %d\n",proc,*perr);
	    return(-1);
	}

	if (db_level > 0){
	    printf("%safter encode_arcs_R(), net=\n",pdb);
	    dump_network(net);
	}

	aln_base->aint[0] = 0;
	aln_base->aligned = F;
	aln_best->d = INT_MAX;
	aln_best->aligned = F;
	pathn = 0;
        /* and calc distance */

	if (db_level > 0){
	    printf("%sjust before aldistn1(net,...), net=\n",pdb);
	    dump_network(net);
	}
	
	if (! global_use_phonetic_alignments){
	    old_compositional = aln_base->pcset->compositional;
	    aln_base->pcset->compositional = F;
	}

	distance = aldistn1(net->start_node,aln_base,wod2,
			     return_alignment,aln_best,&pathn,perr);

	if (! global_use_phonetic_alignments)
	    aln_base->pcset->compositional = old_compositional;

	if (! global_use_phonetic_alignments)
	    pcode1->compositional = old_compositional;

	if (db_level > 0) fprintf(stdout," Done, distance = %d\n",distance);
	if (*perr > 10) {
	    fprintf(stdout," *ERR: aldist returns *perr=%d\n",*perr);
	    return(-1);
	}
	if (*perr <= 10) *perr = 0;
	/* NOTE: compact() is not really necessary - it zaps 0:0 correspondences */
	compact(aln_best,&ndel); 

	ali_int_to_SENT(ts,aln_best);

	if (db_level > 0) {
	    if (db_level > 1) printf("%saln_best->d=%d\n",pdb,aln_best->d);
	    fprintf(stdout," The best alignment is:\n\n");
	    ps1 = make_al_chr();
	    ps1->aligned = F;
	    aln_i_to_s_fancy(aln_best,ps1,perr);
	    dump_chr_alignment(ps1);
	    free_al_chr(ps1);
	}
	free_network(net);

	/* count the number of reference words in the output */
	{   int l;
	    for (l=1; l<=aln_best->aint[0]; l++)
		s_nref(ts) += (aln_best->aint[l] != 0) ? 1 : 0;
	}
    }
    return 0;
}


int align_ali_int_copy_to_SENT(ALIGNMENT_INT *aln_base, ALIGNMENT_INT *aln_out, SENT *ts)
{
    ALIGNMENT_CHR *ps1;
    int distance, err, *perr=&err;
    boolean old_compositional;

    aln_base->aligned = F;
    aln_out->d = INT_MAX;
    /* and calc distance */

    if (! global_use_phonetic_alignments){
	old_compositional = aln_base->pcset->compositional;
	aln_base->pcset->compositional = F;
    }

    distance = aldist2(aln_base,wod2,TRUE,aln_out,perr);
    if (! global_use_phonetic_alignments)
	aln_base->pcset->compositional = old_compositional;

    if (db_level > 0) fprintf(stdout," Done, distance = %d\n",distance);
    if (*perr > 10) {
	fprintf(stdout," *ERR: aldist2 returns *perr=%d\n",*perr);
    } 	

    ali_int_to_SENT(ts,aln_out);

    if (db_level > 0) {
	fprintf(stdout," The best alignment is:\n\n");
	ps1 = make_al_chr();
	ps1->aligned = F;
	aln_i_to_s_fancy(aln_out,ps1,perr);
	dump_chr_alignment(ps1);
	free_al_chr(ps1);
    }
    return(0);
}



/*                END STRING ALIGNMENT FUNCTIONS CALLS               */
/*===================================================================*/



int copy_ntistr2istr(WCHAR_T *ntistr, int numnti, WCHAR_T *istr, int numi)
{
    int i;
    if (numnti < istr[0])
	return(10);
    istr[0] = 0;
    for (i=0; ntistr[i] != END_OF_WORDS && istr[0] <= numi; i++)
	istr[++istr[0]] = ntistr[i];
    if (istr[0] > numi && ntistr[i] != END_OF_WORDS)
	return(11);
    return(0);
}

/* copy 2 to 1 */
int copy_istr(WCHAR_T *istr1, WCHAR_T *istr2)
{
    int i;
    for (i=0; i <= istr2[0]; i++)
	istr1[i] = istr2[i];

    return(0);
}


void SENT_to_ali_int(SENT *ts, ALIGNMENT_INT *al_int)
{
    int l;
 
    al_int->aint[0] = 0;
    for (l=0; s_ref_wrd(ts,l) != END_OF_WORDS; l++)
        al_int->aint[++al_int->aint[0]] = s_ref_wrd(ts,l);
    al_int->bint[0] = 0;
    for (l=0; s_hyp_wrd(ts,l) != END_OF_WORDS; l++)
        al_int->bint[++al_int->bint[0]] = s_hyp_wrd(ts,l);
}
 
void ali_int_to_SENT(SENT *ts, ALIGNMENT_INT *al_int)
{
    int l;
    for (l=1; l<=al_int->aint[0]; l++){
	/* printf("  word %d  %d\n",l,al_int->aint[l]); */
	s_ref_wrd(ts,l-1) = (al_int->aint[l] == 0) ? INSERTION : al_int->aint[l];
	s_hyp_wrd(ts,l-1) = (al_int->bint[l] == 0) ? DELETION : al_int->bint[l];
	s_eval_wrd(ts,l-1)= END_OF_WORDS;
    }
    s_hyp_wrd(ts,l-1) = END_OF_WORDS;
    s_ref_wrd(ts,l-1) = END_OF_WORDS;
    s_eval_wrd(ts,l-1)= END_OF_WORDS;


    /* dump_SENT(ts); */
}

void copy_ali_int_and_WTOKE_to_SENT(SENT *ts, ALIGNMENT_INT *al_int, WTOKE_STR1 *ref_segs, WTOKE_STR1 *hyp_segs)
{
    int l, r=1, h=1;
    for (l=1; l<=al_int->aint[0]; l++){
	/* printf("  word %d  %d %d\n",l,al_int->aint[l],al_int->bint[l]); */
	if (al_int->aint[l] == 0) {
	    s_ref_wrd(ts,l-1) = INSERTION;
	    s_ref_beg_wrd(ts,l-1) = 0.0;
	    s_ref_dur_wrd(ts,l-1) = 0.0;
	} else {
	    s_ref_wrd(ts,l-1) = al_int->aint[l];
	    s_ref_beg_wrd(ts,l-1) = ref_segs->word[r].t1;
	    s_ref_dur_wrd(ts,l-1) = ref_segs->word[r].dur;
	    r++;
	}
	if (al_int->bint[l] == 0) {
	    s_hyp_wrd(ts,l-1) = DELETION;
	    s_hyp_beg_wrd(ts,l-1) = 0.0;
	    s_hyp_dur_wrd(ts,l-1) = 0.0;
	} else {
	    s_hyp_wrd(ts,l-1) = al_int->bint[l];
	    s_hyp_beg_wrd(ts,l-1) = hyp_segs->word[h].t1;
	    s_hyp_dur_wrd(ts,l-1) = hyp_segs->word[h].dur;
	    h++;
	}
	s_eval_wrd(ts,l-1)= END_OF_WORDS;
    }
    s_hyp_wrd(ts,l-1) = END_OF_WORDS;
    s_ref_wrd(ts,l-1) = END_OF_WORDS;
    s_eval_wrd(ts,l-1)= END_OF_WORDS;
}


int string_has_alternates(char *str)
{
    char *p;
    if ((p=strchr(str,'{')) == NULL)
	return(0);
    if ((p=strchr(p,'/')) == NULL)
	return(0);
    if ((p=strchr(p,'}')) == NULL)
	return(0);
    return(1);
}

