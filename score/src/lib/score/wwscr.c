/**********************************************************************/
/*                                                                    */
/*             FILENAME:  wwscr.c                                     */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains utilities to perform word      */
/*                  weighted word Scoring                             */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/*************************************************************************/
/*   The word weighting scoring program                                  */

#define MAX_W 5

typedef struct word_weight_struct{
    double weight[MAX_W];
    PCIND_T pcind;
} WW;

typedef struct word_weight_list_struct{
    char *filename;
    char *weight_desc[MAX_W];
    int num_w;
    double default_weight;
    int max;
    int num;
    int curw;
    WW **words;
} WWL;

typedef struct WWL_spkr_struct{
    double ref, corr, sub, del, ins, spl, mrg;
    char *id;
} WWL_SPKR;

typedef struct WWL_FUNC_struct{
    double ref, corr, sub, del, ins, spl, mrg;
    int n_spkrs;
    WWL_SPKR *spkr;
    char *title;
} WWL_FUNC;

typedef struct WWL_score_struct{
    int numfunc;
    int maxfunc;
    WWL_FUNC *func;
} WWL_SCORE;

void do_weighted_score PROTO((SYS_ALIGN *sys_align, TABLE_OF_CODESETS *pcs, WWL*, int dbg, double (*Weight) (PCIND_T, TABLE_OF_CODESETS *, WWL *), WWL_FUNC *wwlscore));

double Weight_wwl PROTO((PCIND_T ind, TABLE_OF_CODESETS *pcs, WWL *wwl));
double Weight_one PROTO((PCIND_T ind, TABLE_OF_CODESETS *pcs, WWL *wwl));
double Weight_phones PROTO((PCIND_T ind, TABLE_OF_CODESETS *pcs, WWL *wwl));
double Weight_syllables PROTO((PCIND_T ind, TABLE_OF_CODESETS *pcs, WWL *wwl));
int load_WWL PROTO((WWL **wwl, char *filename, TABLE_OF_CODESETS *pcs));
void dump_WWL PROTO((WWL *wwl, TABLE_OF_CODESETS *pcs, FILE *fp));
int compare_WW PROTO((const void *ww1, const void *ww2));
int is_same_pcind PROTO((const void *key, const void *datum));
void free_WWL PROTO((WWL **wwl));
double get_weight_from_WW_by_pcind PROTO((WWL *wwl, PCIND_T ind));
WWL_SCORE *alloc_WWL_SCORE PROTO((int num_func, int num_spk));
void free_WWL_SCORE PROTO((WWL_SCORE *wwlscr));

static void make_total_summary PROTO((WWL_SCORE *wwlscore, FILE *fp));
static void make_speaker_summary PROTO((WWL_SCORE *wwlscore, FILE *fp));
void expand_WWL_SCORE PROTO((WWL_SCORE *wwlscr));



