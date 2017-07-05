/**********************************************************************/
/*                                                                    */
/*           FILE: rank_util.c                                        */
/*           WRITTEN BY: Jonathan G. Fiscus                           */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           USAGE: for general procedures to manipulate a RANK       */
/*                  structure                                         */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/**********************************************************************/
/*   initialize and allocate memory for the RANK structure from the   */
/*   SYS_ALIGN_LIST                                                   */
/**********************************************************************/
void init_RANK_struct_from_SA_LIST(RANK *rank, SYS_ALIGN_LIST *sa_list)
{
    int blk=0,trt,i,n;

    /* set it to the max number of speakers for all systems */
    for (n=0; n<num_sys(sa_list); n++)
	if (blk < num_spkr(sys_i(sa_list,n)))
	    blk = num_spkr(sys_i(sa_list,n));
    trt = num_sys(sa_list);

    rnk_blks(rank) = blk;
    rnk_trt(rank) = trt;
    alloc_char_2dimarr(rnk_b_name(rank),blk,MAX_SPEAKER_NAME_LEN+1);
    /* copy in only what's available from the first system */
    for (i=0; i<blk; i++)
	if (i < num_spkr(sys_i(sa_list,0)))
	    strcpy(Vrnk_b_name(rank,i),spkr_name(sys_i(sa_list,0),i));
    alloc_char_2dimarr(rnk_t_name(rank),trt,MAX_NAME_LEN+1);
    for (i=0; i<trt; i++){
        strcpy(Vrnk_t_name(rank,i),sys_name(sys_i(sa_list,i)));
    }
    alloc_float_2dimarr_zero(rnk_t_rank(rank),blk,trt);
    alloc_float_2dimarr_zero(rnk_b_rank(rank),blk,trt);
    alloc_float_2dimarr(rnk_pcts(rank),blk,trt);
    alloc_float_singarr(ovr_t_rank(rank),trt);
    alloc_float_singarr(ovr_b_rank(rank),blk);
    alloc_int_singarr(srt_t_rank(rank),trt);
    alloc_int_singarr(srt_b_rank(rank),blk);
}

