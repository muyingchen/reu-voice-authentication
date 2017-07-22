/**********************************************************************/
/*                                                                    */
/*             FILENAME:  range.c                                     */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*             USAGE: this file contains programs to print a range    */
/*                    analysis table and graph for the systems and the*/
/*                    speakers                                        */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include "defs.h"

void print_gnu_range_graph PROTO((RANK *rank, char *percent_desc, char *testname, char *basename, int for_blks));

/**********************************************************************/
/*  controlling program to print the ranges                           */
/**********************************************************************/
void print_gnu_rank_ranges(RANK *rank, char *percent_desc, char *testname,char *basename)
{
    print_gnu_range_graph(rank,percent_desc,testname,basename,TRUE);
    print_gnu_range_graph(rank,percent_desc,testname,basename,FALSE);
}

/**********************************************************************/
/*  controlling program to print the ranges                           */
/**********************************************************************/
void print_rank_ranges(RANK *rank, char *percent_desc, char *testname)
{
    int scale;

    /* scale is whether to base graphs on 50(2) or 100(1) columns */
    if (pad_pr_width() > 100)
        scale = 1;
    else 
        scale = 2;

    do_blk_ranges(rank,scale,percent_desc,testname);
    do_trt_ranges(rank,scale,percent_desc,testname);
}

/**********************************************************************/
/*  do the block ranges, alias the speakers                           */
/**********************************************************************/
void do_blk_ranges(RANK *rank, int scale, char *p_desc, char *tname)
{
    int i,j;
    float Z_stat, *high, *low, *mean, *std_dev,variance,*pcts;
    char *title = "RANGE ANALYSIS ACROSS SPEAKERS FOR THE TEST:";
    char *title1= "by ";
    char pad[FULL_SCREEN];

    fprintf(stderr,"beginning range analysis on blocks \n");
    alloc_float_singarr(high,rnk_blks(rank));
    alloc_float_singarr(low,rnk_blks(rank));
    alloc_float_singarr(std_dev,rnk_blks(rank));
    alloc_float_singarr(mean,rnk_blks(rank));
    alloc_float_singarr(pcts,rnk_trt(rank));

    set_pad(pad,title);
    printf("\n\n\n%s%s\n",pad,title);
    set_pad(pad,tname);
    printf("%s%s\n",pad,tname);
    set_pad_cent_n(pad,strlen(title1)+strlen(p_desc));
    printf("%s%s%s\n\n\n\n\n",pad,title1,p_desc);

    /**** find the high, low, and the standard deviation for each block */
    for (i=0;i<rnk_blks(rank);i++){
        high[i] = 0.0;
        low[i] = 100.0;
        mean[i] = 0.0;
        for (j=0;j<rnk_trt(rank);j++){
            if (Vrnk_pcts(rank,i,j) > high[i])
               high[i] = Vrnk_pcts(rank,i,j);
            if (Vrnk_pcts(rank,i,j) < low[i])
                low[i] = Vrnk_pcts(rank,i,j);
            pcts[j] = Vrnk_pcts(rank,i,j);
        }
        calc_mean_var_std_dev_Zstat_float(pcts,rnk_trt(rank),&(mean[i]),
                                    &(variance),&(std_dev[i]),&Z_stat);
    }

    print_range_graph(scale,high,low,mean,std_dev,ovr_b_rank(rank),
                      rnk_b_name(rank),rnk_blks(rank),"  SPKR",
                      srt_b_rank(rank));

    form_feed();
}