int perform_word_weighted_scoring(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, char *sys_root_name, int do_weight_one, int do_phone_weight, int do_syllables, int n_wwlf, char **wwl_files, int make_sum, int make_ovrall)
{    
    WWL *wwl=(WWL *)0;
    WWL_SCORE *wwlscore;
    int wf, dbg=0, wfc, maxwwlf;

    maxwwlf = n_wwlf + ((do_weight_one) ? 1 : 0)
	             + ((do_phone_weight) ? 1 : 0)
		     + ((do_syllables) ? 1 : 0);
    wwlscore = alloc_WWL_SCORE(maxwwlf,num_spkr(align_str));

    if (do_weight_one){
	wwlscore->func[wwlscore->numfunc].title = mtrf_strdup("Weight 1");
	do_weighted_score(align_str,pcs,(WWL *)0,dbg,Weight_one,
			  &(wwlscore->func[wwlscore->numfunc]));
	wwlscore->numfunc++;
    }
    
    if (do_phone_weight){
	wwlscore->func[wwlscore->numfunc].title = mtrf_strdup("Phones");
	do_weighted_score(align_str,pcs,(WWL *)0,dbg,Weight_phones,
			  &(wwlscore->func[wwlscore->numfunc]));
	wwlscore->numfunc++;
    }

    if (do_syllables){
	wwlscore->func[wwlscore->numfunc].title = mtrf_strdup("Syllables");
	do_weighted_score(align_str,pcs,(WWL *)0,dbg,Weight_syllables,
			  &(wwlscore->func[wwlscore->numfunc]));
	wwlscore->numfunc++;
    }
    
    for (wf=0; wf<n_wwlf; wf++){
	if (load_WWL(&wwl, wwl_files[wf], pcs) != 0){
	    fprintf(stderr,"Error: Unable to read WWL file '%s'\n",
		    wwl_files[wf]);
	    return(1);
	}
	/* dump_WWL (wwl, pcs, stdout);*/
	for (wfc=0; wfc < wwl->num_w; wfc++){
	    if (wwlscore->numfunc >= wwlscore->maxfunc)
		expand_WWL_SCORE(wwlscore);
	    wwl->curw = wfc;
	    wwlscore->func[wwlscore->numfunc].title = 
		mtrf_strdup(wwl->weight_desc[wfc]);
	    do_weighted_score(align_str,pcs,wwl,dbg,Weight_wwl,
			      &(wwlscore->func[wwlscore->numfunc]));
	    wwlscore->numfunc++;
	}
	free_WWL(&wwl);
    }

    if (make_sum) {
	FILE *fp;
	char *fname;
	/* open an output file, if it fails, write to stdout */
	fp = stdout;
	if (sys_root_name != (char *)0){
	    if ((fp = fopen(fname=rsprintf("%s.%s",sys_root_name,
					   "wwl_sum"),"w")) != NULL)
		printf("Writing Weighted Word summary to '%s'\n", fname);
	}
	make_total_summary(wwlscore, fp);
	if (fp != stdout)
	    fclose(fp);
    }

    if (make_ovrall) {
	FILE *fp;
	char *fname;
	/* open an output file, if it fails, write to stdout */
	fp = stdout;
	if (sys_root_name != (char *)0){
	    if ((fp = fopen(fname=rsprintf("%s.%s",sys_root_name,
					   "wwl_sys"),"w")) != NULL)
		printf("Writing Weighted Word summary to '%s'\n",fname);
	}
	make_speaker_summary(wwlscore, fp);
	if (fp != stdout)
	    fclose(fp);
    }

    free_WWL_SCORE(wwlscore);
    return(0);
}

void expand_WWL_SCORE(WWL_SCORE *wwlscr)
{
    int min = MAX(wwlscr->maxfunc,1), i;
    WWL_FUNC *twwlf;
    /* printf("Expanding WWLSCORE structure\n"); */

    alloc_singarr(twwlf, (min*2), WWL_FUNC);
    memcpy(twwlf,wwlscr->func,wwlscr->maxfunc * sizeof(WWL_FUNC));
    free_singarr(wwlscr->func,WWL_FUNC);
    wwlscr->func = twwlf;
    for (i=wwlscr->maxfunc; i<min*2; i++){
	wwlscr->func[i].n_spkrs = wwlscr->func[0].n_spkrs;
	wwlscr->func[i].title = (char *)0;
	alloc_singarr(wwlscr->func[i].spkr,wwlscr->func[i].n_spkrs,WWL_SPKR);
    }
    wwlscr->maxfunc = min*2;
}

WWL_SCORE *alloc_WWL_SCORE(int num_func, int num_spk)
{
    WWL_SCORE *twwlscr;
    int i;

    alloc_singarr(twwlscr,1,WWL_SCORE);
    twwlscr->maxfunc = num_func;
    twwlscr->numfunc = 0;
    alloc_singarr(twwlscr->func,twwlscr->maxfunc,WWL_FUNC);
    for (i=0; i<twwlscr->maxfunc; i++){
	twwlscr->func[i].n_spkrs = num_spk;
	twwlscr->func[i].title = (char *)0;
	alloc_singarr(twwlscr->func[i].spkr,twwlscr->func[i].n_spkrs,WWL_SPKR);
    }
    return (twwlscr);
}