/**********************************************************************/
/*   given the calculation formula, total the COUNT structure and     */
/*   compute the percentages                                          */
/**********************************************************************/
void calc_rank_pct(RANK *rank, COUNT ****count, SYS_ALIGN_LIST *sa_list, int calc_formula)
{
    int d,c,i,s,sys,spkr,snt;

    fprintf(stderr,"Calculating rank pcts.  Formula %c\n",calc_formula);
    for (sys=0;sys<num_sys(sa_list);sys++){
        for (spkr=0;spkr < num_spkr(sys_i(sa_list,sys)); spkr++){
            d = c = i = s = 0;
            /**** total the amount for this speaker and system */
            for (snt=0;snt < num_sents(sys_i(sa_list,sys),spkr); snt++){
                d += del(count,sys,spkr,snt);
                c += corr(count,sys,spkr,snt);
                i += inserts(count,sys,spkr,snt);
                s += subs(count,sys,spkr,snt);
	    }
            switch (calc_formula) {
                case PER_CORR_REC:
                         /***************************************/
                         /*          # corr rec                 */
                         /*      ------------------------------ */
                         /*       #words in ref sent            */
                         /***************************************/
                    Vrnk_pcts(rank,spkr,sys) = pct(c,(c + d + s));
                    break;
                case TOTAL_ERROR:
                         /***************************************/
                         /*          # errors                   */
                         /*      ------------------------------ */
                         /*       #words in ref sent            */
                         /***************************************/
                    Vrnk_pcts(rank,spkr,sys) = pct((i + s + d),(c + d + s));
                    break;
                case WORD_ACCURACY:
                         /***************************************/
                         /*          # errors                   */
                         /* 100- ------------------------------ */
                         /*       #words in ref sent            */
                         /***************************************/
                    Vrnk_pcts(rank,spkr,sys) = 100.0000 - 
                              pct((i + s + d),(c + d + s));
                    break;
                default:
                    fprintf(stderr,"You did not give calc_pct a formula\n");
                    break;
	    }
	}
    }

    if (calc_formula == TOTAL_ERROR)
        rank_on_pcts(rank,INCREASING);
    else
        rank_on_pcts(rank,DECREASING);

}
/**********************************************************************/
/*  given the RANK structure rank the percentages across blocks and   */
/*  treatments                                                        */
/**********************************************************************/
void rank_on_pcts(RANK *rank, int ordering)
{
    int t, b;
    int *t_ptr_arr;
    float *b_pcts, *b_ranks, *t_pcts, *t_ranks;

    alloc_singZ(t_ptr_arr,rnk_trt(rank),int,0);
    alloc_singZ(b_pcts,rnk_blks(rank),float,0.0);
    alloc_singZ(b_ranks,rnk_blks(rank),float,0.0);
    alloc_singZ(t_pcts,rnk_trt(rank),float,0.0);
    alloc_singZ(t_ranks,rnk_trt(rank),float,0.0);

    /**** ranks the blocks */
    /**** i.e. rank this speaker's results over all the systems */
    for (b=0;b<rnk_blks(rank);b++){
        rank_float_arr(rnk_pcts_arr(rank,b),
                       rnk_trt(rank),
                       t_ptr_arr,
                       rnk_t_rank_arr(rank,b),
                       ordering);
    }

    /****  rank the treatments */
    /****  i.e. rank this systems speakers */	
    for (t=0;t<rnk_trt(rank);t++){
        /**** since treatment pcts are the same index into n arrays, */
        /**** use a temporary array                                  */
        for (b=0;b<rnk_blks(rank);b++)
            b_pcts[b] = Vrnk_pcts(rank,b,t);

        rank_float_arr(b_pcts,
                       rnk_blks(rank),
                       srt_b_rank(rank),
                       b_ranks,
                       ordering);
        for (b=0;b<rnk_blks(rank);b++){
            Vrnk_b_rank(rank,b,t) = b_ranks[b];
	}
    }

    /*  rank the systems in an overall sense */
    for (b=0;b<rnk_blks(rank);b++)
        b_pcts[b] = 0.0;
    for (t=0; t<rnk_trt(rank); t++)
        t_pcts[t] = 0.0;

    for (t=0; t<rnk_trt(rank); t++)
        for (b=0;b<rnk_blks(rank);b++){
            t_pcts[t] += Vrnk_pcts(rank,b,t);
            b_pcts[b] += Vrnk_pcts(rank,b,t);
	}

    rank_float_arr(t_pcts,rnk_trt(rank),srt_t_rank(rank),t_ranks,ordering);
    rank_float_arr(b_pcts,rnk_blks(rank),srt_b_rank(rank),b_ranks,ordering);

    free_singarr(b_ranks,float)
    free_singarr(b_pcts,float)
    free_singarr(t_ranks,float)
    free_singarr(t_pcts,float)
    free_singarr(t_ptr_arr,int)
}

