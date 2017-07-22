/**********************************************************************/
/*                                                                    */
/*             FILENAME:  qscore.c                                    */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  These utilities can be called from any program    */
/*                  with a loaded alignment structure and codeset,    */
/*                  to produce summary scoring reports.               */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>


void print_score_for_sm_scoring(SYS_ALIGN *align_str, TABLE_OF_CODESETS *pcs, FILE *fp){
    int spkr, snt, wrd; 
    int cor, sub, del, ins, mrg, spl, nsm1, nsm2, nsm3, nsm;

    cor=sub=del=ins=mrg=spl=0;
    nsm1=nsm2=nsm3=0;

    for (spkr=0;spkr < num_spkr(align_str); spkr++){
	for (snt=0;snt < num_sents(align_str,spkr); snt++){
	    for (wrd = 0;ref_word(align_str,spkr,snt,wrd) !=
		 END_OF_WORDS; wrd ++){
		nsm = 0;
		if (is_CORR(eval_wrd(align_str,spkr,snt,wrd)))
		    cor++;
		else if (is_INS(eval_wrd(align_str,spkr,snt,wrd)))
		    ins++;
		else if (is_DEL(eval_wrd(align_str,spkr,snt,wrd)))
		    del++;
		else if (is_SUB(eval_wrd(align_str,spkr,snt,wrd)))
		    sub++;
		else if (is_MERGE(eval_wrd(align_str,spkr,snt,wrd))){
		    mrg++;
		    nsm = count_sms(pcs->cd[1].pc->pc[ref_word(align_str,spkr,snt,wrd)].ac);
		} else if (is_SPLIT(eval_wrd(align_str,spkr,snt,wrd))) {
		    spl++;
		    nsm = count_sms(pcs->cd[1].pc->pc[hyp_word(align_str,spkr,snt,wrd)].ac);
		} else {
		    fprintf(stderr,"Error: SM_COUNT unknown evaluation type %x\n",
			    eval_wrd(align_str,spkr,snt,wrd));
		    exit(-1);
		}
		switch (nsm){
		  case 0: break;
		  case 1: nsm1++; break;
		  case 2: nsm2++; break;
		  case 3: nsm3++; break;
		  default: fprintf(fp,"NSM = %d\n",nsm); break;
		}
	    }
	}
    }
    fprintf(fp,"Split/Merge rescoring\n");
    fprintf(fp,"        Reference Words = %d\n",sys_nref(align_str));
    fprintf(fp,"COUNT C=%6d  S=%6d  I=%6d  D=%6d  M=%6d  T=%6d  E=%6d NSM1=%d NSM2=%d NSM3=%d\n",
	   cor,sub,ins,del,mrg,spl,
	   sub+ins+del+mrg+spl, nsm1, nsm2, nsm3);
    fprintf(fp,"PCT   C=%6.3f  S=%6.3f  I=%6.3f  D=%6.3f  M=%6.3f  T=%6.3f  E=%6.3f\n",
	   pct(cor,sys_nref(align_str)),
	   pct(sub,sys_nref(align_str)),
	   pct(ins,sys_nref(align_str)),
	   pct(del,sys_nref(align_str)),
	   pct(mrg,sys_nref(align_str)),
	   pct(spl,sys_nref(align_str)),
	   pct(sub+ins+del+mrg+spl,sys_nref(align_str)));
}

int count_sms(char *str){
    char *p=str;
    int sum=0;
    /* printf("Count_sms  %s",str); */
    while ((p=(char *)strchr(p,'=')) != (char *)NULL)
	sum++,  p++;
    /* printf("  %d\n",sum); */
    return(sum);
}


