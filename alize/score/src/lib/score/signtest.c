/**********************************************************************/
/*                                                                    */
/*           FILE: signtest.c                                         */
/*           WRITTEN BY: Jonathan G. Fiscus                           */
/*           DATE: April 14 1989                                      */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           USAGE: This uses the rank structure to perform           */
/*                  the non-parametric Sign Test and generates        */
/*                  a report.                                         */
/*                                                                    */
/*           SOURCE:Statistics: Basic Techniques for Solving          */
/*                  Applied Problems, by Stephen A. Book, 1977        */
/*                                                                    */
/*           See Also: The documentation file "signtest.doc"          */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/****************************************************************/
/*   main procedure to perform the sign test on the RANK        */
/*   structure.                                                 */
/****************************************************************/
void perform_signtest(RANK *rank, int verbose, int report, char *formula_str, int formula_id, int **winner)
{
    int comp1, comp2;

    if (!report) verbose=FALSE;
    for (comp1=0; comp1 <rnk_trt(rank) -1; comp1++)
        for (comp2=comp1+1; comp2<rnk_trt(rank); comp2++){
            winner[comp1][comp2] = 
               compute_signtest_for_treatment(rank,comp1,comp2,"Spkr",
                                              formula_str,verbose,formula_id==TOTAL_ERROR);
        }

    if (report) print_trt_comp_matrix_for_RANK_one_winner(winner,rank,
                 "Comparison Matrix for the Sign Test",formula_str,
                 "Speaker");
}