void free_WWL_SCORE(WWL_SCORE *wwlscr)
{
    int i, s;

    for (i=0; i<wwlscr->maxfunc; i++){
	if (i < wwlscr->numfunc){
	    mtrf_free(wwlscr->func[i].title);
	    for (s=0; s<wwlscr->func[i].n_spkrs; s++)
		mtrf_free(wwlscr->func[i].spkr[s].id);
	}
	free_singarr(wwlscr->func[i].spkr,WWL_SPKR);
    }
    free_singarr(wwlscr->func,WWL_FUNC);
    free_singarr(wwlscr,WWL_SCORE);
}

static void make_total_summary(WWL_SCORE *wwlscore, FILE *fp)
{
    int cfunc;
    char *ffmt="%6.2f ";
    Desc_erase();
    Desc_set_page_center(SCREEN_WIDTH);
    Desc_add_row_values("l|c|cccccc"," Weighting ","Err","Corr",
			" Sub"," Del"," Ins"," Spl"," Mrg");
    Desc_add_row_separation('-',BEFORE_ROW);
    
    for (cfunc=0; cfunc < wwlscore->numfunc; cfunc++){ 
	WWL_FUNC *tfunc;
	tfunc = &(wwlscore->func[cfunc]);
	Desc_set_iterated_format("l|c|cccccc");
	Desc_set_iterated_value(rsprintf(" %s ",tfunc->title));
	Desc_set_iterated_value(rsprintf(" %6.2f ",
					 pct(tfunc->sub+tfunc->mrg+
					     tfunc->del+tfunc->ins+
					     tfunc->spl,tfunc->ref)));
	Desc_set_iterated_value(rsprintf(" %6.2f ",
					 pct(tfunc->corr,tfunc->ref)));
	Desc_set_iterated_value(rsprintf(ffmt,pct(tfunc->sub,tfunc->ref)));
	Desc_set_iterated_value(rsprintf(ffmt,pct(tfunc->del,tfunc->ref)));
	Desc_set_iterated_value(rsprintf(ffmt,pct(tfunc->ins,tfunc->ref)));
	Desc_set_iterated_value(rsprintf(ffmt,pct(tfunc->spl,tfunc->ref)));
	Desc_set_iterated_value(rsprintf(ffmt,pct(tfunc->mrg,tfunc->ref)));
	Desc_flush_iterated_row();
    }
    Desc_dump_report(0,fp);
}

static void make_speaker_summary(WWL_SCORE *wwlscore, FILE *fp)
{
    int s, cfunc;

    Desc_erase();
    Desc_set_page_center(SCREEN_WIDTH);
    Desc_add_row_values("c|l|c|cccccc","Spkr"," Weighting ","Err","Corr",
			" Sub"," Del"," Ins"," Spl"," Mrg");
    Desc_add_row_separation('-',BEFORE_ROW);
    
    for (s=0; s<wwlscore->func[0].n_spkrs; s++){
	for (cfunc=0; cfunc < wwlscore->numfunc; cfunc++){
	    WWL_FUNC *tfunc;
	    WWL_SPKR *tsp;
	    tfunc = &(wwlscore->func[cfunc]);
	    tsp = &(tfunc->spkr[s]);
	    Desc_set_iterated_format("c|l|c|cccccc");	
	    Desc_set_iterated_value(rsprintf(" %s ",tfunc->spkr[s].id));
	    Desc_set_iterated_value(rsprintf(" %s ",tfunc->title));
	    Desc_set_iterated_value(rsprintf(" %6.2f ",
					     pct(tsp->sub+tsp->mrg+
						 tsp->del+tsp->ins+
						 tsp->spl,tsp->ref)));
	    Desc_set_iterated_value(rsprintf(" %6.2f ",
					     pct(tsp->corr,tsp->ref)));
	    Desc_set_iterated_value(rsprintf("%6.2f ",pct(tsp->sub,tsp->ref)));
	    Desc_set_iterated_value(rsprintf("%6.2f ",pct(tsp->del,tsp->ref)));
	    Desc_set_iterated_value(rsprintf("%6.2f ",pct(tsp->ins,tsp->ref)));
	    Desc_set_iterated_value(rsprintf("%6.2f ",pct(tsp->spl,tsp->ref)));
	    Desc_set_iterated_value(rsprintf("%6.2f ",pct(tsp->mrg,tsp->ref)));
	    Desc_flush_iterated_row();
	}
	if (s < wwlscore->func[0].n_spkrs-1)
	    Desc_add_row_separation('-',BEFORE_ROW);
    }
    Desc_add_row_separation('=',BEFORE_ROW);
    for (cfunc=0; cfunc < wwlscore->numfunc; cfunc++){
	WWL_FUNC *tfunc;
	tfunc = &(wwlscore->func[cfunc]);

	Desc_set_iterated_format("c|l|c|cccccc");	
	Desc_set_iterated_value(rsprintf(" %s ","Ave"));
	Desc_set_iterated_value(rsprintf(" %s ",tfunc->title));
	Desc_set_iterated_value(rsprintf(" %6.2f ",
					 pct(tfunc->sub+tfunc->mrg+
					 tfunc->del+tfunc->ins+
					 tfunc->spl,tfunc->ref)));
	Desc_set_iterated_value(rsprintf(" %6.2f ",
					 pct(tfunc->corr,tfunc->ref)));
	Desc_set_iterated_value(rsprintf("%6.2f ",pct(tfunc->sub,tfunc->ref)));
	Desc_set_iterated_value(rsprintf("%6.2f ",pct(tfunc->del,tfunc->ref)));
	Desc_set_iterated_value(rsprintf("%6.2f ",pct(tfunc->ins,tfunc->ref)));
	Desc_set_iterated_value(rsprintf("%6.2f ",pct(tfunc->spl,tfunc->ref)));
	Desc_set_iterated_value(rsprintf("%6.2f ",pct(tfunc->mrg,tfunc->ref)));
	Desc_flush_iterated_row();
    }
    Desc_dump_report(0,fp);
}