/**********************************************************************/
/*   print to stdout the full rank structure                          */
/**********************************************************************/
void dump_full_RANK_struct(RANK *rank, char *t_name, char *b_name, char *blk_label, char *trt_label, char *formula_str, char *test_name, char *blk_desc, char *trt_desc)
{
    char *title 	= "RANKING TABLE BY PERCENTAGES FOR THE TEST";
    char *title_2_a	= "Showing: ";
/*    char *title_3	= "         System ranks for the speaker";
    char *title_4	= "         Speaker ranks for the system";*/
    float sum_trt_ranks[MAX_TREATMENTS], sum_trt_pct[MAX_TREATMENTS];
    float sum_blk_pct, sum_blk_ranks;
    int i,j,k, *t_ind, *b_ind;
    char pad[FULL_SCREEN], pad1[FULL_SCREEN];
    int max_rank_len=6, tmp, max_len;
    char rank_format[20];

    /* zero out the array to total the treatments */
    for (i=0;i<rnk_trt(rank);i++){
       sum_trt_pct[i] = 0.0;
       sum_trt_ranks[i] = 0.0;
    }
    for (j=0;j<rnk_blks(rank);j++)
        if ((tmp=strlen(Vrnk_b_name(rank,j))) > max_rank_len)
            max_rank_len = tmp;
    max_rank_len++;
    sprintf(rank_format,"%%-%1ds",max_rank_len);

    /* use temporary pointers to point to the sorted indexes arrays */
    t_ind = srt_t_rank(rank);
    b_ind = srt_b_rank(rank);

/* print the title */
    set_pad(pad,title);
    printf("\n\n%s%s\n",pad,title);
    if (*test_name != NULL_CHAR){
       set_pad(pad,test_name);
       printf("%s%s\n",pad,test_name);
    }
    max_len = strlen(formula_str);
    if (strlen(blk_desc) > max_len) max_len = strlen(blk_desc);
    if (strlen(trt_desc) > max_len) max_len = strlen(trt_desc);
    set_pad_cent_n(pad,strlen(title_2_a) + max_len);
    set_pad_n(pad1,strlen(title_2_a));
    printf("%s%s%s\n",pad,title_2_a,formula_str);
    printf("%s%s%s\n",pad,pad1,blk_desc);
    printf("%s%s%s\n\n",pad,pad1,trt_desc);

/*    set_pad(pad,title);
    printf("\n\n%s%s\n",pad,title);
    if (*test_name != NULL_CHAR){
       set_pad(pad,test_name);
       printf("%s%s\n",pad,test_name);
    }
    set_pad(pad,title_4);
    printf("%s%s%s\n",pad,title_2_a,formula_str);
    printf("%s%s\n",pad,title_3);
    printf("%s%s\n\n",pad,title_4);
*/
/* top line */
    set_pad_cent_n(pad,rnk_trt(rank)*8 + (12+max_rank_len) + 11);
    printf("%s%s",pad,"|----------");
    for (i=0;i<max_rank_len;i++)
        printf("-");
    for (i=0;i<rnk_trt(rank);i++)
        printf("--------");
    printf("-----------|\n");
/* second line */
    printf("%s|\\ %-8s",pad,t_name);
    for (i=0;i<max_rank_len;i++)
        printf(" ");
    printf("|");
    for (i=0;i<rnk_trt(rank);i++)
        printf("       |");
    printf("| Av. pct |\n");
/* third line */
    printf("%s%s",pad,"| \\______");
    for (i=0;i<max_rank_len;i++)
        printf("_");
    printf("  |");
    for (i=0;i<rnk_trt(rank);i++){
        printf("  ");
        print_substr(stdout,5,Vrnk_t_name(rank,t_ind[i]));
        printf("|");
    }
    printf("|         |\n");
/* fourth line */
    printf("%s%s",pad,"|        ");
    for (i=0;i<max_rank_len;i++)
        printf(" ");
    printf("\\ |");
    for (i=0;i<rnk_trt(rank);i++)
        printf("       |");
    printf("|         |\n");
/* fifth line */
    printf("%s| %-8s",pad,b_name);
    for (i=0;i<max_rank_len;i++)
        printf(" ");
    printf("\\|");
    for (i=0;i<rnk_trt(rank);i++)
        printf("       |");
    printf("|Av Spkr R|\n");
/* print the blocks and treatments */
    for (i=0;i<rnk_blks(rank);i++){
      /* separation line */
        printf("%s%s",pad,"|----------");
        for (k=0;k<max_rank_len;k++)
            printf("-");
        for (k=0;k<rnk_trt(rank);k++)
            printf("+-------");
        printf("++---------|\n");
      /* print percent correct */
        printf("%s| ",pad);
        for (k=0;k<max_rank_len;k++)
            printf(" ");
        printf("Percent  ");
        sum_blk_pct = 0;
        for (j=0;j<rnk_trt(rank);j++){
            printf("|%5.1f%% ",Vrnk_pcts(rank,b_ind[i],t_ind[j]));
            sum_trt_pct[t_ind[j]] += Vrnk_pcts(rank,b_ind[i],t_ind[j]);
            sum_blk_pct += Vrnk_pcts(rank,b_ind[i],t_ind[j]);
	}
        printf("|| %5.1f%%  |\n",sum_blk_pct/(float)rnk_trt(rank));
      /* print System ranks  */
        printf("%s| ",pad);
        printf(rank_format,Vrnk_b_name(rank,b_ind[i]));
        printf("Sys rnk  ");
        for (j=0;j<rnk_trt(rank);j++){
            sum_trt_ranks[t_ind[j]] += Vrnk_t_rank(rank,b_ind[i],t_ind[j]);
            printf("|  %3.1f  ",Vrnk_t_rank(rank,b_ind[i],t_ind[j]));
	}
        printf("||         |\n");
      /* print speaker ranks  */
        printf("%s| ",pad);
        for (k=0;k<max_rank_len;k++)
            printf(" ");
        printf("Spkr rnk ");
        sum_blk_ranks = 0;
        for (j=0;j<rnk_trt(rank);j++){
            printf("|%5.1f  ",Vrnk_b_rank(rank,b_ind[i],t_ind[j]));
            sum_blk_ranks += Vrnk_b_rank(rank,b_ind[i],t_ind[j]);
	}
        printf("||  %5.1f  |\n",sum_blk_ranks/(float)rnk_trt(rank));

    }
/* total line */
    printf("%s%s",pad,"|==========");
    for (k=0;k<max_rank_len;k++)
        printf("=");
    for (i=0;i<rnk_trt(rank);i++)
        printf("========");
    printf("===========|\n");
/* percent average lines */
    printf("%s| Ave pcts ",pad);
    for (k=0;k<max_rank_len;k++)
        printf(" ");
    for (j=0;j<rnk_trt(rank);j++)
        printf("|%5.1f%% ",sum_trt_pct[t_ind[j]]/(float)rnk_blks(rank));
    printf("||         |\n");
/* print ave ranks for the treatments */
    printf("%s| Ave ranks",pad);
    for (k=0;k<max_rank_len;k++)
        printf(" ");
    for (j=0;j<rnk_trt(rank);j++)
        printf("|  %3.1f  ",sum_trt_ranks[t_ind[j]]/(float)rnk_blks(rank));
    printf("||         |\n");
/* print a blank line to match the format of squares */
    printf("%s|          ",pad);
    for (k=0;k<max_rank_len;k++)
        printf(" ");
    for (j=0;j<rnk_trt(rank);j++)
        printf("|       ");
    printf("||         |\n");
/* bottom line */
    printf("%s%s",pad,"|----------");
    for (k=0;k<max_rank_len;k++)
        printf("-");
    for (i=0;i<rnk_trt(rank);i++)
        printf("--------");
    printf("-----------|\n");

    form_feed();
}