/****************************************************************/
/*   Given the two indexes of treatments to compare (in the     */
/*   RANK Struct) compute the Rank Sum statistics               */
/*   vars:                                                      */
/*       zero_is_best : This identifies the "ideal" value for   */
/*                      the value computed in the rank struct   */
/*                      percentages.                            */
/****************************************************************/
int compute_signtest_for_treatment(RANK *rank, int treat1, int treat2, char *block_id, char *formula_str, int verbose, int zero_is_best)
{
    int sum_plus=0, sum_minus=0, sum_equal=0, i;
    int block, max_len_block=0, max_len_treat=6;
    int tptr[2]; /* a sorting pointer array for the treatment numbers */
    char pct_format[20], thresh_str[140];
    char title_line[200], diff_line[40];
    int paper_width = 79, rep_width, diff_col_len;
    float equal_thresh = 0.005;
    float sum_trt1=0.0, sum_trt2=0.0;

    /* compute the maximum block title length */
    max_len_block = strlen(block_id);
    for (block=0; block < rnk_blks(rank); block++)
         if (strlen(Vrnk_b_name(rank,block)) > max_len_block)
             max_len_block = strlen(Vrnk_b_name(rank,block));

    if (max_len_treat < strlen(Vrnk_t_name(rank,treat1)))
        max_len_treat = strlen(Vrnk_t_name(rank,treat1));
    if (max_len_treat < strlen(Vrnk_t_name(rank,treat2)))
        max_len_treat = strlen(Vrnk_t_name(rank,treat2));

    /* set the treatment sorting array */
    for (block=0; block < rnk_blks(rank); block++){
        sum_trt1 += Vrnk_pcts(rank,block,treat1);
        sum_trt2 += Vrnk_pcts(rank,block,treat2);
    }
    if (sum_trt1 > sum_trt2)
        tptr[0] = treat1, tptr[1] = treat2;
    else
        tptr[0] = treat2, tptr[1] = treat1;
/*    if (zero_is_best)
        i = tptr[0], tptr[0] = tptr[1], tptr[1] = i;*/

    /* set up format strings and titles */
    sprintf(thresh_str,"(Threshold for equal percentages +- %5.3f)",
                       equal_thresh);
    pct_format[0] = NULL_CHAR;
    strcat(pct_format,center("",(max_len_treat-6)/2));
    strcat(pct_format,"%6.2f");
    strcat(pct_format,center("",max_len_treat - ((max_len_treat-6)/2 + 6) ));
    sprintf(diff_line,"[%s - %s]",Vrnk_t_name(rank,tptr[0]),Vrnk_t_name(rank,tptr[1]));
    diff_col_len = 15;
    if (strlen(diff_line) > diff_col_len) diff_col_len=strlen(diff_line);

    rep_width = max_len_block + max_len_treat * 2 + 4 * 3 + diff_col_len;

    /*  Print a detailed table showing sign differences */
    if (verbose) {
        sum_trt1 = sum_trt2 = 0.0;
        printf("%s\n",center("Sign Test Calculations Table Comparing",paper_width));
        sprintf(title_line,"%s %s Percentages for Systems %s and %s",
                            block_id, formula_str,Vrnk_t_name(rank,tptr[0]),
                            Vrnk_t_name(rank,tptr[1]));
        printf("%s\n",center(title_line,paper_width));
        printf("%s\n\n",center(thresh_str,paper_width));

        printf("%s",center("",(paper_width - rep_width)/2));
        printf("%s",center("",max_len_block));
        printf("    "); printf("%s",center("",max_len_treat));
        printf("    "); printf("%s",center("",max_len_treat));
        printf("    ");
        printf("%s\n",center("Difference Sign",diff_col_len));

        printf("%s",center("",(paper_width - rep_width)/2));
        printf("%s",center(block_id,max_len_block));
        printf("    ");
        printf("%s",center(Vrnk_t_name(rank,tptr[0]),max_len_treat));
        printf("    ");
        printf("%s",center(Vrnk_t_name(rank,tptr[1]),max_len_treat));
        printf("    ");
        printf("%s\n",center(diff_line,diff_col_len));
        printf("%s",center("",(paper_width - rep_width)/2));
        for (i=0; i<rep_width; i++)
            printf("-");
        printf("\n");
    }
    for (block=0; block < rnk_blks(rank); block++){
         if (verbose) {
             printf("%s",center("",(paper_width - rep_width)/2));
             printf("%s",center(Vrnk_b_name(rank,block),max_len_block));
             printf("    ");
             printf(pct_format,Vrnk_pcts(rank,block,tptr[0]));
             printf("    ");
             printf(pct_format,Vrnk_pcts(rank,block,tptr[1]));
             printf("    ");
         }
         if (fabs(Vrnk_pcts(rank,block,tptr[0])-Vrnk_pcts(rank,block,tptr[1])) <=
                  equal_thresh){
             if (verbose) printf("%s\n",center("0",diff_col_len));
             sum_equal++;
         }
         else if (Vrnk_pcts(rank,block,tptr[0]) < Vrnk_pcts(rank,block,tptr[1])){
             if (verbose) printf("%s\n",center("-",diff_col_len));
             sum_minus++;
         }
         else {
             if (verbose) printf("%s\n",center("+",diff_col_len));
             sum_plus++;
         }
         sum_trt1 += Vrnk_pcts(rank,block,tptr[0]); 
         sum_trt2 += Vrnk_pcts(rank,block,tptr[1]);
    }
    if (verbose){
        printf("%s",center("",(paper_width - rep_width)/2));
        for (i=0; i<rep_width; i++)
            printf("-");
        printf("\n");
        /* an Average line */
        printf("%s",center("",(paper_width - rep_width)/2));
        printf("%s",center("Avg.",max_len_block));
        printf("    ");
        printf(pct_format,sum_trt1 / rnk_blks(rank));
        printf("    ");
        printf(pct_format,sum_trt2 / rnk_blks(rank));
        printf("\n\n");

        sprintf(title_line,"No. Speakers with Positive %s Differences = N(+) = %2d",formula_str,sum_plus);
        printf("%s\n",center(title_line,paper_width));
        sprintf(title_line,"No. Speakers with Negative %s Differences = N(-) = %2d",formula_str,sum_minus);
        printf("%s\n",center(title_line,paper_width));
        sprintf(title_line,"No. Speakers with No %s Differences = N(0) = %2d",formula_str,sum_equal);
        printf("%s\n",center(title_line,paper_width));
        printf("\n\n");
    }
    
    /* Analyze the Results */
    { int result;
      result = sign_test_analysis(sum_plus,sum_minus,sum_equal,"+","-",0,
				  0.05,verbose, Vrnk_t_name(rank,tptr[0]),
				  Vrnk_t_name(rank,tptr[1]),
				  tptr,zero_is_best);
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
int sign_test_analysis(int num_a, int num_b, int num_z, char *str_a, char *str_b, int str_z, float alpha, int verbose, char *treat1_str, char *treat2_str, int *tptr, int zero_is_best)
{
    float test_stat, p=0.5;
    int decision_cutoff=(-1), i;

    num_b += (num_z / 2) + (num_z % 2);
    num_a += (num_z / 2);
    num_z = 0;
    test_stat = compute_acc_binomial(num_b,num_a+num_b+num_z,p);

    for (i=0; i< (num_a + num_b); i++)
        if (compute_acc_binomial(i,num_a+num_b,p) < alpha)
            decision_cutoff=i;

    if (verbose){
        printf("The NULL Hypothesis:\n\n");
        printf("     The number of speakers for which the differences is positive\n");
        printf("     equals the number of speakers for which the differences is\n");
        printf("     negative.\n");
        printf("			 P(N(+)) = P(N(-)) = 0.50\n\n");

        printf("Alternate Hypothesis:\n\n");
        printf("     The number of speakers for which the differences is positive \n");
        printf("     is NOT equal to the number of speakers for which the difference\n");
        printf("     is negative.\n\n");
        printf("Decision Analysis:\n\n");
        printf("     Assumptions:\n");
        printf("        A1: The distibution of positive and negative differences\n");
        printf("            follows the binomial distribution for N fair coin tosses.\n");
        printf("\n");
        printf("        A2: In order to resolve the complication caused by cases where the\n");
        printf("            difference in Word Accuracy is zero, half of the cases will\n");
        printf("            be assigned to N(+) and half to N(-).  In the event of an\n");
        printf("            odd number of zero differences, N(-) will get one extra, this\n");
        printf("            reduces the probability of there being a difference between\n");
        printf("            the two systems.\n\n");
        printf("     Rejection criterion:\n");
        printf("        Reject the null hypothesis at the 95%% confidence level based\n");
        printf("        on the following critical values table.  N is the number of\n");
        printf("        speakers being compared and N(-) is the number of negative\n");
        printf("        differences.\n\n");
        /* print a table of critical values */
        printf("                 N(-)      P(N(-) | N=%2d)\n",num_a+num_b+num_z);

        printf("                 ----      --------------\n");
        for (i=0; compute_acc_binomial(i-1,num_a+num_b+num_z,p) < (MAX(0.25,test_stat)); i++){
            printf("                 %3d           %5.3f", i,
                   compute_acc_binomial(i,num_a+num_b+num_z,p));
            if ((compute_acc_binomial(i,num_a+num_b+num_z,p) < alpha) &&
                (compute_acc_binomial(i+1,num_a+num_b+num_z,p) > alpha))
                printf("  <---  Null Hypothesis rejection threshold\n");
            else
                printf("\n");
        }
        printf("\n");
        printf("     Decision:\n");

        printf("        There were N(-)=%d negative differences , the probability of\n",num_b);
        printf("        it occuring is %5.3f, therefore the null hypothesis ",test_stat);        
        if (test_stat < alpha){            
            printf("is REJECTED\n");
            printf("        in favor of the Alternate Hypothesis.  Further, %s is the\n",
                   (zero_is_best) ? treat2_str : treat1_str);
            printf("        better System.\n");
        } else{
            printf("is ACCEPTED\n");
            printf("        There is no statistical difference between %s and %s\n",treat1_str,treat2_str);
        }
        form_feed();
    }
    if (test_stat < alpha){
        if (0) printf("Returning Result %d\n",TEST_DIFF * ((zero_is_best) ? 1 : -1));
        return(TEST_DIFF * ((zero_is_best) ? 1 : -1));
      }
    return(NO_DIFF);
}