double Weight_wwl(PCIND_T ind, TABLE_OF_CODESETS *pcs, WWL *wwl)
{
    double sum=0.0;
    char buffer[100], *p;

    if (strchr(pcs->cd[1].pc->pc[ind].ac, '=') == (char *)0)
	return (get_weight_from_WW_by_pcind(wwl, ind));

    strcpy(buffer,pcs->cd[1].pc->pc[ind].ac);
    /* Loop through each sub-word and compute the weight */
    /* printf("LOOK AT %s\n",buffer); */
    p = strtok(buffer,"=");
    while (p != NULL){
	/* printf("Computing weight for word '%s'\n",p); */

	ind = pcindex5(p,pcs->cd[1].pc,TRUE,FALSE);
	sum += get_weight_from_WW_by_pcind(wwl, ind);
	/* printf("      %6.2f\n",sum); */

	p = strtok(NULL,"=");
    }
    return(sum);
}


double Weight_one(PCIND_T ind, TABLE_OF_CODESETS *pcs, WWL *wwl)
{
    return (1.0);
}

double Weight_phones(PCIND_T ind, TABLE_OF_CODESETS *pcs, WWL *wwl)
{
    return((double)pcs->cd[1].pc->pc[ind].lc_int[0]);
}

double Weight_syllables(PCIND_T ind, TABLE_OF_CODESETS *pcs, WWL *wwl)
{
    int perr;
    int ret = nsylbs1(ind, pcs->cd[1].pc, &perr);

    if (perr > 0){
	fprintf(stderr,"Warning: nsylbs1 return'd error flag of %d,",ret);
	fprintf(stderr,"returning 1\n");
	ret=1;
    }
    return((double)ret);
}

int compare_WW(const void *ww1, const void *ww2)
{
    if ((*(WW **)ww1)->pcind < (*(WW **)ww2)->pcind)
	return(-1);
    if ((*(WW **)ww1)->pcind > (*(WW **)ww2)->pcind)
	return(1);
    return(0);
}

double get_weight_from_WW_by_pcind(WWL *wwl, PCIND_T ind)
{
    /* This implements a binary search */
    int low, high, mid, eval;
    
    low = 0, high = wwl->num-1;

    do { 
        mid = (low + high)/2;
        eval = (ind < wwl->words[mid]->pcind) ? -1 : 
	        ((ind > wwl->words[mid]->pcind) ? 1 : 0);
        if (eval == 0)
            return(wwl->words[mid]->weight[wwl->curw]);
        if (eval < 0)
            high = mid-1;
        else
            low = mid+1;
    } while (low <= high);

    return(wwl->default_weight);
}