void print_n_winner_comp_matrix(RANK *rank, int ***wins, char **win_ids, int win_cnt, int page_width)
{

    int report_width, max_name_len=0, len_per_trt, trt, trt2, i, wid;
    int max_win_ids_len=0;
    char sysname_fmt[40], statname_fmt[40], sysname_fmt_center[40];

    /* find the largest treat length */
    for (trt=0;trt<rnk_trt(rank);trt++)
      if ((i=strlen(Vrnk_t_name(rank,trt))) > max_name_len)
         max_name_len = i;
    /* find the largest id length */
    for (wid=0; wid<win_cnt; wid++)
      if ((i=strlen(win_ids[wid])) > max_win_ids_len)
         max_win_ids_len=i;
    wid=0;
    sprintf(statname_fmt," %%%ds",max_win_ids_len);
    sprintf(sysname_fmt_center," %%%ds |",max_name_len+1+max_win_ids_len);
    sprintf(sysname_fmt," %%%ds |",max_name_len);

  
    len_per_trt = 3 + max_name_len + 1 + max_win_ids_len;
    report_width=(rnk_trt(rank)+1) * len_per_trt + 1; 
     

    /* begin to print the report matrix */
    /* just a line */
    printf("%s",center("",(page_width-report_width)/2));
    printf("|");
    for (i=0; i<len_per_trt * (rnk_trt(rank)+1) - 1; i++)
        printf("-");
    printf("|\n");
    /* the treatment titles */
    printf("%s",center("",(page_width-report_width)/2));
    printf("|");
    printf(sysname_fmt_center,"");
    for (trt=0; trt<rnk_trt(rank); trt++){
        printf(sysname_fmt_center,center(Vrnk_t_name(rank,trt),len_per_trt-3));
    }
    printf("\n");
    for (trt2=0; trt2<rnk_trt(rank); trt2++){
        /* spacer lines */
        printf("%s",center("",(page_width-report_width)/2));
	printf("|");
        for (trt=0; trt<rnk_trt(rank)+1; trt++){
            for (i=0; i<len_per_trt - 1; i++)
                printf("-");
            if (trt < rnk_trt(rank))
                printf("+");
        }
        printf("|\n");
        for (wid=0; wid<win_cnt; wid++){
            printf("%s",center("",(page_width-report_width)/2));
            printf("|");
            if (wid == 0)
                printf(sysname_fmt_center,center(Vrnk_t_name(rank,trt2),len_per_trt-3));
            else
                printf(sysname_fmt_center,"");
            for (trt=0; trt<rnk_trt(rank); trt++){
                if (trt2 >= trt)
                    printf(statname_fmt,"");
                else
                    printf(statname_fmt,win_ids[wid]);
 
                if (trt2 >= trt)
                    printf(sysname_fmt,"");
                else if (wins[wid][trt2][trt] == NO_DIFF)
                    printf(sysname_fmt,"same");
                else if (wins[wid][trt2][trt] < 0)
                    printf(sysname_fmt,Vrnk_t_name(rank,trt2));
                else
                    printf(sysname_fmt,Vrnk_t_name(rank,trt));
            }
            printf("\n");
        }
    }   
    /* just a line */
    printf("%s",center("",(page_width-report_width)/2));
    printf("|");
    for (i=0; i<len_per_trt * (rnk_trt(rank)+1) - 1; i++)
        printf("-");
    printf("|\n");
    form_feed();

}

