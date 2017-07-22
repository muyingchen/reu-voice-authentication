/**********************************************************************/
/*                                                                    */
/*           FILE: anovar.c                                           */
/*           WRITTEN BY: Jonathan G. Fiscus                           */
/*           DATE: April 14 1989                                      */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           USAGE: This uses the rank structure to perform           */
/*                  a Friedman two-way analysis of variance           */
/*                  by ranks and generates a report.                  */
/*                                                                    */
/*           SOURCE:Applied Non Parametric Statistics by Daniel       */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include "defs.h"


#define FRI_TITLE	"Friedman Two-way Analysis of Variance by Ranks"
#define RANK_TITLEA	"COMPARISON MATRIX: Comparing All Systems"
#define RANK_TITLEB	"COMPARISON MATRIX: Comparing All Speakers"
#define RANK_TITLE_1	"Using a Multiple Comparison Test"
#define ADJUST_THRESH	0.50000
#define MULTI_COMP_ALPHA	0.05

float fsum_sqr(float *arr, int len);
float calc_adjust_for_trt(RANK *rank);
float calc_adjust_for_blks(RANK *rank);
float calc_comp_value(int trt_num, int blk_num);

/****************************************************************/
/*   main procedure to perform a two-way analysis of variance by*/
/*   ranks.  first the sum of the treatment and block ranks     */
/*   are summed then if there is a difference between at least  */
/*   one of the blocks or treatments, then a multiple comparison*/
/*   is performed                                               */
/****************************************************************/
void compute_anovar(RANK *rank, int verbose)
{
    float *sum_trt_rank, *sum_blk_rank;
    char pad[FULL_SCREEN];
    int i,j;

    alloc_float_singarr_zero(sum_trt_rank,rnk_trt(rank));
    alloc_float_singarr_zero(sum_blk_rank,rnk_blks(rank));

    /*  sum the ranks */
    for (i=0;i<rnk_blks(rank);i++)
        for (j=0;j<rnk_trt(rank);j++){
            sum_trt_rank[j] += Vrnk_t_rank(rank,i,j);
            sum_blk_rank[i] += Vrnk_b_rank(rank,i,j);
	}

    if (verbose){
        set_pad(pad,FRI_TITLE);
        printf("\n\n\n%s%s\n",pad,FRI_TITLE);
        printf("%s----------------------------------------------\n\n",pad);
        set_pad_cent_n(pad,SCREEN_WIDTH);
    printf("%s\tHo: Testing the hypothesis that all recognizers are the same",
                 pad);
    }
    /* if at least one difference, do multiple comparison */
    if (analyze_rank_sum(rank,rnk_trt(rank),rnk_blks(rank),
			 sum_trt_rank,
                         FOR_TREATMENTS,verbose) == TEST_DIFF)
        do_multi_comp_for_anovar(rnk_trt(rank),rnk_blks(rank),
                                 rnk_t_name(rank),sum_trt_rank,
                                 ovr_t_rank(rank),RANK_TITLEA,
                                 srt_t_rank(rank),verbose);
    if (verbose) form_feed();

    if (verbose){
        set_pad(pad,FRI_TITLE);
        printf("\n\n\n%s%s\n",pad,FRI_TITLE);
        printf("%s----------------------------------------------\n\n",pad);
        set_pad_cent_n(pad,SCREEN_WIDTH);
       printf("%s\tHo: Testing the hypothesis that all speakers are the same",
                 pad);
    }
    /* if at least one difference, do multiple comparison */
    if (analyze_rank_sum(rank,rnk_blks(rank),rnk_trt(rank),sum_blk_rank,
                         FOR_BLOCKS,verbose) == TEST_DIFF)
        do_multi_comp_for_anovar(rnk_blks(rank),rnk_trt(rank),
                                 rnk_b_name(rank),sum_blk_rank,
                                 ovr_b_rank(rank),RANK_TITLEB,
                                 srt_b_rank(rank),verbose);
    if (verbose) form_feed();
}

