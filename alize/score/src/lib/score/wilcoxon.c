/**********************************************************************/
/*                                                                    */
/*           FILE: wilcoxon.c                                         */
/*           WRITTEN BY: Jonathan G. Fiscus                           */
/*           DATE: April 14 1989                                      */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           USAGE: This uses the rank structure to perform           */
/*                  the non-parametric Wilcoxon Test and generates    */
/*                  a report.                                         */
/*                                                                    */
/*           SOURCE:Statistics: Probability, Inference and Decision   */
/*                  By Robert L. Winkler and William L. Hays,         */
/*                  Page 856                                          */
/*                                                                    */
/*           See Also: The documentation file "wilcoxon.doc"          */
/*                                                                    */
/**********************************************************************/


#include <phone/stdcenvf.h>
#include <score/scorelib.h>

float wilcoxon_stat(int n, int rank_sum);

/****************************************************************/
/*   main procedure to perform the wilcoxon test on the RANK    */
/*   structure.                                                 */
/****************************************************************/
void perform_wilcoxon(RANK *rank, int verbose, int report, char *formula_str, int formula_id, int **winner)
{
    int comp1, comp2, result;
  
    if (!report) verbose=FALSE;
    for (comp1=0; comp1 <rnk_trt(rank) -1; comp1++)
        for (comp2=comp1+1; comp2<rnk_trt(rank); comp2++){
            result = compute_wilcoxon_for_treatment(rank,comp1,comp2,"Spkr",
                                  formula_str,verbose,formula_id==TOTAL_ERROR);
            winner[comp1][comp2] = result;
        }

    if (report) print_trt_comp_matrix_for_RANK_one_winner(winner,rank,
                 "Comparison Matrix for the Wilcoxon Test",formula_str,
                 "Speaker");
}