void print_speaker_stats(SYS_ALIGN_LIST *sa_list, int num_spkrs, int **speaker_matrix, char *title)
{
  int i,j,max_spkr_len=4,max_sys_len=5;
  char s1[200], s2[200],sheep[200],goat[200],same[200];
  printf("\n\n%s\n\n",title);
  for (i=0; i < num_spkrs; i++)
    if (strlen(spkr_name(sys_i(sa_list,1),i)) > max_spkr_len)
      max_spkr_len=strlen(spkr_name(sys_i(sa_list,1),i));
  for (i=0; i < num_sys(sa_list); i++)
    if (strlen(sys_name(sys_i(sa_list,i))) > max_sys_len)
      max_sys_len=strlen(sys_name(sys_i(sa_list,i)));
  i=(max_sys_len-5);
  if (i == 0)
    {
      strcpy(sheep,"  sheep |");
      strcpy(goat,"  goat  |");
      strcpy(same,"        |");
    }
  else
    {
      strcpy(sheep,"  sheep");
      strcpy(goat,"  goat");
      strcpy(same,"      ");
      while (i > 0)
	{
	  strcat(sheep," ");
	  strcat(goat," ");
	  strcat(same," ");
	}
      strcat(sheep,"|");
      strcat(goat,"|");
      strcat(same,"|");
    }
  strcpy(s1,"|");
  for (i=0; i < (max_spkr_len+1)+(num_sys(sa_list)*(max_sys_len+4)); i++)
    strcat(s1,"-");
  strcat(s1,"|");
  printf("%s\n|",s1);
  for (i=0; i < max_spkr_len+1; i++)
    printf(" ");
  printf("|");
  for (i=0; i < num_sys(sa_list); i++)
    {
      j=(max_sys_len-strlen(sys_name(sys_i(sa_list,i))));
      if (j == 0)
	printf("  %s |",sys_name(sys_i(sa_list,i)));
      else
	{
	  printf("  %s",sys_name(sys_i(sa_list,i)));
	  while (j > 0)
	    {
	      printf(" ");
	      --j;
	    }
	  printf(" |");
	}
    }
  printf("\n");
  strcpy(s2,"|");
  for (i=0; i < max_spkr_len+1; i++)
    strcat(s2,"-");
  strcat(s2,"+");
  for (i=0; i < num_sys(sa_list); i++)
    {
      for (j=0; j < max_sys_len+3; j++)
	strcat(s2,"-");
      if (i == num_sys(sa_list)-1)
	strcat(s2,"|");
      else
	strcat(s2,"+");
    }
  printf("%s\n",s2);
  for (i=0; i < num_spkrs; i++)
    {
      j=(max_spkr_len-strlen(spkr_name(sys_i(sa_list,1),i)));
      if (j == 0)
	printf("|%s |",spkr_name(sys_i(sa_list,1),i));
      else
	{
	  printf("|%s",spkr_name(sys_i(sa_list,1),i));
	  while (j > 0)
	    {
	      printf(" ");
	      --j;
	    }
	  printf(" |");
	}
      for (j=0; j < num_sys(sa_list); j++)
	{
	  if (speaker_matrix[i][j] == -1)
	    printf("%s",goat);
	  else
	    if (speaker_matrix[i][j] == 0)
	      printf("%s",same);
	    else
	      printf("%s",sheep);
	}
      if (i == num_spkrs-1)
	printf("\n%s\n",s1);
      else
	printf("\n%s\n",s2);
    }
}