/**********************************************************************/
/*  do the treatments ranges, alias the systems                       */
/**********************************************************************/
void do_trt_ranges(RANK *rank, int scale, char *p_desc, char *tname)
{
    int i,j;
    float Z_stat, variance, *high, *low, *mean, *std_dev, *pcts;
    char *title = "RANGE ANALYSIS ACROSS RECOGNITION SYSTEMS FOR THE TEST:";
    char *title1= "by ";
    char pad[FULL_SCREEN];

    fprintf(stderr,"beginning range analysis on treatments \n");
    alloc_float_singarr(high,rnk_trt(rank));
    alloc_float_singarr(low,rnk_trt(rank));
    alloc_float_singarr(std_dev,rnk_trt(rank));
    alloc_float_singarr(mean,rnk_trt(rank));
    alloc_float_singarr(pcts,rnk_blks(rank));


    set_pad(pad,title);
    printf("\n\n\n%s%s\n",pad,title);
    set_pad(pad,tname);
    printf("%s%s\n",pad,tname);
    set_pad_cent_n(pad,strlen(title1)+strlen(p_desc));
    printf("%s%s%s\n\n\n\n\n",pad,title1,p_desc);

    /**** find the high, low, and the standard deviation for each treatment*/
    for (i=0;i<rnk_trt(rank);i++){
        high[i] = 0.0;
        low[i] = 100.0;
        mean[i] = 0.0;
        for (j=0;j<rnk_blks(rank);j++){
            if (Vrnk_pcts(rank,j,i) > high[i])
               high[i] = Vrnk_pcts(rank,j,i);
            if (Vrnk_pcts(rank,j,i) < low[i])
                low[i] = Vrnk_pcts(rank,j,i);
            pcts[j] = Vrnk_pcts(rank,j,i);
        }
        calc_mean_var_std_dev_Zstat_float(pcts,rnk_blks(rank),&(mean[i]),
                                    &(variance),&(std_dev[i]),&(Z_stat));
    }
    print_range_graph(scale,high,low,mean,std_dev,ovr_t_rank(rank),
                      rnk_t_name(rank),rnk_trt(rank),"  SYS",srt_t_rank(rank));
    form_feed();
}

/**********************************************************************/
/*   this program prints a graph showing the ranges in descending     */
/*   order of the means                                               */
/**********************************************************************/
void print_range_graph(int scale, float *high, float *low, float *mean, float *std_dev, float *ovr_rank, char **r_names, int num_ranges, char *r_label, int *ptr_arr)
{
    int i,j,pad_1, pad_2;
    int max_range_len=6, tmp;
    char pad[FULL_SCREEN], *pct_str = "PERCENTAGES";
    char range_format[20];

    fprintf(stderr,"beginning printing of range graph\n");
    for (i=0;i<num_ranges;i++)
        if ((tmp=strlen(r_names[i])) > max_range_len)
            max_range_len = tmp;
    max_range_len++;
    sprintf(range_format,"%%-%1ds",max_range_len);

    /************************************************************/
    /*   make the range table                                   */
    /************************************************************/
    /* print the header */
    set_pad_cent_n(pad,42+max_range_len);
    printf("%s|-",pad);
    for (i=0;i<max_range_len;i++)
        printf("-");
    for (i=0;i<4;i++)
       printf("---------");
    printf("---|\n");
    printf("%s| ",pad);
    printf(range_format,r_label);
    printf("|  high  |   low  || std dev |   mean  |\n");

    printf("%s|-",pad);
    for (i=0;i<max_range_len;i++)
        printf("-");
    for (i=0;i<2;i++)
        printf("+--------");
    printf("+");
    for (i=0;i<2;i++)
        printf("+---------");
    printf("|\n");
    for (i=0;i<num_ranges;i++){
        printf("%s| ",pad);
        printf(range_format,r_names[ptr_arr[i]]);
        printf("| %5.1f  ",high[ptr_arr[i]]);
        printf("| %5.1f  ",low[ptr_arr[i]]);
        printf("||  %5.1f  ",std_dev[ptr_arr[i]]);
        printf("|  %5.1f  |\n",mean[ptr_arr[i]]);
    }
    printf("%s|-",pad);
    for (i=0;i<max_range_len;i++)
        printf("-");
    for (i=0;i<4;i++)
        printf("---------");
    printf("---|\n\n\n\n\n\n\n");




    /************************************************************/
    /*   make the range graph                                   */
    /************************************************************/
    set_pad_cent_n(pad,(100/scale) + (4+max_range_len));
    printf("%s|---",pad);
    for (i=0;i<max_range_len;i++)
        printf("-");
    for (i=0;i<100;i+=(5*scale))
        printf("-----");
    printf("|\n");


    /*   center the percent string in the table */
    printf("%s| ",pad);
    for (i=0;i<max_range_len;i++)
        printf(" ");
    printf("|");

    pad_1 =(((100/scale +1) - strlen(pct_str)) / 2);
    pad_2 =(100/scale +1) - (pad_1 + strlen(pct_str));
    set_pad_n(pad,pad_1);
    printf("%s%s",pad,pct_str);
    set_pad_n(pad,pad_2);
    printf("%s|\n",pad);

    /* print the scale */
    set_pad_cent_n(pad,(100/scale) + (4+max_range_len));
    printf("%s|-",pad);
    for (i=0;i<max_range_len;i++)
        printf("-");
    printf("+-");
    for (i=0;i<100;i+=(5*scale))
        printf("-----");
    printf("|\n");

    printf("%s| ",pad);
    for (i=0;i<max_range_len;i++)
        printf(" ");
    printf("|0");
    for (i=(5*scale);i<101;i+=(5*scale))
        printf("  %3d",i);
    printf("|\n");

    printf("%s| ",pad);
    printf(range_format,r_label);
    printf("||");
    for (i=(5*scale);i<101;i+=(5*scale))
        printf("    |");
    printf("|\n");

    printf("%s|-",pad);
    for (i=0;i<max_range_len;i++)
        printf("-");
    printf("+-");
    for (i=0;i<100;i+=(5*scale))
        printf("-----");
    printf("|\n");

    /* begin to make the graphs of the ranges */
    for (i=0;i<num_ranges;i++){
        printf("%s| ",pad);
        printf(range_format,r_names[ptr_arr[i]]);
        printf("|");
        for (j=0;j<101;j+=scale){
            if ((j > (mean[ptr_arr[i]]-(0.0001+(float)(scale)/2.00))) &&
                (j < (mean[ptr_arr[i]]+(0.0001+(float)(scale)/2.00))))
                printf("|");
            else if (((j>(mean[ptr_arr[i]]+
                         std_dev[ptr_arr[i]]-(0.0001+(float)(scale)/2.00))) &&
                      (j<(mean[ptr_arr[i]]+
                         std_dev[ptr_arr[i]]+(0.0001+(float)(scale)/2.00))))||
                     ((j>(mean[ptr_arr[i]]-
                         std_dev[ptr_arr[i]]-(0.0001+(float)(scale)/2.00))) &&
                      (j<(mean[ptr_arr[i]]-
                         std_dev[ptr_arr[i]]+(0.0001+(float)(scale)/2.00)))))
                printf("+");
          
            else if ((j > high[ptr_arr[i]]) || 
                     (j < low[ptr_arr[i]]))
                printf(" ");
            else
                printf("-");
	}
        printf("|\n");
    }
    printf("%s|---",pad);
    for (i=0;i<max_range_len;i++)
        printf("-");
    for (i=0;i<100;i+=(5*scale))
        printf("-----");
    printf("|\n\n");

    printf("%s      | -> shows the mean\n",pad);
    printf("%s      + -> shows plus or minus one standard deviation\n",pad);
}

