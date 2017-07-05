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
/*   this procedure does all the system comparisons then it         */
/*   prints a report to stdout                                      */
/********************************************************************/
void McNemar_sent(SYS_ALIGN_LIST *sa_list, COUNT ****count, int **winner, char *testname, int print_results, int verbose)
{
    int comp1, comp2;
    int result;
    int *sum_winner;

    if (!print_results) verbose = FALSE;
    fprintf(stderr,"entering sentence McNemar analysis\n");
    alloc_singZ(sum_winner,num_sys(sa_list),int,0);

    for (comp1=0; comp1 <(num_sys(sa_list)-1); comp1++)
        for (comp2=comp1+1; comp2<(num_sys(sa_list)); comp2++){
            result = do_McNemar_by_sent(sa_list,count,comp1,comp2,verbose);
            winner[comp1][comp2] = result;
            sum_winner[comp2] += result * (-1); /*invert for reflective prop*/
            sum_winner[comp1] += result;
        }
    if (print_results)
        print_compare_matrix_for_sent_M(sa_list,count,winner,
                                        sum_winner,testname);
}

/********************************************************************/
/*   using the COUNT structure, calculate the matrix of the McNemar */
/*   test then perform the test                                     */
/********************************************************************/
int do_McNemar_by_sent(SYS_ALIGN_LIST *sa_list, COUNT ****count, int sys1, int sys2, int verbose)
{
    int i, j, ans, spkr, snt, mc=0, cor1=0, cor2=0, tot_sents=0;
    static int **table=NULL;

    if (table == NULL) alloc_int_2dimarr(table,2,2);

    /* clear the table */
    for (i=0;i<2;i++)
        for (j=0;j<2;j++)
           table[i][j] = 0;

    /* count the matched correct and the number correct for both systems */
    for (spkr=0; spkr<num_spkr(sys_i(sa_list,sys1)); spkr++){
        for (snt=0; snt<num_sents(sys_i(sa_list,sys1),spkr); snt++){
            mc += m_corr(count,sys1,spkr,snt,sys2);
            cor1 += m_corr(count,sys1,spkr,snt,sys1);
            cor2 += m_corr(count,sys2,spkr,snt,sys2);
	}
        tot_sents += num_sents(sys_i(sa_list,sys1),spkr);
    }
    
 
    table[0][0] = mc;                      /*number corr by both sys*/
    table[0][1] = cor1 -                   /*number corr by sys1 but*/
                  table[0][0];             /* not sys2 */
    table[1][0] = cor2 -                   /*number corr by sys2 but*/
                  table[0][0];             /* not sys1 */
    table[1][1] = tot_sents -              /* number missed by both   */
                  (table[0][0] + table[0][1] + table[1][0]);  
                                           /* sys1 and sys2 */
    ans = do_McNemar(table,sys_name(sys_i(sa_list,sys1)),
                           sys_name(sys_i(sa_list,sys2)),verbose);
    free_2dimarr(table,2,int);
    return(ans);
}

/********************************************************************/
/*   this procedure is called to print the results of the McNemar   */
/*   test to stdout in a matrix format                              */
/********************************************************************/
void print_compare_matrix_for_sent_M(SYS_ALIGN_LIST *sa_list, COUNT ****count, int **winner, int *s_winner, char *tname)
{
    char *matrix_name = 
      "COMPARISON MATRIX: McNEMAR\'S TEST ON CORRECT SENTENCES FOR THE TEST:";
    char *what_sys    = "For all systems";
    int i,j,k,sys,spkr,snt, *corr_arr, max_name_len=0, block_size;
    int hyphen_len=49, space_len=49;
    char pad[FULL_SCREEN], hyphens[50], spaces[50], sysname_fmt[40];

    alloc_int_singarr(corr_arr,num_sys(sa_list));

/* init the hyphens and spaces array */
    for (i=0; i<hyphen_len; i++){
         hyphens[i]=HYPHEN; 
         spaces[i]=SPACE;
    }
    hyphens[49]=NULL_CHAR; spaces[49]=NULL_CHAR;
    /* find the largest system length */
    for (sys=0;sys<num_sys(sa_list);sys++)
      if ((i=strlen(sys_name(sys_i(sa_list,sys)))) > max_name_len)
         max_name_len = i;
    block_size = (7+max_name_len);
    sprintf(sysname_fmt," %%%1ds(%%3d)|",max_name_len);

    /* calc the number of correct for each system */
    for (sys=0;sys<num_sys(sa_list);sys++){
        corr_arr[sys] = 0;
        for (spkr=0;spkr<num_spkr(sys_i(sa_list,sys));spkr++)
            for (snt=0;snt<num_sents(sys_i(sa_list,sys),spkr);snt++)
                corr_arr[sys] += m_corr(count,sys,spkr,snt,sys);
    }

    /*  print the header */
    set_pad(pad,matrix_name);
    printf("\n\n\n%s%s\n",pad,matrix_name);
    set_pad(pad,tname);
    printf("%s%s\n",pad,tname);
    set_pad_cent_n(pad,strlen(what_sys));
    printf("%s%s\n\n",pad,what_sys);
    set_pad_cent_n(pad,(num_sys(sa_list)+1) * block_size);

    printf("\n%s|%s",pad,hyphens+hyphen_len-(block_size-1));
    for (k=0;k<num_sys(sa_list);k++)
        printf("%s",hyphens+hyphen_len-block_size);
    printf("|\n");

    printf("%s|%s|",pad,spaces+space_len-(block_size-1));
    for (i=0;i<num_sys(sa_list);i++)
        printf(sysname_fmt,sys_name(sys_i(sa_list,i)),corr_arr[i]);
    printf("\n");

    for (i=0;i<num_sys(sa_list);i++){
        printf("%s|%s",pad,hyphens+hyphen_len-(block_size-1));
        for (k=0;k<num_sys(sa_list);k++)
            printf("+%s",hyphens+hyphen_len-(block_size-1));
        printf("|\n");
        printf("%s|",pad);
        printf(sysname_fmt,sys_name(sys_i(sa_list,i)),corr_arr[i]);
        for (j=0;j<num_sys(sa_list);j++)
            if (j > i){ int t;
                t=(block_size-8)/2;
                printf("%sD=(%3d)%s|",
                            spaces+space_len-t,
                            abs(corr_arr[i]-corr_arr[j]),
                            spaces+space_len-((block_size-8)-t));
            }
            else
                printf("%s",spaces+space_len-(block_size-1)), printf("|");
        printf("\n%s|%s|",pad,spaces+space_len-(block_size-1));
        for (j=0;j<num_sys(sa_list);j++){
            char *name="";
            int t=0;
            if (j > i){
                name=spaces+space_len-(block_size-1);
                if (winner[i][j] == TEST_DIFF)
                    name=sys_name(sys_i(sa_list,j));
                else if (winner[i][j] == NO_DIFF)
                    name="same";
                else
                    name=sys_name(sys_i(sa_list,i));
                t = (block_size-1-strlen(name))/2;
            }
            printf("%s%s%s|",spaces+space_len-t,name,
                             spaces+space_len-(block_size-1-strlen(name)-t));
        }
        printf("\n");
    }
    printf("%s|%s",pad,hyphens+hyphen_len-(block_size-1));
    for (k=0;k<num_sys(sa_list);k++)
        printf("%s",hyphens+hyphen_len-block_size);
    printf("|\n");

    form_feed();
}