/****************************************************************/
/*   Given the two indexes of treatments to compare (in the     */
/*   RANK Struct) compute the Wilcoxon statistics               */
/*   vars:                                                      */
/*       zero_is_best : This identifies the "ideal" value for   */
/*                      the value computed in the rank struct   */
/*                      percentages.                            */
/****************************************************************/
int compute_wilcoxon_for_treatment(RANK *rank, int treat1, int treat2, char *block_id, char *formula_str, int verbose, int zero_is_best)
{
    int sum_plus=0, sum_minus=0, i;
    float sum_plus_rank = 0.0, sum_minus_rank = 0.0;
    float sum_trt1=0.0, sum_trt2=0.0, sum_trt_diff=0.0;
    int block, max_len_block=0, max_len_treat=6;
    char pct_format[20], thresh_str[140];
    char title_line[200];
    int paper_width = 79, rep_width;
    float equal_thresh = 0.05;
    float *block_diff, *block_rank;
    int *block_sort, tptr[2];
    int alternate;

    alloc_singarr(block_diff,rnk_blks(rank),float);
    alloc_singarr(block_rank,rnk_blks(rank),float);
    alloc_singarr(block_sort,rnk_blks(rank),int);

    /* change the ordering if necessary */
    for (block=0; block < rnk_blks(rank); block++){
        sum_trt1 += Vrnk_pcts(rank,block,treat1);
        sum_trt2 += Vrnk_pcts(rank,block,treat2);
    }
    if (sum_trt1 > sum_trt2)
        tptr[0] = treat1, tptr[1] = treat2;
    else
        tptr[0] = treat2, tptr[1] = treat1;
    sum_trt1 = sum_trt2 = 0.0;

    /* compute the test */
    for (block=0; block < rnk_blks(rank); block++)
        block_diff[block] = fabs(Vrnk_pcts(rank,block,tptr[0]) -
                                 Vrnk_pcts(rank,block,tptr[1]));
    rank_float_arr(block_diff,rnk_blks(rank),block_sort,block_rank,INCREASING);
    for (block=0; block < rnk_blks(rank); block++)
        block_diff[block] = Vrnk_pcts(rank,block,tptr[0]) -
                                 Vrnk_pcts(rank,block,tptr[1]);


    sprintf(thresh_str,"(Threshold for equal percentages +- %5.3f)",
                       equal_thresh);

    max_len_block = strlen(block_id);
    for (block=0; block < rnk_blks(rank); block++)
         if (strlen(Vrnk_b_name(rank,block)) > max_len_block)

             max_len_block = strlen(Vrnk_b_name(rank,block));

    if (max_len_treat < strlen(Vrnk_t_name(rank,tptr[0])))
        max_len_treat = strlen(Vrnk_t_name(rank,tptr[0]));
    if (max_len_treat < strlen(Vrnk_t_name(rank,tptr[1])))
        max_len_treat = strlen(Vrnk_t_name(rank,tptr[1]));
    rep_width = max_len_block + max_len_treat * 2 + 37;

    pct_format[0] = NULL_CHAR;
    strcat(pct_format,center("",(max_len_treat-6)/2));
    strcat(pct_format,"%6.2f");
    strcat(pct_format,center("",max_len_treat - ((max_len_treat-6)/2 + 6) ));

    /*  Print a detailed table showing sign differences */
    if (verbose) {
        printf("%s\n",center("Wilcoxon Test Calculations Table",paper_width));
        sprintf(title_line,"Comparing %s %s Percentages for Systems %s and %s",
                            block_id, formula_str,Vrnk_t_name(rank,tptr[0]),
                            Vrnk_t_name(rank,tptr[1]));
        printf("%s\n\n",center(title_line,paper_width));
/*        printf("%s\n\n",center(thresh_str,paper_width));*/

        printf("%s",center("",(paper_width - rep_width)/2));
        printf("%s",center("",max_len_block));
        printf("    "); printf("%s",center("",max_len_treat));
        printf("    "); printf("%s",center("",max_len_treat));
        printf("  ");
        printf("                    Signed\n");

        printf("%s",center("",(paper_width - rep_width)/2));
        printf("%s",center(block_id,max_len_block));
        printf("    ");
        printf("%s",center(Vrnk_t_name(rank,tptr[0]),max_len_treat));
        printf("    ");
        printf("%s",center(Vrnk_t_name(rank,tptr[1]),max_len_treat));
        printf("  ");
        printf("Difference   Rank    Rank\n");
        printf("%s",center("",(paper_width - rep_width)/2));
        for (i=0; i<rep_width; i++)
            printf("-");
        printf("\n");
    }
    alternate=0;
    for (block=0; block < rnk_blks(rank); block++){
         if (verbose) {
             printf("%s",center("",(paper_width - rep_width)/2));
             printf("%s",center(Vrnk_b_name(rank,block),max_len_block));
             printf("    ");
             printf(pct_format,Vrnk_pcts(rank,block,tptr[0]));
             printf("    ");
             printf(pct_format,Vrnk_pcts(rank,block,tptr[1]));
             printf("   ");
	     printf("%6.2f    %5.1f    ",block_diff[block],block_rank[block]);
             sum_trt1 += Vrnk_pcts(rank,block,tptr[0]); 
             sum_trt2 += Vrnk_pcts(rank,block,tptr[1]);
             sum_trt_diff += block_diff[block];
         }
         if (block_diff[block] < 0.0 && (fabs(block_diff[block]) > 0.005)){
             if (verbose) printf("%5.1f\n",(-1.0) * block_rank[block]);
             sum_minus++;
             sum_minus_rank += block_rank[block];
         }
         else if (block_diff[block] > 0.0 && (fabs(block_diff[block]) > 0.005)){
             if (verbose) printf("%5.1f\n",block_rank[block]);
             sum_plus++;
             sum_plus_rank += block_rank[block];
         } else {
             if (verbose) printf("%5.1f\n",block_rank[block]);
	     if ((alternate++ % 2) == 0){
		 sum_plus++;
		 sum_plus_rank += block_rank[block];
	     } else {
		 sum_minus++;
		 sum_minus_rank += block_rank[block];
	     }		 
	 }
    }
    if (verbose){
        printf("%s",center("",(paper_width - rep_width)/2));
        for (i=0; i<rep_width; i++)
            printf("-");
        printf("\n");
        /* An average line */
        printf("%s",center("",(paper_width - rep_width)/2));
        printf("%s",center("Avg.",max_len_block));
        printf("    ");
        printf(pct_format,sum_trt1 / rnk_blks(rank));
        printf("    ");
        printf(pct_format,sum_trt2 / rnk_blks(rank));
        printf("   ");
	printf("%6.2f",sum_trt_diff / rnk_blks(rank));
        printf("\n\n");

        sprintf(title_line,"Sum of %2d positive ranks = %4.1f",sum_plus,sum_plus_rank);
        printf("%s\n",center(title_line,paper_width));
        sprintf(title_line,"Sum of %2d negative ranks = %4.1f",sum_minus,sum_minus_rank);
        printf("%s\n",center(title_line,paper_width));
        printf("\n\n");
    }
    
    free_singarr(block_diff,float);
    free_singarr(block_rank,float);
    free_singarr(block_sort,int);

    /* Analyze the Results */
    { int result;
      result = wilcoxon_test_analysis(sum_plus,sum_minus,sum_plus_rank,sum_minus_rank,
                                  Vrnk_t_name(rank,tptr[0]),Vrnk_t_name(rank,tptr[1]),
                                  0.05,verbose,zero_is_best);
      /* if the result is significant, system which is better depends on if */
      /* the treatments have been swapped, a negative result means tprt[0] is */
      /* better, positive one tprt[1] is better */
      return(result * ((tptr[0] == treat1) ? 1 : (-1)));
    }
}