void dump_WWL(WWL *wwl, TABLE_OF_CODESETS *pcs, FILE *fp)
{
    int i, w;
    fprintf(fp,"Dump of WWL\nFile name: %s\n",wwl->filename);
    for (w=0; w<wwl->num_w; w++)
	fprintf(fp,"   Desc: '%s'\n",wwl->weight_desc[w]);
    fprintf(fp,"Default Missing Weight: %e\n",wwl->default_weight);
    fprintf(fp,"      #: ");
    for (w=0; w<wwl->num_w; w++)
	fprintf(fp,"     Wgt     ");
    fprintf(fp,"   IND   String\n"); 
    for (i=0; i<wwl->num; i++){
	fprintf(fp,"   %4d: ",i);
	for (w=0; w<wwl->num_w; w++)
	    fprintf(fp,"%e ",wwl->words[i]->weight[w]);
	fprintf(fp,"  %5d  %s\n",wwl->words[i]->pcind,
		pcs->cd[1].pc->pc[wwl->words[i]->pcind].ac);
    }
/* 	fprintf(fp,"   %4d: %6.3f  %5d  %s\n",i,wwl->words[i]->weight,
		wwl->words[i]->pcind,wwl->words[i]->sp);
	fprintf(fp,"          weight %6.3f\n",
		get_weight_from_WW_by_pcind(wwl,wwl->words[i]->pcind));*/
    fprintf(fp,"\n\n");
}

void free_WWL(WWL **wwl)
{
    int w;

    free_2dimarr((*wwl)->words,(*wwl)->num,WW);
    mtrf_free((*wwl)->filename);
    for (w=0; w<(*wwl)->num_w; w++)
	mtrf_free((*wwl)->weight_desc[w]);
    free_singarr((*wwl),WWL);
}