void print_composite_significance(RANK *rank, int pr_width, int num_win, int **w1, char *desc1, char *str1, int **w2, char *desc2, char *str2, int **w3, char *desc3, char *str3, int **w4, char *desc4, char *str4, int **w5, char *desc5, char *str5, int **w6, char *desc6, char *str6, int matrix, int report, char *test_name)
{
    int **wins[40];
    char *win_str[40];
    char *win_desc[40], title[200];
    int i,max_desc_len=9,max_str_len=7,wid;

    wins[0] = w1; wins[1] = w2; wins[2] = w3; wins[3] = w4; wins[4] = w5; wins[5] = w6;
    win_str[0] = str1;    win_str[1] = str2;
    win_str[2] = str3;    win_str[3] = str4;
    win_str[4] = str5;    win_str[5] = str6;
    win_desc[0] = desc1;    win_desc[1] = desc2;
    win_desc[2] = desc3;    win_desc[3] = desc4;
    win_desc[4] = desc5;    win_desc[5] = desc6;
    for (wid=0; wid<num_win; wid++){
        if ((i=strlen(win_str[wid])) > max_str_len)
            max_str_len=i;
        if ((i=strlen(win_desc[wid])) > max_desc_len)
            max_desc_len=i;
    }

    if (matrix){
        printf("%s\n",center("Composite Report of All Significance Tests",pr_width));
        sprintf(title,"For the %s Test",test_name);
        printf("%s\n\n",center(title,pr_width));
        printf("%s",center("",(pr_width - (max_desc_len+5+max_str_len)) / 2));
        printf("%s",center("Test Name",max_desc_len));
        printf("%s",center("",5));
        printf("%s\n",center("Abbrev.",max_str_len));
        /* a line of dashes */
        printf("%s",center("",(pr_width - (max_desc_len+5+max_str_len)) / 2));
        for (i=0; i<max_desc_len; i++)
            printf("-");
        printf("%s",center("",5));
        for (i=0; i<max_str_len; i++)
            printf("-");
        printf("\n");
        for (wid=0; wid<num_win; wid++){
            printf("%s",center("",(pr_width - (max_desc_len+5+max_str_len)) / 2));
            printf("%s",center(win_desc[wid],max_desc_len));
            printf("%s",center("",5));
            printf("%s\n",center(win_str[wid],max_str_len));
        }
        printf("\n\n");
        print_n_winner_comp_matrix(rank,wins,win_str,num_win,pr_width);
    }
    if (report)
        print_n_winner_comp_report(rank,wins,win_str,win_str,win_desc,num_win,pr_width,test_name);
}