/*****************************************************************/
/*        X2_r formula:                                          */
/*                       k                                       */
/*             12        --.  2                                  */
/*          -------  *   \   R   - 3b(t+1)                       */
/*         bt(t+1)       /    j                                  */
/*                       --'                                     */
/*                      j = 1                                    */
/*                                                               */
/*   b = num_blk                                                 */
/*   t = num_trt                                                 */
/*   R = Sum of the ranks for that column                        */
/*                                                               */
/*  this is the initial test to see if the treatment or ranks    */
/*  are different                                                */
/*****************************************************************/
int analyze_rank_sum(RANK *rank, int trt_num, int blk_num, float *sum_arr, int orient, int verbose)
{
    int df;
    float X2_r, adjust;
    char *subject_blk = "speaker";
    char *subject_trt = "recognition system";
  
    /* calculate the test  statistic */
    df = trt_num - 2;     /* #trt is one based so subtract 2 */
    /* make sure df isn't negative */;
    if (df < 0) df = 0;
    /* make sure df isn't greater than MAX_DF */
    if (df > MAX_DF) df = MAX_DF;

    X2_r = (12.00000 /
                (float)(blk_num*trt_num*(trt_num+1))) *
           (fsum_sqr(sum_arr,trt_num)) -
           3.00000 * (float)blk_num * (float)(trt_num+1);
    if (orient == FOR_TREATMENTS)
        adjust = calc_adjust_for_trt(rank);
    else
        adjust = calc_adjust_for_blks(rank);

    /* if two identical systems are entered, adjust will be 0.0 and core*/
    /* dump.  this avoids this */
    if (adjust == 0.0)
       X2_r = 0.0;
    else
       X2_r /= adjust;

    if (verbose) 
        if (orient == FOR_TREATMENTS)
            print_result_of_analyze_rank_sum(df, X2_r, adjust, subject_trt);
        else
            print_result_of_analyze_rank_sum(df, X2_r, adjust, subject_blk);

    /* return the result */
    if (X2_r > X2.df[df].level[GEN_X2_PER])
        return(TEST_DIFF);
    else
        return(NO_DIFF);
}

/*****************************************************************/
/*   print to stdout the results of the rank_sum test            */
/*****************************************************************/
void print_result_of_analyze_rank_sum(int df, float X2_r, float adjust, char *subject)
{
    char pad[FULL_SCREEN];
    int i;

    set_pad_cent_n(pad,SCREEN_WIDTH);
    printf("\n\n\n%s%35sReject if\n",pad,"");
    printf("%s%26sX2_r > X2 of %s %s (%2.3f)\n", pad, "", 
                          X2_pct_str(GEN_X2_PER),
                          X2_df_str(df),
                          X2_score(df,GEN_X2_PER));
    printf("\n"); 
    printf("%s%26sadjustment = %2.3f\n",pad,"",adjust);
    printf("%s%30s  X2_r = %2.3f\n",pad,"",X2_r);
    if (adjust < ADJUST_THRESH){
        printf("\n\n%s\t\t*** Warning:  ties adjustment may have severely\n",
                                                                   pad);
        printf("%s\t\t              exagerated the X2_r value\n\n",pad);
    }
    printf("%sANALYSIS:\n%s--------\n",pad,pad);
    if (X2_r > X2_score(df,GEN_X2_PER)){
     printf("%s\tThe test statistic X2_r shows, with %s confidence, that at\n"
                                            ,pad,X2.neg_per_str[GEN_X2_PER]),
     printf("%s\tleast one %s is significantly different.\n",pad,subject);
        printf("\n");
        printf("%s\tFurther, the probablity of there being a difference is\n",
               pad);
        for (i=GEN_X2_PER;i<MAX_X2_PER;i++)
            if (X2_r < X2_score(df,i+1))
                break;
        if (i==MAX_X2_PER)
            printf("%s\tgreater that %s.\n",pad,X2_neg_pct_str(i));
        else
            printf("%s\tbetween %s to %s.\n",pad,X2_neg_pct_str(i),
                                             X2_neg_pct_str(i+1));
        printf("\n\n");
    }
    else{
       printf("%s\tThe test statistic X2_r shows that at the %s confidence\n"
                          ,pad,X2_neg_pct_str(GEN_X2_PER)),
       printf("%s\tinterval, the %ss are not significantly\n",pad,subject);
        printf("%s\tdifferent.\n\n",pad);
        printf("%s\tFurther, the probablity of there being a difference is\n"
                          ,pad);
        for (i=GEN_X2_PER;i>MIN_X2_PER;i--)
            if (X2_r > X2_score(df,i-1))
                break;
        if (i==MIN_X2_PER)
            printf("%s\tless than %s.\n",pad,X2_neg_pct_str(i));
        else
            printf("%s\tbetween %s to %s.\n",pad,X2_neg_pct_str(i-1),
                                         X2_neg_pct_str(i));
        printf("\n\n");
    }
}

/********************************************************************/
/*  return the floating point value of the sum of the squares       */
/********************************************************************/
float fsum_sqr(float *arr, int len)
{
    int i;
    float tot=0.000000;

    for (i=0;i<len;i++)
        tot += (arr[i]*arr[i]);
    return(tot);
}