/**********************************************************************/
/*   this program prints a graph showing the ranges in descending     */
/*   order of the means                                               */
/**********************************************************************/
void print_gnu_range_graph(RANK *rank, char *percent_desc, char *testname, char *base, int for_blks)
{
    int b,t;
    FILE *fp_dat, *fp_mean, *fp_plt, *fp_median;
    float sum;
    char mean_name[50], dat_name[50], plt_name[50], basename[50];
    char median_name[50];
    float *pctt, *rnkk;
    int *ind;

    alloc_singarr(pctt,MAX(rnk_blks(rank),rnk_trt(rank)),float);
    alloc_singarr(rnkk,MAX(rnk_blks(rank),rnk_trt(rank)),float);
    alloc_singarr(ind,MAX(rnk_blks(rank),rnk_trt(rank)),int);

    sprintf(basename,"%s%s",base,(for_blks) ? ".blk" : ".trt");
    sprintf(dat_name,"%s%s",basename,".dat");
    /* make the datafiles for the treatements */
    if ((fp_dat = fopen(dat_name,"w")) == (FILE *)0){
	fprintf(stderr,"Error: unable to open GNUPLOT data file %s\n",
		dat_name);
	exit(-1);
    } 
    sprintf(mean_name,"%s%s",basename,".mean");
    if ((fp_mean = fopen(mean_name,"w")) == (FILE *)0){
	fprintf(stderr,"Error: unable to open GNUPLOT data file %s\n",
		mean_name);
	exit(-1);
    }
    sprintf(median_name,"%s%s",basename,".median");
    if ((fp_median = fopen(median_name,"w")) == (FILE *)0){
	fprintf(stderr,"Error: unable to open GNUPLOT data file %s\n",
		median_name);
	exit(-1);
    }
    sprintf(plt_name,"%s%s",basename,".plt");
    if ((fp_plt = fopen(plt_name,"w")) == (FILE *)0){
	fprintf(stderr,"Error: unable to open GNUPLOT file %s\n",
		plt_name);
	exit(-1);
    } 
    fprintf(fp_plt,"set yrange [%d:0]\n",
	    1 + ((for_blks) ? rnk_blks(rank) : rnk_trt(rank)));
    fprintf(fp_plt,"set xrange [0:100]\n");
    fprintf(fp_plt,"set title \"%s\"\n",testname);
    fprintf(fp_plt,"set key\n");
    fprintf(fp_plt,"set ylabel \"%s\"\n",(for_blks) ? "Speaker ID" :"Systems");
    fprintf(fp_plt,"set xlabel \"%s\"\n",percent_desc);
    fprintf(fp_plt,"set ytics (");

    if (for_blks){
	for (b=0;b<rnk_blks(rank);b++){
	    float medianval=0.0;
	    sum = 0.0;
	    for (t=0;t<rnk_trt(rank);t++){
		fprintf(fp_dat,"%d %f\n",
			b+1,Vrnk_pcts(rank,Vsrt_b_rank(rank,b),t));
		pctt[t] = Vrnk_pcts(rank,Vsrt_b_rank(rank,b),t);
		sum += Vrnk_pcts(rank,Vsrt_b_rank(rank,b),t);
	    }
	    printf("  num_blk %d  num_trt %d\n",rnk_blks(rank),rnk_trt(rank));

	    rank_float_arr(pctt,rnk_trt(rank),ind,rnkk,INCREASING);
	    if (rnk_trt(rank) % 2 == 0) { /* handle the even arr len */
		medianval = (pctt[ind[rnk_trt(rank) / 2]] + 
			     pctt[ind[(rnk_trt(rank) / 2) - 1]]) / 2.0;
	    } else { /* handle the odd arr len */
		printf("odd ind %d %d\n",
		       ind[rnk_trt(rank) / 2],
		       rnk_trt(rank) / 2);
		medianval = pctt[ind[rnk_trt(rank) / 2]];
	    }
	    fprintf(fp_mean,"%d %f\n",b+1,sum / (float)(rnk_trt(rank)));
	    fprintf(fp_median,"%d %f\n",b+1,medianval);
	    fprintf(fp_plt,"\"%s\" %d",Vrnk_b_name(rank,Vsrt_b_rank(rank,b)),
		    b+1);
	    if (b != rnk_blks(rank)-1)
		fprintf(fp_plt,",");
	}
	fprintf(fp_plt,")\n");
    } else {
	for (t=0;t<rnk_trt(rank);t++){
	    float medianval=0.0;
	    sum = 0.0;

	    for (b=0;b<rnk_blks(rank);b++){
		fprintf(fp_dat,"%d %f\n",
			t+1,Vrnk_pcts(rank,b,Vsrt_t_rank(rank,t)));
		pctt[b] = Vrnk_pcts(rank,b,Vsrt_t_rank(rank,t));
		sum += Vrnk_pcts(rank,b,Vsrt_t_rank(rank,t));
	    }
	    rank_float_arr(pctt,rnk_blks(rank),ind,rnkk,INCREASING);
	    if (rnk_blks(rank) % 2 == 0) { /* handle the even arr len */
		medianval = (pctt[ind[rnk_blks(rank) / 2]] + 
			     pctt[ind[(rnk_blks(rank) / 2) - 1]]) / 2.0;
	    } else { /* handle the odd arr len */
		printf("odd ind %d %d\n",
		       ind[rnk_blks(rank) / 2],
		       rnk_blks(rank) / 2);
		medianval = pctt[ind[rnk_blks(rank) / 2]];
	    }
	    fprintf(fp_mean,"%d %f\n",t+1,sum / (float)(rnk_blks(rank)));
	    fprintf(fp_median,"%d %f\n",t+1,medianval);
	    fprintf(fp_plt,"\"%s\" %d",Vrnk_t_name(rank,Vsrt_t_rank(rank,t)),
		    t+1);
	    if (t != rnk_trt(rank)-1)
		fprintf(fp_plt,",");
	}
	fprintf(fp_plt,")\n");
    }

    fprintf(fp_plt,"plot \"%s\" using 2:1 title \"Mean %s\" with lines,",
	    mean_name, percent_desc);
    fprintf(fp_plt,"\"%s\" using 2:1 title \"Median %s\" with lines,",
	    median_name, percent_desc);
    fprintf(fp_plt,"\"%s\" using 2:1 title \"Individual %s\"\n",dat_name,
	    percent_desc);
    fclose(fp_dat);
    fclose(fp_mean);
    fclose(fp_plt);

    free_singarr(pctt,float);
    free_singarr(rnkk,float);
    free_singarr(ind,int);
}