int load_WWL(WWL **wwl, char *filename, TABLE_OF_CODESETS *pcs)
{
    FILE *fp;
    WWL *twwl;
    WW **twords;
    char buff[MAX_BUFF_LEN], word[MAX_BUFF_LEN];
    char weight[MAX_W][MAX_BUFF_LEN], *p, *e;
    int line=0, size, count_guess, w, ret, min_weight=10;

    /* Initialize the structur */
    alloc_singarr(twwl,1,WWL);
    twwl->max = twwl->num = twwl->num_w = 0;
    twwl->words = (WW **)0;
    twwl->default_weight = 0.0; 
    for (w=0; w<MAX_W; w++)
	twwl->weight_desc[w] = (char *)0;

    if (filename == (char *)0) {
        return 1;
    }
    if (*(filename) == NULL_CHAR) {
        return 1;
    }

    twwl->filename = mtrf_strdup(filename);

    if ((size = file_size(filename)) > 0)
	count_guess = size/8;
    else
	count_guess = 100;

    if ((fp=fopen(filename,"r")) == NULL){
        fprintf(stderr,"Warning: Could not open Word-weight file: %s",
		filename);
        fprintf(stderr," No words loaded ! ! ! !\n");
        return 1;
    }
    
    while (safe_fgets(buff,MAX_BUFF_LEN,fp) != NULL){
	line++;
        /* ignore comments */
        if (!is_comment(buff) && !is_empty(buff)){
	    if ((ret=sscanf(buff,"%s %s %s %s %s %s",word,weight[0], weight[1],
			    weight[2], weight[3], weight[4])) < 2){
		fprintf(stderr,
			"Error: Unable to parse line %d of WWF file '%s'\n",
			line,buff);
		return(1);
	    }
	    if (ret < min_weight) min_weight = ret;
	    if (ret > MAX_W+1) ret = MAX_W + 1;
	    if (twwl->num + 1 > twwl->max) {
		/* I need much more space, alloc a new one, then free */
		alloc_singZ(twords,count_guess,WW *,(WW *)0);
		memcpy(twords,twwl->words,sizeof(WW *) * twwl->num);
		if (twwl->words != (WW **)0) free_singarr(twwl->words,WW *);
		twwl->words = twords;

		twwl->max = count_guess;

		count_guess *= 1.5;    
	    }
	    alloc_singarr(twwl->words[twwl->num],1,WW);
	    for (w=0; w<ret-1; w++)
		twwl->words[twwl->num]->weight[w] = (double)atof(weight[w]);

	    adjust_type_case(word,pcs->cd[1].pc);
	    /* Look up the word, but don't save the text */
	    if ((twwl->words[twwl->num]->pcind = pcindex5(word,pcs->cd[1].pc,
							  TRUE,FALSE)) == -1){
		fprintf(stderr,"Error: Unable to lookup word %d '%s' in WWL\n",
			twwl->num,word);
		return (1);
	    }

	    twwl->num++;
	} else if ((p = strstr(buff,"'Headings'")) != (char *)NULL){
	    /* increment p by the appropriate length */
	    p += strlen("'Headings'");
	    /* skip over the 'Word Spelling' string */
	    if ((p = strstr(p,"'Word Spelling'")) == (char *)NULL){
		fprintf(stderr,"Warning: Unable to parse label line %s",buff);
		continue;
	    }
	    p += strlen("'Word Spelling'");
	    /* parse out the description field */
	    while ((p = strchr(p,'\'')) != NULL){
		if ((e = strchr(p+1,'\'')) != NULL){
		    alloc_singZ(twwl->weight_desc[twwl->num_w],
				e-p +1,char,'\0');
		    strncpy(twwl->weight_desc[twwl->num_w],p+1,e-p-1);
		    twwl->num_w ++;
		}
		p = e + 1;
	    }
	} else if ((p = strstr(buff,"Default missing weight"))!=(char *)NULL){
	    /* parse out the missing weight field */
	    if ((p = strchr(buff,'\'')) != NULL){
		if ((e = strchr(p+1,'\'')) != NULL){
		    strncpy(word,p+1,e-p-1);
		    twwl->default_weight = (double)atof(word);
		    continue;
		}
	    }
	    fprintf(stderr,"Warning: Failure to parse 'Default missing weight' %s",buff);
	}
    }
    fclose(fp);

    if (twwl->num_w == 0){
	if (min_weight < 2){
	    fprintf(stderr,"Error: No wieghts defined in WWL '%s'\n",
		    filename);
	    return(1);
	}
	twwl->num_w = min_weight-1;
    }

    for (w=0; w<MAX_W; w++)
	if (twwl->weight_desc[w] == (char *)0)
	    twwl->weight_desc[w] = mtrf_strdup(rsprintf("%s Col %d",
							filename,w+1));

    /* Sort the words by pcind */
    qsort(twwl->words,twwl->num,sizeof(WW *),compare_WW);

    *wwl = twwl;

    return (0);
}

