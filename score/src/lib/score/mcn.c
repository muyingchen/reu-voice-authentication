/********************************************************************/
/*                                                                  */
/*           FILE: McNemar_sent.c                                   */
/*           WRITTEN BY: Jonathan G. Fiscus                         */
/*           DATE: May 31 1989                                      */
/*                 NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY   */
/*                 SPEECH RECOGNITION GROUP                         */
/*                                                                  */
/*           USAGE: This program, performs a McNemar test on the    */
/*                  list of SYS_ALIGN structures                    */
/*                  The test statistics models the CHI SQUARE dist  */
/*                  and is calculated from the McNemar matrix:      */
/*                                                                  */
/*                McNemar matrix:                                   */
/*                          sys1            C: denotes a corr sent  */
/*                         C     E          E: denotes a error sent */
/*                      -------------       TC: sum of C            */
/*                    C |  a  |  b  | TC    TE: sum of E            */
/*               sys2   |-----+-----|                               */
/*                    E |  c  |  d  | TE    element definitions:    */
/*                      -------------         a: num corr by both   */
/*                        TC    TE               systems            */
/*                                            c: num corr by sys1   */
/*                                               but not by sys2    */
/*                                            b: num corr by sys2   */
/*                                               but not by sys1    */
/*                                            d: num errored on by  */
/*                                               both systems       */
/*           Test Statistic formula:                                */
/*                                    2                             */
/*                        ( |b-c| -1 )                              */
/*                   TS = ------------                              */
/*                             b+c                                  */
/*                                                                  */
/********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/********************************************************************/
/*   given the McNemar matrix, come up with an answer.  verbose if  */
/*   desired                                                        */
/********************************************************************/
int do_McNemar(int **table, char *name1, char *name2, int verbose)
{
    
  if (verbose){
   printf("\n\n");
   printf("                            McNemar test results\n");
   printf("                            ====================\n\n");
   printf("                                     %s\n\n",name2);
   printf("                                  corr     incorr\n");
   printf("      %15s      corr   %3d        %3d\n",name1,
                                      table[0][0],table[0][1]);
   printf("\t                 incorr   %3d        %3d\n",
                                      table[1][0],table[1][1]);
 }
 if ((table[0][1] == 0) && (table[1][0] == 0)){
   /* if both of these feilds are 0 then there is no difference so say so*/
   /* otherwise the calculation blows up                                 */
   if (verbose){
     printf("\n\n\t\tSUMMARY:\n\t\t-------\n\n");
     printf("\n\n\tThe two totals for words missed by either test results\n");
     printf("\tare both zero, therfore there is no significant difference\n");
     printf("\tbetween the two tests!\n");
   }
     return(NO_DIFF);
 }
 else
   return(perform_peregoy_method(table,verbose));
}

/********************************************************************/
/*   this program calculates the test statistic corresponding to a  */
/*   CHI SQUARED distribution. the formula is at the top of this    */
/*   file.                                                          */
/*   references to the 'Peregoy method' are adapted from            */
/*   Appendix B "The Significance of Bench Mark Test Results" by    */
/*   Peter Peregoy in American National Standard IAI 1-1987.        */
/*   Other distributions might be assumed.                          */
/********************************************************************/
int perform_peregoy_method(int **table, int verbosely)
{
    int i;
    double ts_per; 
    extern double sqrt(double);

    /*  Perogoy method */
    ts_per = ((fabs((double)(table[0][1]-table[1][0]))-1)*
              (fabs((double)(table[0][1]-table[1][0]))-1)) / 
             (table[0][1]+table[1][0]);
    if (verbosely){
        printf("\n\n"); 
        printf("%30s     Reject if\n","");
        printf("%27sX > X2 of %s %s (%2.3f)\n", "",
                          X2.per_str[GEN_X2_PER],
                          X2.df[DF1].str,
                          X2.df[DF1].level[GEN_X2_PER]);
        printf("\n"); 
        printf("%30s   X = %2.3f\n","",ts_per);
        printf("\n\n\t\tSUMMARY:\n\t\t-------\n\n");
    }
    if (fabs(ts_per) > X2.df[DF1].level[GEN_X2_PER]){
        if (verbosely){
            printf("\tPeregoy's method shows that, with %s confidence, the\n",
                                 X2.neg_per_str[GEN_X2_PER]),
            printf("\t2 recognition systems are significantly different.\n");
            printf("\n");
            printf("\tFurther, the probablity of there being a difference is\n");
            for (i=GEN_X2_PER;i<MAX_X2_PER;i++)
                if (fabs(ts_per) < X2.df[DF1].level[i+1])
                    break;
            if (i==MAX_X2_PER)
                printf("\tgreater that %s.\n",X2.neg_per_str[i]);
            else
                printf("\tbetween %s to %s.\n",X2.neg_per_str[i],
                                               X2.neg_per_str[i+1]);
            printf("\n\n");
	}
        if (table[1][0] < table[0][1])
            return(TEST_DIFF * (-1)); /* invert because sys1 was better */
        else
            return(TEST_DIFF);
    }
    else{
        if (verbosely){ 
            printf("\tPeregoy's method shows that at the %s confidence\n",
                                  X2.neg_per_str[GEN_X2_PER]),
            printf("\tinterval, the 2 recognition systems are not significantly\n");
            printf("\tdifferent.\n\n");
            printf("\tFurther, the probablity of there being a difference is\n");
            for (i=GEN_X2_PER;i>MIN_X2_PER;i--)
                if (fabs(ts_per) > X2.df[DF1].level[i-1])
                    break;
            if (i==MIN_X2_PER)
                printf("\tless than %s.\n",X2.neg_per_str[i]);
            else
                printf("\tbetween %s to %s.\n",X2.neg_per_str[i-1],
                                         X2.neg_per_str[i]);
            printf("\n\n");
	}
        return(NO_DIFF);
    }
}