/************************************************************************/
/*   print the report for the entire system.                            */
/************************************************************************/
void quick_print_overall_desc(SYS_ALIGN *align_str, char *sys_root_name, int do_sm, int do_raw)
{
    int spkr, snt, wrd, tot_corr=0, tot_sub=0, tot_del=0;
    int tmp, total_sents=0;
    FILE *fp;
    char *fname;

    /* added by Brett to compute mean, variance, and standard dev */
    double *corr_arr, *sub_arr, *del_arr, *ins_arr, *err_arr, *serr_arr;
    double *spl_arr, *mrg_arr;

    int tot_ins=0, tot_ref =0, tot_st=0, tot_st_er=0;
    int tot_spl=0, tot_mrg=0;

    char *units;

    double mean_corr, mean_del, mean_ins, mean_sub, mean_err, mean_serr;
    double mean_spl, mean_mrg;
    double var_corr, var_del, var_ins, var_sub, var_err, var_serr;
    double var_spl, var_mrg;
    double sd_corr, sd_del, sd_ins, sd_sub, sd_err, sd_serr;
    double sd_spl, sd_mrg, Z_stat;
    int *sent_num_arr;
    float mean_sent_num, var_sent_num, sd_sent_num, Z_stat_fl;
    SENT *tsent;
    char *pct_fmt, *tot_pct_fmt, *spkr_fmt=" %s ", *sent_fmt="%5d";
    
    if (!do_raw)
	pct_fmt=tot_pct_fmt="%5.1f ";
    else {
	pct_fmt="%5.0f ";
	tot_pct_fmt="%5.1f ";
    }

    /* open an output file, if it fails, write to stdout */
    if ((fp = fopen(fname=rsprintf("%s.%s",sys_root_name,
				   (do_raw) ? "raw" : "sys"),"w")) == NULL)
	fp = stdout;
    else
	printf("Writing %sscoring report to '%s'\n",
	       do_raw ? "raw " : "", fname);

    /* allocate memory */
    alloc_singZ(corr_arr,num_spkr(align_str),double,0.0);
    alloc_singZ(sub_arr,num_spkr(align_str),double,0.0);
    alloc_singZ(del_arr,num_spkr(align_str),double,0.0);
    alloc_singZ(ins_arr,num_spkr(align_str),double,0.0);
    alloc_singZ(err_arr,num_spkr(align_str),double,0.0);
    alloc_singZ(serr_arr,num_spkr(align_str),double,0.0);
    alloc_singZ(spl_arr,num_spkr(align_str),double,0.0);
    alloc_singZ(mrg_arr,num_spkr(align_str),double,0.0);
    alloc_singZ(sent_num_arr,num_spkr(align_str),int,0);


    for (spkr=0; spkr<num_spkr(align_str); spkr++){
	int Tcorr, Tins, Tdel, Tsub, Tmerg, Tsplt, Trefs, Tserr, serr;

	Tcorr=Tins=Tdel=Tsub=Tmerg=Tsplt=Tserr= 0;
        for (snt=0; snt < num_sents(align_str,spkr); snt++){
	    tsent = sent_n(align_str,spkr,snt);
	    for (wrd = 0,serr=0; s_ref_wrd(tsent,wrd) != END_OF_WORDS; wrd ++){
		if (is_CORR(s_eval_wrd(tsent,wrd)))
		    Tcorr++;
		else {
		    serr++;
		    if (is_INS(s_eval_wrd(tsent,wrd)))
			Tins++;
		    else if (is_DEL(s_eval_wrd(tsent,wrd)))
			Tdel++;
		    else if (is_SUB(s_eval_wrd(tsent,wrd)))
			Tsub++;
		    else if (is_MERGE(s_eval_wrd(tsent,wrd)))
			Tmerg++;	
		    else if (is_SPLIT(s_eval_wrd(tsent,wrd)))
			Tsplt++;	
		    else {
			fprintf(stderr,"Error: unknown evaluation type %x\n",
				s_eval_wrd(tsent,wrd));
			exit(-1);
		    }
		}
	    }
	    if (serr > 0) Tserr++;
	}
	Trefs = Tsub + Tcorr + Tdel + Tmerg + Tsplt;
	if (! do_raw){
	    corr_arr[spkr] = pct(Tcorr,Trefs);
	    sub_arr[spkr]  = pct(Tsub,Trefs);
	    ins_arr[spkr]  = pct(Tins,Trefs);
	    del_arr[spkr]  = pct(Tdel,Trefs);
	    mrg_arr[spkr]  = pct(Tmerg,Trefs);
	    spl_arr[spkr]  = pct(Tsplt,Trefs);
	    err_arr[spkr]  = pct(Tsub+Tins+Tdel+Tmerg+Tsplt,Trefs);
	    serr_arr[spkr] = pct(Tserr,num_sents(align_str,spkr));
	} else {
	    corr_arr[spkr] = Tcorr;
	    sub_arr[spkr]  = Tsub;
	    ins_arr[spkr]  = Tins;
	    del_arr[spkr]  = Tdel;
	    mrg_arr[spkr]  = Tmerg;
	    spl_arr[spkr]  = Tsplt;
	    err_arr[spkr]  = Tsub+Tins+Tdel+Tmerg+Tsplt;
	    serr_arr[spkr] = Tserr;
	}
	sent_num_arr[spkr]=num_sents(align_str,spkr);
	    
	tot_corr+=  Tcorr;
	tot_sub+=   Tsub;
	tot_del+=   Tdel;
	tot_ins+=   Tins;
	tot_ref+=   Trefs;
	tot_spl+=   Tsplt;
	tot_mrg+=   Tmerg;
	tot_st_er+= Tserr;
	tot_st+=    num_sents(align_str,spkr);
	total_sents+=num_sents(align_str,spkr);
    }
    
    fprintf(fp,"\n\n\n%s\n\n",
	    center("SYSTEM SUMMARY PERCENTAGES by SPEAKER",SCREEN_WIDTH));
    if (!do_sm && (tot_spl + tot_mrg) > 0)
	fprintf(fp,"\nWarning: Split and/or Merges found, but not reported\n");

    Desc_erase();
    Desc_set_page_center(SCREEN_WIDTH);
    Desc_add_row_values("c",sys_desc(align_str));
    Desc_add_row_separation('-',BEFORE_ROW);
    if (do_sm)
	Desc_add_row_values("l|c|cccccccc"," SPKR"," # Snt","Corr",
			    " Sub"," Del"," Ins"," Mrg"," Spl"," Err","S.Err");
    else
	Desc_add_row_values("l|c|cccccc"," SPKR"," # Snt","Corr",
			    " Sub"," Del"," Ins"," Err","S.Err");

    for (spkr=0; spkr<num_spkr(align_str); spkr++){
	Desc_add_row_separation('-',BEFORE_ROW);

	if (do_sm)
	    Desc_set_iterated_format("l|c|cccccccc");
	else
	    Desc_set_iterated_format("l|c|cccccc");
	Desc_set_iterated_value(rsprintf(spkr_fmt,spkr_name(align_str,spkr)));
	Desc_set_iterated_value(rsprintf(sent_fmt,sent_num_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,corr_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,sub_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,del_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,ins_arr[spkr]));
	if (do_sm){
	    Desc_set_iterated_value(rsprintf(pct_fmt,mrg_arr[spkr]));
	    Desc_set_iterated_value(rsprintf(pct_fmt,spl_arr[spkr]));
	}
	Desc_set_iterated_value(rsprintf(pct_fmt,err_arr[spkr]));
	Desc_set_iterated_value(rsprintf(pct_fmt,serr_arr[spkr]));
	Desc_flush_iterated_row();
    }
    Desc_add_row_separation('=',BEFORE_ROW); 
    if (do_sm)
	Desc_set_iterated_format("l|c|cccccccc");
    else
	Desc_set_iterated_format("l|c|cccccc");
    if (!do_raw){
	Desc_set_iterated_value(" Sum/Avg");
	Desc_set_iterated_value(rsprintf(sent_fmt,tot_st));
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,pct(tot_corr,tot_ref)));
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,pct(tot_sub,tot_ref)));
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,pct(tot_del,tot_ref)));
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,pct(tot_ins,tot_ref)));
	if (do_sm){
	    Desc_set_iterated_value(rsprintf(tot_pct_fmt,
					     pct(tot_mrg,tot_ref)));
	    Desc_set_iterated_value(rsprintf(tot_pct_fmt,
					     pct(tot_spl,tot_ref)));
	}
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,
					 pct(tot_sub + tot_ins + tot_del +
					     tot_spl + tot_mrg,tot_ref)));
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,pct(tot_st_er,tot_st)));
	Desc_flush_iterated_row();
    } else {
	Desc_set_iterated_value(" Sum");
	Desc_set_iterated_value(rsprintf(sent_fmt,tot_st));
	Desc_set_iterated_value(rsprintf(pct_fmt,(float)tot_corr));
	Desc_set_iterated_value(rsprintf(pct_fmt,(float)tot_sub));
	Desc_set_iterated_value(rsprintf(pct_fmt,(float)tot_del));
	Desc_set_iterated_value(rsprintf(pct_fmt,(float)tot_ins));
	if (do_sm){
	    Desc_set_iterated_value(rsprintf(pct_fmt,(float)tot_mrg));
	    Desc_set_iterated_value(rsprintf(pct_fmt,(float)tot_spl));
	}
	Desc_set_iterated_value(rsprintf(pct_fmt,(float)(tot_sub + tot_ins +
							 tot_del + tot_spl +
							 tot_mrg)));
	Desc_set_iterated_value(rsprintf(pct_fmt,(float)tot_st_er));
	Desc_flush_iterated_row();
    }

    Desc_add_row_separation('=',BEFORE_ROW); 
    /* added by Brett to compute mean, variance, and standard dev */
    calc_mean_var_std_dev_Zstat(sent_num_arr,num_spkr(align_str),
			&mean_sent_num,&var_sent_num,&sd_sent_num,&Z_stat_fl);
    calc_mean_var_std_dev_Zstat_double(corr_arr,num_spkr(align_str),
 		        &mean_corr,&var_corr,&sd_corr,&Z_stat);
    calc_mean_var_std_dev_Zstat_double(sub_arr,num_spkr(align_str),
			&mean_sub,&var_sub,&sd_sub,&Z_stat);
    calc_mean_var_std_dev_Zstat_double(ins_arr,num_spkr(align_str),
		        &mean_ins,&var_ins,&sd_ins,&Z_stat);
    calc_mean_var_std_dev_Zstat_double(del_arr,num_spkr(align_str),
			&mean_del,&var_del,&sd_del,&Z_stat);
    calc_mean_var_std_dev_Zstat_double(spl_arr,num_spkr(align_str),
			&mean_spl,&var_spl,&sd_spl,&Z_stat);
    calc_mean_var_std_dev_Zstat_double(mrg_arr,num_spkr(align_str),
			&mean_mrg,&var_mrg,&sd_mrg,&Z_stat);
    calc_mean_var_std_dev_Zstat_double(err_arr,num_spkr(align_str),
			&mean_err,&var_err,&sd_err,&Z_stat);
    calc_mean_var_std_dev_Zstat_double(serr_arr,num_spkr(align_str),
		        &mean_serr,&var_serr,&sd_serr,&Z_stat);

    if (do_sm)
	Desc_set_iterated_format("l|c|cccccccc");
    else
	Desc_set_iterated_format("l|c|cccccc");
    Desc_set_iterated_value(" Mean");
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,mean_sent_num));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,mean_corr));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,mean_sub));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,mean_del));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,mean_ins));
    if (do_sm){
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,mean_spl));
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,mean_mrg));
    }
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,mean_err));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,mean_serr));
    Desc_flush_iterated_row();

    if (do_sm)
	Desc_set_iterated_format("l|c|cccccccc");
    else
	Desc_set_iterated_format("l|c|cccccc");
    Desc_set_iterated_value(" S.D.");
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,sd_sent_num));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,sd_corr));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,sd_sub));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,sd_del));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,sd_ins));
    if (do_sm){
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,sd_spl));
	Desc_set_iterated_value(rsprintf(tot_pct_fmt,sd_mrg));
    }
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,sd_err));
    Desc_set_iterated_value(rsprintf(tot_pct_fmt,sd_serr));
    Desc_flush_iterated_row();
    Desc_dump_report(0,fp);

    if (do_raw)
	units="";
    else
	units="%";

    fprintf(fp,"\n\n");
    /* calc_two_sample_z_test_double(serr_arr,err_arr,num_spkr(align_str),
       num_spkr(align_str),&Z_stat);*/
    fprintf(fp,"For this set of %d speakers and %d sentences, the mean\n",
	    num_spkr(align_str),total_sents);
    fprintf(fp,"number of sentences per speaker was %d with a standard\n",
	    (int)mean_sent_num);
    fprintf(fp,"uncertainty of %d.  The \'%s\' system had a mean word\n",
	    (int)sd_sent_num,sys_root_name);
    fprintf(fp,"error of %.1f%s with a standard uncertainty of %.1f%%,\n",
	    mean_err,units,sd_err);
    fprintf(fp,"and a mean sentence error of %.1f%s with a standard\n",
	    mean_serr,units);
    fprintf(fp,"uncertainty of %.1f%s.  A list of speakers out of the\n",
	    sd_serr,units);
    fprintf(fp,"expanded uncertainty range is given below.\n");
    fprintf(fp,"\n");

    fprintf(fp,"Speakers with an unusually LOW number of sentences:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (num_sents(align_str,tmp) < (int)(mean_sent_num-(2*sd_sent_num)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");
    fprintf(fp,"Speakers with an unusually HIGH number of sentences:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (num_sents(align_str,tmp) > (int)(mean_sent_num+(2*sd_sent_num)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n\n");

    fprintf(fp,"Speakers with an unusually LOW word error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (err_arr[tmp] < (mean_err-(2*sd_err)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");
    fprintf(fp,"Speakers with an unusually HIGH word error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (err_arr[tmp] > (mean_err+(2*sd_err)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n\n");

    fprintf(fp,"Speakers with an unusually LOW sentence error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (serr_arr[tmp] < (mean_serr-(2*sd_serr)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");
    fprintf(fp,"Speakers with an unusually HIGH sentence error rate:");
    for (tmp=0; tmp<num_spkr(align_str); tmp++)
      if (serr_arr[tmp] > (mean_serr+(2*sd_serr)))
	fprintf(fp," %s",spkr_name(align_str,tmp));
    fprintf(fp,"\n");

    if (fp != stdout)
       fclose(fp);

    free_singarr(corr_arr,double);
    free_singarr(sub_arr,double);
    free_singarr(del_arr,double);
    free_singarr(ins_arr,double);
    free_singarr(err_arr,double);
    free_singarr(serr_arr,double);
    free_singarr(spl_arr,double);
    free_singarr(mrg_arr,double);
    free_singarr(sent_num_arr,int);
}