/****************************************************************/
/*   Given the vital numbers for computing a rank sum test,     */
/*   Compute it, and if requested, print a verbose analysis     */
/****************************************************************/
int wilcoxon_test_analysis(int num_a, int num_b, float sum_a, float sum_b, char *treat1_str, char *treat2_str, float alpha, int verbose, int zero_is_best)
{
    float test_stat, wilcoxon_test_stat;
    int i, z_ind=0, dbg=0;
    char better;

    for (i=0; i <= PER90; i++)
        if (Z2tail[i].perc_interior-(1.0 - alpha) > -0.001){
            z_ind=i;
            if (dbg) printf("Z score %d %f %f\n",i,Z2tail[i].perc_interior,
                            Z2tail[i].perc_interior-(1.0 - alpha));
	    }
    /* compute the Z statistic */
    { float W, E;
      double var;
      int n;
      if (num_a < num_b)
          W = sum_a; 
      else
          W = sum_b; 
      n = num_a + num_b;
      better = (num_a < num_b) ? 'b' : 'a';
      if (zero_is_best)  /* make an adjustment for direction of PCTS*/
          better = (better == 'a') ? 'b' : 'a';
      E = ((float)n * (float)(n+1) / 4.0);
      var = (double)(n * (n+1) * (2*n + 1)) / 24.0;
      test_stat = ((W - E) / (float)sqrt(var));
      /* W will be rounded to the nearest whole number */
/*      wilcoxon_test_stat = wilcoxon_stat(num_a+num_b,(int)(W+0.5));*/
      if (dbg) printf("   Z Approximation for Wilcoxon Signed-Rank test\n");
      if (dbg) printf("        W = %f\n",W);
      if (dbg) printf("        n = %d\n",n);
      if (dbg) printf("     E(W) = %f\n",E);
      if (dbg) printf("      var = %f\n",var);
      if (dbg) printf("   Z_stat = %f\n",test_stat);
      if (dbg) printf(" Wilcoxon = %f\n",wilcoxon_test_stat);
      if (dbg) printf("   Better = %c\n",better);
      if (dbg) printf(" Better S = %s\n",(better == 'a') ? treat1_str : treat2_str);

    }

    if (verbose){
        printf("    The Null Hypothesis:\n\n");
        printf("         The two populations represented by the respective matched\n");
        printf("         pairs are identical.\n\n");
        printf("    Alternate Hypothesis:\n\n");
        printf("         The two populations are not equal and therfore\n");
        printf("         there is a difference between systems.\n\n");
        printf("    Decision Analysis:\n\n");
        printf("         Assumptions:\n");
        printf("              The distribution of signed ranks is approximated\n");
        printf("              by the normal distribution if the number of Blocks\n");
        printf("              is greater than equal to 8.\n\n");
        printf("         Rejection Threshold:\n");
        printf("              Based on a %2.0f%% confidence interval, the Z-score should\n",
                              Z2tail[z_ind].perc_interior*100.0);
        printf("              be greater than %4.2f or less than -%4.2f.\n\n",Z2tail[z_ind].z,
                          Z2tail[z_ind].z);
        printf("         Decision:\n");
        printf("              The Z statistic = %4.2f, therefore the Hypothesis\n",test_stat);
        if (fabs(test_stat) > Z2tail[z_ind].z){
            printf("              is REJECTED in favor of the Null Hypothesis.\n\n");
            printf("              Further, %s is the better System.\n",
                   (better == 'a') ? treat1_str : treat2_str);
        } else
            printf("              is ACCEPTED.\n");
        form_feed();
    }
    if (fabs(test_stat) > Z2tail[z_ind].z)
        return(TEST_DIFF * ((better == 'a') ? (-1) : 1));
    return(NO_DIFF);
}

float wilcoxon_stat(int n, int rank_sum)
{
    int i, maxx, j, dig,sum, max_hist;
    int *ptr, dbg=0, dbg2=0;
    int *hist;
    int acc=0, tail_sum=0;


    maxx= pow(2.0,(float)n);
    max_hist = (n * (n+1)) / 2;
    if (dbg2){ 
        printf("Computing the Percentage of the Wilcoxon Stat\n"); 
        printf("           n=%d\n",n);
        printf("           Total combinations = %d\n",maxx);
        printf("           Total range = %d\n",max_hist);
        printf("           Rank Sum = %d\n",rank_sum);
    }	

    alloc_singarr(ptr,n,int);
    alloc_singarr(hist,max_hist+1,int);


    for (i=0; i <= max_hist; i++)
        hist[i] = 0;
    for (i=0; i < n; i++)
        ptr[i]=0;   
    for (i=0; i < maxx; i++){
        dig=0;
        while (ptr[dig] == 1){
           ptr[dig] = 0;
           dig ++;
        }
        ptr[dig] = 1;
        if (dbg)
            for (j=0; j < n; j++)
                printf(" %d",ptr[j]);
        sum=0;
        for (j=0; j < n; j++)
            sum += (ptr[j] ? j+1 : 0);
        if (dbg) printf("  %d \n",sum);
        hist[sum] ++;
    }
    for (i=0, tail_sum=0; i <= max_hist; i++){
        acc+= hist[i];
        if (dbg2) printf("hist[%3d] = %d   ->  %5.3f   %5.3f\n",i,hist[i],
                         (float)hist[i] / (float)maxx,
                         (float)acc / (float)maxx);
        if (rank_sum >= i)
            tail_sum += hist[i];
    }
    if (dbg2)
        printf("Wilcoxon Percent of 0 to %d is %5.2f%%\n",rank_sum,
               (float)tail_sum / (float)maxx * 100.0);
    return((float)tail_sum / (float)maxx);
     
}

        
        
        
    