void print_n_winner_comp_report(RANK *rank, int ***wins, char **win_ids, char **win_str, char **win_desc, int win_cnt, int page_width, char *testname)
{

    int max_name_len=0, trt1, trt2, trt, t1, t2, i, wid;
    int max_win_ids_len=0, max_str_len=7, max_desc_len=0, pr_width=79;
    char sysname_fmt[40], title_line[200], fname[200];
    FILE *fp;

    /* find the largest treat length */
    for (trt=0;trt<rnk_trt(rank);trt++)
      if ((i=strlen(Vrnk_t_name(rank,trt))) > max_name_len)
         max_name_len = i;
    /* find the largest id length */
    for (wid=0; wid<win_cnt; wid++){
      if ((i=strlen(win_ids[wid])) > max_win_ids_len)
         max_win_ids_len=i;
      if ((i=strlen(win_str[wid])) > max_str_len)
         max_str_len=i;
      if ((i=strlen(win_desc[wid])) > max_desc_len)
          max_desc_len=i;
    }
    wid=0;
    sprintf(sysname_fmt," %%%ds ",max_name_len);
    pr_width = (max_name_len + 2) * (win_cnt + 1);
    sprintf(title_line,"for the %s Test with other Systems:",testname);
    pr_width = MAX(pr_width,strlen(title_line));
    pr_width = MAX(pr_width,max_desc_len+5+max_str_len);

    printf("\n");
    for (trt1=0; trt1<rnk_trt(rank); trt1++){
        sprintf(fname,"%s.hyp.sts",Vrnk_t_name(rank,trt1));
        if ((fp=fopen(fname,"w")) == (FILE *)0){
            fprintf(stderr,"Warning: Unable to open output file %s, using stdout\n",fname);
            fp=stdout;
        }
        sprintf(title_line,"Summary of Statistical Significance Tests:");
        fprintf(fp,"%s\n",center(title_line,pr_width));
        sprintf(title_line,"Comparing the System");
        fprintf(fp,"%s\n",center(title_line,pr_width));
        sprintf(title_line,"%s",Vrnk_t_name(rank,trt1));
        fprintf(fp,"%s\n",center(title_line,pr_width));
        sprintf(title_line,"for the %s Test with other Systems:",testname);
        fprintf(fp,"%s\n\n",center(title_line,pr_width));
       
        fprintf(fp,sysname_fmt,"");
        for (wid=0; wid<win_cnt; wid++)
            fprintf(fp,sysname_fmt,center(win_str[wid],max_name_len));
        fprintf(fp,"\n");
        fprintf(fp,sysname_fmt,"");
        for (wid=0; wid<win_cnt; wid++){
            fprintf(fp," ");
            for (i=0; i<max_name_len; i++)
                fprintf(fp,"-");
            fprintf(fp," ");
        }
        fprintf(fp,"\n");        
        for (trt2=0; trt2<rnk_trt(rank); trt2++){
            if (trt1 == trt2)
                continue;
            fprintf(fp,sysname_fmt,Vrnk_t_name(rank,trt2));
            for (wid=0; wid<win_cnt; wid++){
                if (trt1 < trt2)
                    t1 = trt1, t2 = trt2;
                else
                    t1 = trt2, t2 = trt1;
                if (wins[wid][t1][t2] == NO_DIFF)
                    fprintf(fp,sysname_fmt,center("same",max_name_len));
                else if (wins[wid][t1][t2] < 0)
                    fprintf(fp,sysname_fmt,center(Vrnk_t_name(rank,t1),max_name_len));
                else
                    fprintf(fp,sysname_fmt,center(Vrnk_t_name(rank,t2),max_name_len));
            }
            fprintf(fp,"\n");
	}
        fprintf(fp,"\n\n\n");
        fprintf(fp,"%s",center("",(pr_width - (max_desc_len+5+max_str_len)) / 2));
        fprintf(fp,"%s",center("Test Name",max_desc_len));
        fprintf(fp,"%s",center("",5));
        fprintf(fp,"%s\n",center("Abbrev.",max_str_len));

        /* a line of dashes */
        fprintf(fp,"%s",center("",(pr_width - (max_desc_len+5+max_str_len)) / 2));
        for (i=0; i<max_desc_len; i++)
             fprintf(fp,"-");
        fprintf(fp,"%s",center("",5));
        for (i=0; i<max_str_len; i++)
            fprintf(fp,"-");
        fprintf(fp,"\n");
        for (wid=0; wid<win_cnt; wid++){
            fprintf(fp,"%s",center("",(pr_width - (max_desc_len+5+max_str_len)) / 2));
            fprintf(fp,"%s",center(win_desc[wid],max_desc_len));
            fprintf(fp,"%s",center("",5));
            fprintf(fp,"%s\n",center(win_str[wid],max_str_len));
        }
        fprintf(fp,"\n");
        fprintf(fp,"Note: System ID with lower error rate is printed if the Null\n");
        fprintf(fp,"      Hypothesis is rejected at the 95%% Confidence Level,\n");
        fprintf(fp,"      if not, 'same' is printed.\n");
        if (fp != stdout)
            fclose(fp);
        else
            form_feed();
    }   
}