/********************************************************************/
/*     Ties adjustments                                             */
/*              b                                                   */
/*             --.                                                  */
/*    1   -    \   T                                                */
/*             /    i                                               */
/*             --'                                                  */
/*          -----------                                             */
/*                2                                                 */
/*            bt(t -1)                                              */
/*                                                                  */
/*               3                                                  */
/*     T =  Sum t   - Sum t        where t = num ties in ith block  */
/*      i        i         i                                        */
/*                                                                  */
/*   to adjust for ties, the test statistic must be taken down      */
/*   by a certain factor, this function returns it                  */
/********************************************************************/
float calc_adjust_for_trt(RANK *rank)
{
    float *sort_arr;
    int sort_num, *sort_count, i, j, sum_T=0;

    alloc_float_singarr(sort_arr,rnk_trt(rank));
    alloc_int_singarr(sort_count,rnk_trt(rank));

    /* find the ties ans sum Ti */
    for (i=0;i<rnk_blks(rank);i++){
        for (j=0;j<rnk_trt(rank); j++){
            sort_arr[j]=0.0;
            sort_count[j]=0;
	}
        for (j=0;j<rnk_trt(rank); j++){
            sort_num = 0;
            while ((sort_arr[sort_num] != 0.0) && 
                   (Vrnk_t_rank(rank,i,j) != sort_arr[sort_num]))
                sort_num++;
            sort_arr[sort_num] = Vrnk_t_rank(rank,i,j);
            sort_count[sort_num]++;
	}
        for (j=0;j<rnk_trt(rank); j++)
            if (sort_count[j] > 0)
                sum_T += (sort_count[j] * sort_count[j] * sort_count[j])
                           - sort_count[j];
    }

    /* return the adjustment number */
    return( 1.000000 - (sum_T/
                        (float)(rnk_blks(rank)*
                                rnk_trt(rank)*
                                ((rnk_trt(rank) * rnk_trt(rank)) - 1))));
}

/********************************************************************/
/*  see calc_adjust_for_trt but substitute B for t                  */
/********************************************************************/
float calc_adjust_for_blks(RANK *rank)
{
    float *sort_arr;
    int sort_num, *sort_count, i, j, sum_T=0;

    alloc_float_singarr(sort_arr,rnk_blks(rank));
    alloc_int_singarr(sort_count,rnk_blks(rank));

    /* find the ties ans sum Ti */
    for (i=0;i<rnk_trt(rank);i++){
        for (j=0;j<rnk_blks(rank); j++){
            sort_arr[j]=0.0;
            sort_count[j]=0;
	}
        for (j=0;j<rnk_blks(rank); j++){
            sort_num = 0;
            while ((sort_arr[sort_num] != 0.0) && 
                   (Vrnk_b_rank(rank,j,i) != sort_arr[sort_num]))
                sort_num++;
            sort_arr[sort_num] = Vrnk_b_rank(rank,j,i);
            sort_count[sort_num]++;
	}
        for (j=0;j<rnk_trt(rank); j++)
            if (sort_count[j] > 0)
                sum_T += (sort_count[j] * sort_count[j] * sort_count[j])
                           - sort_count[j];
    }
    /* return the adjustment number */
    return( 1.000000 - (sum_T/
                        (float)(rnk_trt(rank)*
                                rnk_blks(rank)*
                                ((rnk_blks(rank) * rnk_blks(rank)) - 1))));
}

/********************************************************************/
/*  for the treatments, calculate and do the MULTIPLE comparison    */
/*  print everything out if requested                               */
/********************************************************************/
void do_multi_comp_for_anovar(int trt_num, int blk_num, char **trt_names, float *sum_arr, float *ovr_rank_arr, char *title, int *srt_ptr, int verbose)
{
    static int **stat_sum=NULL;
 
    if (stat_sum == NULL){
        alloc_int_2dimarr(stat_sum,MAX_TREATMENTS,MAX_TREATMENTS);
    }

    clear_2dim(stat_sum,MAX_TREATMENTS,MAX_TREATMENTS,0.0);
    calc_stat_ranks(trt_num,blk_num,stat_sum,sum_arr);

    if (verbose)
        print_ANOVAR_comp_matrix(trt_num,stat_sum,trt_names,title,srt_ptr);
}

