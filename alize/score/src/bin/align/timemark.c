#include <phone/stdcenvf.h>
#include <score/scorelib.h>

#include "alifunc.h"

/*****************************************************************/
/*   This set of alignment functions uses Recursion AND Global   */
/*   variables to align WTOKE structures which possibly contain  */
/*   alternations.                                               */
/*****************************************************************/

static ALIGNMENT_CHR *ST_ps1=(ALIGNMENT_CHR *)0, *ST_ps2;
static ALIGNMENT_INT *ST_pn1,*ST_pn2; 
static SENT *ST_best_sent;
static pcodeset *ST_pcode1;
static TABLE_OF_CODESETS *ST_pcs;
static int ST_best_score;

int dump_path PROTO((WTOKE_STR1 *));
int find_alts PROTO((SENT *, WTOKE_STR1 *, WTOKE_STR1 *, WTOKE_STR1 *, int, int (*func)(SENT *sent, WTOKE_STR1 *ref, WTOKE_STR1 *hyp)));
int do_WTOKE_alignment PROTO((SENT *sent, WTOKE_STR1 *ref_segs, WTOKE_STR1 *hyp_segs));
int evaluate_alternate PROTO((SENT *sent, WTOKE_STR1 *ref, WTOKE_STR1 *hyp));

int align_WTOKE_into_SENT(SENT *sent, WTOKE_STR1 *ref_segs, WTOKE_STR1 *hyp_segs,TABLE_OF_CODESETS *pcs)
{
    int has_alternates, i;

    ST_pcs = pcs;
    ST_pcode1 = pcs->cd[1].pc; /* to make references easier            */
    ST_best_score = 99999;

    if (ST_ps1 == (ALIGNMENT_CHR *)0){
	ST_ps1 = make_al_chr();
	ST_ps2 = make_al_chr();
	ST_pn1 = make_al_int(ST_pcode1);
	ST_pn2 = make_al_int(ST_pcode1);
	alloc_and_init_SENT(ST_best_sent,MAX_NUM_WORDS);
    }
    if (sent == (SENT *)0){
	free_al_chr(ST_ps1);
	free_al_chr(ST_ps2);
	free_al_int(ST_pn1);
	free_al_int(ST_pn2);
	free_SENT(ST_best_sent);
	return(0);
    }

    strncpy(s_id(ST_best_sent),s_id(sent),SENT_ID_LENGTH-1);
    for (i=1,has_alternates=FALSE; i<=ref_segs->n; i++)
	if (ref_segs->word[i].alternate){
	    has_alternates = TRUE;
	    break;
	}

    if (has_alternates){
	WTOKE_STR1 temp_ref;
	temp_ref.n = 0;
	find_alts(sent, ref_segs, &temp_ref, hyp_segs, 1, evaluate_alternate);
    } else {
	do_WTOKE_alignment(sent,ref_segs,hyp_segs);
    }

    return(0);
}

int do_WTOKE_alignment(SENT *sent, WTOKE_STR1 *ref_segs, WTOKE_STR1 *hyp_segs)
{
    int err; int *perr = &err;
    int d, i, j, return_alignment=TRUE;
    int len;
    
    /* make alignment in ST_ps1 from file data*/
    init_al_chr(ST_ps1);
    ST_ps1->pastr = strcpy(ST_ps1->pastr," ");
    len = strlen(ST_ps1->pastr);
    for (i=1; i<=ref_segs->n; i++){
	len += strlen(ref_segs->word[i].sp) + 1;
	if (len > MAX_STR_NCHARS){
	    fprintf(stderr,"*ERR: too long of char alingment string.\n");
	    fprintf(stderr," increase MAX_STR_NCHARS & recompile.\n");
	    return 9;
	}
	ST_ps1->pastr = strcat(ST_ps1->pastr,ref_segs->word[i].sp);
	ST_ps1->pastr = strcat(ST_ps1->pastr," ");
    }
    ST_ps1->pbstr = strcpy(ST_ps1->pbstr," ");
    len = strlen(ST_ps1->pbstr);
    for (j=1; j<=hyp_segs->n; j++) {
	len += strlen(hyp_segs->word[j].sp) + 1;
	if (len > MAX_STR_NCHARS){
	    fprintf(stderr,"*ERR: too long of char alingment string.\n");
	    fprintf(stderr," increase MAX_STR_NCHARS & recompile.\n");
	    return 9;
	}
	ST_ps1->pbstr = strcat(ST_ps1->pbstr,hyp_segs->word[j].sp);
	ST_ps1->pbstr = strcat(ST_ps1->pbstr," ");
    }
    ST_ps1->aligned = F;

    /* adjust type case to fit ST_pcode1 */
    adjust_type_case(ST_ps1->pastr,ST_pcode1);
    adjust_type_case(ST_ps1->pbstr,ST_pcode1);

    alignment_s_to_i(ST_pn1,ST_ps1,ST_pcode1,perr); 
    if (*perr > 0) {
	fprintf(stdout,"; *ERR: alignment_s_to_i returns perr=%d\n",*perr);
	return 7;
    }

    if (db_level > 0) {
	printf("*DB: after alignment_s_to_i, *ST_pn1 is:\n");
	dump_int_alignment(ST_pn1);
    }

    d=aldistt1(ST_pn1,ref_segs,hyp_segs,return_alignment,ST_pn2,perr);

    if (*perr > 1){
	printf("aldistt1: returns error %d\n",*perr);
	return(*perr);
    }
	
    if (db_level > 0) {
	aln_i_to_s_fancy(ST_pn2,ST_ps2,perr);
           if (*perr > 0){
	       fprintf(stdout,"; *ERR: aln_i_to_s err=%d\n",*perr);
	       return 8;
	   }
	fprintf(stdout,";\n");
	fprintf(stdout,"REF: %s\n",ST_ps2->pastr);
	fprintf(stdout,"HYP: %s\n",ST_ps2->pbstr);
    }
    copy_ali_int_and_WTOKE_to_SENT(sent, ST_pn2, ref_segs, hyp_segs);

    /* count the number of reference words in the output */
    {   int l;
	s_nref(sent) = 0;
	for (l=1; l<=ST_pn2->aint[0]; l++)
	    s_nref(sent) += (ST_pn2->aint[l] != 0) ? 1 : 0;
    }

    score_SENT(sent,(HOMO *)0,ST_pcs,(LEX_SUBSET *)0, FALSE);
    return(0);
}