void do_weighted_score(SYS_ALIGN *sys_align, TABLE_OF_CODESETS *pcs, WWL *wwl, int dbg, double (*Weight) (PCIND_T, TABLE_OF_CODESETS *, WWL *), WWL_FUNC *wwscf)
{
    int spkr, st, wd;
    double refW, corrW, subW, delW, insW, mrgW, splW;
    double SrefW, ScorrW, SsubW, SdelW, SinsW, SmrgW, SsplW; 
    double rW, hW;
    SENT *sent;

    wwscf->ref  =  wwscf->corr = wwscf->sub = wwscf->ins = wwscf->del = 0.0;
    wwscf->mrg  =  wwscf->spl  = 0.0;

    for (spkr=0; spkr < num_spkr(sys_align); spkr++){
	wwscf->spkr[spkr].id = mtrf_strdup(spkr_name(sys_align,spkr));
	SrefW = ScorrW = SsubW = SdelW = SinsW = SmrgW = SsplW = 0.0;
        for (st=0; st<num_sents(sys_align,spkr); st++){
	    refW = corrW = subW = delW = insW = mrgW = splW = 0.0;
	    sent = sent_n(sys_align,spkr,st);
	    if (dbg > 5)
		print_readable_n_SENT(pcs,1,sent,
				      (SENT *)0,(SENT *)0,(SENT *)0,(SENT *)0,
				      (SENT *)0,(SENT *)0,(SENT *)0,(SENT *)0,
				      (SENT *)0);

	    for (wd=0; s_eval_wrd(sent,wd) != END_OF_WORDS; wd++){
#ifdef toomuch
		if (is_INS(s_eval_wrd(sent,wd)))
		    printf(" %20s  %e","****",0.0);
		else
		    printf(" %20s  %e",lex_lc_word(pcs,s_ref_wrd(sent,wd)),
			   Weight(s_ref_wrd(sent,wd),pcs,wwl));

		if (is_DEL(s_eval_wrd(sent,wd)))
		    printf(" %20s  %e\n","****",0.0);
		else
		    printf(" %20s  %e\n",lex_lc_word(pcs,s_hyp_wrd(sent,wd)),
			   Weight(s_hyp_wrd(sent,wd),pcs,wwl));
#endif
		if (is_CORR(s_eval_wrd(sent,wd))){
		    refW  += Weight(s_ref_wrd(sent,wd),pcs,wwl);
		    corrW += Weight(s_ref_wrd(sent,wd),pcs,wwl);
		} else if (is_SUB(s_eval_wrd(sent,wd))){
		    rW     = Weight(s_ref_wrd(sent,wd),pcs,wwl);
		    hW     = Weight(s_hyp_wrd(sent,wd),pcs,wwl);
		    refW  += rW;
		    subW  += MAX(rW,hW);
		} else if (is_DEL(s_eval_wrd(sent,wd))){
		    refW  += Weight(s_ref_wrd(sent,wd),pcs,wwl);
		    delW  += Weight(s_ref_wrd(sent,wd),pcs,wwl);
		} else if (is_INS(s_eval_wrd(sent,wd))){
		    insW  += Weight(s_hyp_wrd(sent,wd),pcs,wwl);
		} else if (is_MERGE(s_eval_wrd(sent,wd))){
		    rW     = Weight(s_ref_wrd(sent,wd),pcs,wwl);
		    hW     = Weight(s_hyp_wrd(sent,wd),pcs,wwl);
		    refW  += rW;
		    mrgW  += MAX(rW,hW);
		} else if (is_SPLIT(s_eval_wrd(sent,wd))){
		    rW     = Weight(s_ref_wrd(sent,wd),pcs,wwl);
		    hW     = Weight(s_hyp_wrd(sent,wd),pcs,wwl);
		    refW  += rW;
		    splW  += MAX(rW,hW);
		} else {
		    fprintf(stderr,"Error: undefined evaluation %d word %d\n",
			    s_eval_wrd(sent,wd),wd);
		}
	    }
	    if (dbg > 4){
		printf("Sentence: R:%.2f  C:%.2f  S:%.2f  D:%.2f",
		       refW,corrW,subW,delW);
		printf("  I:%.2f  Mr:%.2f  Sp:%.2f\n",insW,mrgW,splW);
	    }
	    SrefW  += refW;  ScorrW += corrW;   SsubW  += subW;  SdelW += delW;
	    SinsW  += insW;  SmrgW  += mrgW;	SsplW  += splW;
	}
	if (dbg > 3){
	    printf("Speaker: R:%.2f  C:%.2f  S:%.2f  D:%.2f",
		   SrefW,ScorrW,SsubW,SdelW);
	    printf("  I:%.2f  Mr:%.2f  Sp:%.2f\n",SinsW,SmrgW,SsplW);
	}
	wwscf->ref  += SrefW;	wwscf->spkr[spkr].ref  = SrefW;
	wwscf->corr += ScorrW;	wwscf->spkr[spkr].corr = ScorrW;
	wwscf->sub  += SsubW;	wwscf->spkr[spkr].sub  = SsubW;
	wwscf->ins  += SinsW;	wwscf->spkr[spkr].ins  = SinsW;
	wwscf->del  += SdelW;	wwscf->spkr[spkr].del  = SdelW;
	wwscf->mrg  += SmrgW;	wwscf->spkr[spkr].mrg  = SmrgW;
	wwscf->spl  += SsplW;	wwscf->spkr[spkr].spl  = SsplW;
    }
    if (dbg > 2){
	printf("Total: R:%e  C:%e  S:%e  D:%e",
	       wwscf->ref, wwscf->corr, wwscf->sub,wwscf->del);
	printf("  I:%e  Mr:%e  Sp:%e\n",
	       wwscf->ins, wwscf->mrg, wwscf->spl);
    }
}