/**********************************************************************/
/*   print to stdout a comparison matrix based on the treatments      */
/**********************************************************************/
void print_trt_comp_matrix_for_RANK_one_winner(int **winner, RANK *rank, char *title, char *formula_str, char *block_id)
{
    int report_width, max_name_len=0, len_per_trt, trt, trt2, i;
    char sysname_fmt[40], title_line[200];
    int page_width=79;

    /* find the largest treat length */
    for (trt=0;trt<rnk_trt(rank);trt++)
      if ((i=strlen(Vrnk_t_name(rank,trt))) > max_name_len)
         max_name_len = i;
    sprintf(sysname_fmt," %%%ds |",max_name_len);

    len_per_trt = 3 + max_name_len;
    report_width=(rnk_trt(rank)+1) * len_per_trt + 1; 
     
    printf("%s\n",center(title,page_width));
    sprintf(title_line,"Using the %s Percentage per %s",formula_str,block_id);
    printf("%s\n",center(title_line,page_width));
    printf("%s\n\n",center("as the Comparison Metric",page_width));
                       

    /* begin to print the report matrix */
    /* just a line */
    printf("%s",center("",(page_width-report_width)/2));
    printf("|");
    for (i=0; i<len_per_trt * (rnk_trt(rank)+1) - 1; i++)
        printf("-");
    printf("|\n");
    /* the treatment titles */
    printf("%s",center("",(page_width-report_width)/2));
    printf("|");
    printf(sysname_fmt,"");
    for (trt=0; trt<rnk_trt(rank); trt++)
        printf(sysname_fmt,Vrnk_t_name(rank,trt));
    printf("\n");
    for (trt2=0; trt2<rnk_trt(rank); trt2++){
        /* spacer lines */
        printf("%s",center("",(page_width-report_width)/2));
	printf("|");
        for (trt=0; trt<rnk_trt(rank)+1; trt++){
            for (i=0; i<len_per_trt - 1; i++)
                printf("-");
            if (trt < rnk_trt(rank))
                printf("+");
        }
        printf("|\n");
        printf("%s",center("",(page_width-report_width)/2));
        printf("|");
        printf(sysname_fmt,Vrnk_t_name(rank,trt2));
        for (trt=0; trt<rnk_trt(rank); trt++)
            if (trt2 >= trt)
                printf(sysname_fmt,"");
            else if (winner[trt2][trt] == NO_DIFF)
                printf(sysname_fmt,"same");
            else if (winner[trt2][trt] < 0)
                printf(sysname_fmt,Vrnk_t_name(rank,trt2));
            else
                printf(sysname_fmt,Vrnk_t_name(rank,trt));
        printf("\n");
    }   
    /* just a line */
    printf("%s",center("",(page_width-report_width)/2));
    printf("|");
    for (i=0; i<len_per_trt * (rnk_trt(rank)+1) - 1; i++)
        printf("-");
    printf("|\n");
    form_feed();

}

int formula_index(char *str)
{
    if (*str == PER_CORR_REC)
        return(PER_CORR_REC);
    if (*str == TOTAL_ERROR)
        return(TOTAL_ERROR);
    if (*str == WORD_ACCURACY)
        return(WORD_ACCURACY);
    return(WORD_ACCURACY);
}

char *formula_str(char *str)
{
    if (*str == PER_CORR_REC)
        return(CORR_REC_STR);
    if (*str == TOTAL_ERROR)
        return(TOT_ERR_STR);
    if (*str == WORD_ACCURACY)
        return(WORD_ACC_STR);
    return(WORD_ACC_STR);
}