int find_alts(SENT *sent, WTOKE_STR1 *ref, WTOKE_STR1 *tref, WTOKE_STR1 *hyp, int start, int (*func)(SENT *sent, WTOKE_STR1 *ref, WTOKE_STR1 *hyp))
{
    int i, j, more_paths=1, first_alt=(-1), terminate_path;
    int outn, start_o = tref->n, dbg=0;
    /* Erase the used states from here to the end */
    for (j=start; j<=ref->n; j++)
	ref->word[j].comment = 0;
    /* end of erase */
    /* dump(f);  */
    terminate_path = 0;
    more_paths = 1;
    while (more_paths){
	if (dbg) printf("Begin Path\n");
	terminate_path = more_paths = 0;
	outn = start_o;
	for (i=start; i<=ref->n && !terminate_path; i++){
	    if (! ref->word[i].alternate){
		if (dbg) printf("   Use     %2d:  %s\n",i,ref->word[i].sp);
		tref->word[++outn] = ref->word[i];
	    } else if (strcmp("<ALT_BEGIN>",ref->word[i].sp) == 0){
		/* if this is the first alternat, proceed, but remmeber were */
		/* it was */
		if (first_alt == (-1))
		    first_alt = i;
		else if (first_alt != i){
		    /* We hit a second alternate, time for recursion */
		    tref->n = outn;
		    find_alts(sent,ref,tref,hyp,i,func);
		    terminate_path = 1;
		    continue;
		}
		i++;
		
		/* loop through previously used alternates */
		while (i <= ref->n && ref->word[i].comment){
		    i++;
		}
		while (i <= ref->n && (strncmp("<ALT",ref->word[i].sp,4) !=0)){
		    if(dbg) printf("   Use ALT %2d:  %s\n",i,ref->word[i].sp);
		    tref->word[++outn] = ref->word[i];
		    ref->word[i].comment = 1;
		    i++;
		}
		if (i <= ref->n && (strcmp("<ALT>",ref->word[i].sp) == 0)){
		    ref->word[i].comment = 1;
		}
		while (i <= ref->n &&
		       (strcmp("<ALT_END>",ref->word[i].sp) != 0)){
		    more_paths = 1;
		    i++;
		}
	    } else {
		printf("Error: abort\n");
		exit(1);
	    }
	}
	if (i > ref->n) {
	    tref->n = outn;
	    func(sent,tref,hyp);
	}
    }
    return(0);
}

int evaluate_alternate(SENT *sent, WTOKE_STR1 *ref, WTOKE_STR1 *hyp)
{
    int corr, sub, ins, del, spl, mrg;
    int dbg=0;

    if (dbg){
	printf("-------------------------\nEvaluating Alternate\n"); 
	printf("REF: "); dump_path(ref);
	printf("HYP: "); dump_path(hyp);
    }
    do_WTOKE_alignment(ST_best_sent,ref,hyp);

    count_sent_score(ST_best_sent,&corr,&sub,&ins,&del,&spl,&mrg);
    
    if (sub + ins + del + spl + mrg < ST_best_score){
	copy_SENT(sent,ST_best_sent);
	ST_best_score = sub + ins + del + spl + mrg;
	if (dbg){
	    printf("Selecting best sentence\n");
	    dump_SENT_wwt(ST_pcs,sent);
	}
    }
    return(1);
}

int dump_path(WTOKE_STR1 *f)
{
    int i;
    printf("LIST dump:  ");
    for (i=1; i<=f->n; i++){
	printf("%s ",f->word[i].sp);
    }
    printf("\n");
    return(1);
}