/*********************************************************************/
/*      Multiple comparison test to rank the systems                 */
/*                             ________                              */
/*                            / bt(t+1)                              */
/*     | R  - R  |    >=   z / --------                              */
/*        j    i            V     6                                  */ 
/*                                                                   */
/*  R = the Sum of the nth  column ranks                             */
/*  z = Z score the alpha = 0.05                                     */
/*                          ----                                     */
/*                         t(t-1)                                    */
/*                                                                   */
/*  go through all possible comparisons and rank the treatments for  */
/*  according to the number of treatments beaten or beaten by        */
/*                                                                   */
/*********************************************************************/
void calc_stat_ranks(int trt_num, int blk_num, int **stat_sum, float *sum_arr)
{
    int comp1, comp2, result;
    float Zcomp_value, compare_value;
    double sqr_root;

    /* calculate the comparison value for the confidence interval */
    Zcomp_value = calc_comp_value(trt_num,blk_num);
    sqr_root = sqrt(blk_num * 
                                     trt_num * 
                                     (trt_num - 1) /
                                     6.000000);
    compare_value = (float)Zcomp_value * (float)sqr_root;

    /* go through the comparison and build an array to rank */
    for (comp1=0; comp1 <(trt_num-1); comp1++)
        for (comp2=comp1+1; comp2<(trt_num); comp2++){
            if (fabs(sum_arr[comp1] - sum_arr[comp2]) >= compare_value)
                if (sum_arr[comp1] > sum_arr[comp2])
                    result = 1;
                else
                    result = -1;
            else
                result = 0;
            stat_sum[comp1][comp2] += result;
            stat_sum[comp2][comp1] += result * (-1);
	}
}

/*********************************************************************/
/*   Given the alpha, find the closest approximate Z index to that   */
/*   percentage                                                      */
/*********************************************************************/
float calc_comp_value(int trt_num, int blk_num)
{
    float Z_area;
    int i;

    Z_area = MULTI_COMP_ALPHA / (float)(trt_num * (trt_num-1));

    if (Z_area < Z_exter(Z1tail,MAX_Z_PER))
        return(Z_score(Z1tail,MAX_Z_PER));

    for (i=MAX_Z_PER+1; i< MIN_Z_PER ; i++)
        if (Z_area < Z_exter(Z1tail,i))
            return(Z_score(Z1tail,i));

    return(Z_score(Z1tail,MIN_Z_PER));
}


void print_ANOVAR_comp_matrix(int trt_num, int **stat_sum, char **trt_names, char *title, int *srt_ptr)
{
    char pad[FULL_SCREEN],name_format[50],*hyphens="--------------------";
    char *spaces="                    ";
    int t, t2;
    int max_trt_name_len=4, hy_l=20;

    set_pad(pad,title);
    printf("\n\n%s%s\n",pad,title);
    set_pad(pad,RANK_TITLE_1);
    printf("%s%s\n\n\n\n",pad,RANK_TITLE_1);

    for (t=0; t<trt_num; t++)
        if (max_trt_name_len < strlen(trt_names[t]))
            max_trt_name_len = strlen(trt_names[t]);
    set_pad_cent_n(pad,(trt_num+1) * (max_trt_name_len+2+1));
    sprintf(name_format,"| %%-%ds ",max_trt_name_len);

    /* first line */
    printf("%s|%s",pad,(hyphens+(hy_l-(max_trt_name_len+2))));
    for (t=0; t<trt_num; t++)
       printf("%s",(hyphens+(hy_l-(max_trt_name_len+3))));
    printf("|\n");

    /* systems on top */
    printf("%s|%s",pad,(spaces+(hy_l-(max_trt_name_len+2))));
    for (t=0; t<trt_num; t++)
       printf(name_format,trt_names[srt_ptr[t]]);
    printf("|\n"); 

    /* separation line */
    printf("%s|%s+",pad,(hyphens+(hy_l-(max_trt_name_len+2))));
    for (t=0; t<trt_num-1; t++)
       printf("%s",(hyphens+(hy_l-(max_trt_name_len+3))));
    printf("%s|\n",(hyphens+(hy_l-(max_trt_name_len+2))));

    /* systems */
    for (t=0; t<trt_num; t++){
       printf("%s",pad);
       printf(name_format,trt_names[srt_ptr[t]]);
       for (t2=0; t2<trt_num; t2++){
           if (t < t2)
               switch (stat_sum[srt_ptr[t]][srt_ptr[t2]]) {
                   case -1: printf(name_format,trt_names[srt_ptr[t]]);
                            break;
                   case 1:  printf(name_format,trt_names[srt_ptr[t2]]);
                            break;
                   default: printf(name_format,"same");
                            break;
	       }
           else
               printf(name_format,"");
       }
       printf("|\n");
    }

    /* last line */
    printf("%s|%s",pad,(hyphens+(hy_l-(max_trt_name_len+2))));
    for (t=0; t<trt_num; t++)
       printf("%s",(hyphens+(hy_l-(max_trt_name_len+3))));
    printf("|\n");
}


