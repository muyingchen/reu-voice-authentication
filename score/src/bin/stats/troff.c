/**********************************************************************/
/*                                                                    */
/*             FILENAME:  troff_tables.c                              */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains procedures to make troff file  */
/*                  that when ran through tbl and printed on a laser  */
/*                  printer will make "pretty" tables                 */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvf.h>
#include "defs.h"

/**********************************************************************/
/*   make a troff table to show both the McNemar and the Matched      */
/*   Pairs test results                                               */
/**********************************************************************/
void troff_print_mtch_and_MCN(SYS_ALIGN_LIST *sa_list, int **sent_McN_winner, int **mtch_pr_winner, char *testname)
{
    FILE *fp;
    char *title = "COMPARISON TABLE FOR SENTENCE THE SENTENCE MCNEMAR";
    char *titlea= "AND THE MATCHED PAIRS STATISTICAL TESTS";
    int row, col;
   
    if ((fp = fopen(MTCH_MCN_FILENAME,"w")) == NULL){
         fprintf(stderr,"Can not open output file %s\n",
		 MTCH_MCN_FILENAME);
	 return;
    }

    /* make the header */
    fprintf(fp,".sp 2.0i\n");
    if (*testname != NULL_CHAR){
        fprintf(fp,".ce\n");
        fprintf(fp,"%s\n",testname);
    }
    else {
        fprintf(fp,".ce\n");
        fprintf(fp,"%s\n",title);
        fprintf(fp,".ce\n");
        fprintf(fp,"%s\n",titlea);
    }
    fprintf(fp,".sp 0.5i\n");

    /* begin the table */
    fprintf(fp,".TS\n");
    fprintf(fp,"center box tab (/) ;\n");

    /* the table format statements */
    fprintf(fp,"ce ");
    for (col=0; col < num_sys(sa_list); col++)
        fprintf(fp,"| ce ");
    fprintf(fp,"\n");
    fprintf(fp,"le ");
    for (col=0; col < num_sys(sa_list); col++)
        fprintf(fp,"| le ");
    fprintf(fp,".\n");

    /* column titles */
    for (col=0; col < num_sys(sa_list); col++)
        fprintf(fp,"/%s",sys_name(sys_i(sa_list,col)));
    fprintf(fp,"\n");
    
    /* begin data inclusion */
    for (row=0; row < num_sys(sa_list); row++){
        fprintf(fp,"_\n");   /* a line to be drawn */
        /* top seg */
        fprintf(fp," %s",sys_name(sys_i(sa_list,row)));
        for (col=0; col < num_sys(sa_list); col++)
            if (row < col)
                if (sent_McN_winner[row][col] == 0)
                    fprintf(fp,"/N: same");
                else if (sent_McN_winner[row][col] == 1)
                    fprintf(fp,"/N: %s",sys_name(sys_i(sa_list,col)));
                else 
                    fprintf(fp,"/N: %s",sys_name(sys_i(sa_list,row)));
            else
                fprintf(fp,"/");
        fprintf(fp,"\n");
        /* the second segment */
        for (col=0; col < num_sys(sa_list); col++)
            if (row < col)
                if (mtch_pr_winner[row][col] == 0)
                    fprintf(fp,"/P: same");
                else if (mtch_pr_winner[row][col] == 1)
                    fprintf(fp,"/P: %s",sys_name(sys_i(sa_list,col)));
                else 
                    fprintf(fp,"/P: %s",sys_name(sys_i(sa_list,row)));
            else
                fprintf(fp,"/");
        fprintf(fp,"\n");
    }
    /* end the table */
    fprintf(fp,".TE\n");
    print_name_desc(sa_list,fp);
    fclose(fp);
}

/**********************************************************************/
/*   make a troff table to show the McNemar results                   */
/**********************************************************************/
void troff_print_MCN(SYS_ALIGN_LIST *sa_list, int **sent_McN_winner, char *testname)
{
    FILE *fp;
    char *title = "COMPARISON TABLE FOR SENTENCE THE SENTENCE MCNEMAR";
    int row, col;
   
    if ((fp = fopen(MCN_FILENAME,"w")) == NULL){
	fprintf(stderr,"Can not open output file %s\n",
		MTCH_MCN_FILENAME);
        return;
    }

    /* make the header */
    fprintf(fp,".sp 2.0i\n");
    if (*testname != NULL_CHAR){
        fprintf(fp,".ce\n");
        fprintf(fp,"%s\n",testname);
    }
    else {
        fprintf(fp,".ce\n");
        fprintf(fp,"%s\n",title);
    }
    fprintf(fp,".sp 0.5i\n");

    /* begin the table */
    fprintf(fp,".TS\n");
    fprintf(fp,"center box tab (/) ;\n");

    /* the table format statements */
    fprintf(fp,"ce ");
    for (col=0; col < num_sys(sa_list); col++)
        fprintf(fp,"| ce ");
    fprintf(fp,"\n");
    fprintf(fp,"le ");
    for (col=0; col < num_sys(sa_list); col++)
        fprintf(fp,"| le ");
    fprintf(fp,".\n");

    /* column titles */
    for (col=0; col < num_sys(sa_list); col++)
        fprintf(fp,"/%s",sys_name(sys_i(sa_list,col)));
    fprintf(fp,"\n");
    
    /* begin data inclusion */
    for (row=0; row < num_sys(sa_list); row++){
        fprintf(fp,"_\n");   /* a line to be drawn */
        /* top seg */
        fprintf(fp," %s",sys_name(sys_i(sa_list,row)));
        for (col=0; col < num_sys(sa_list); col++)
            if (row < col)
                if (sent_McN_winner[row][col] == 0)
                    fprintf(fp,"/N: same");
                else if (sent_McN_winner[row][col] == 1)
                    fprintf(fp,"/N: %s",sys_name(sys_i(sa_list,col)));
                else 
                    fprintf(fp,"/N: %s",sys_name(sys_i(sa_list,row)));
            else
                fprintf(fp,"/");
        fprintf(fp,"\n");
    }
    /* end the table */
    fprintf(fp,".TE\n");
    print_name_desc(sa_list,fp);
    fclose(fp);
}

/**********************************************************************/
/*   make a troff table to show the Matched Pairs results             */
/**********************************************************************/
void troff_print_mtch(SYS_ALIGN_LIST *sa_list, int **mtch_pr_winner, char *testname)
{
    FILE *fp;
    char *title = "COMPARISON TABLE FOR THE MATCHED PAIRS STATISTICAL TESTS";
    int row, col;
   
    if ((fp = fopen(MTCH_FILENAME,"w")) == NULL){
	fprintf(stderr,"Can not open output file %s\n",
		MTCH_MCN_FILENAME);
        return;
    }

    /* make the header */
    fprintf(fp,".sp 2.0i\n");
    if (*testname != NULL_CHAR){
        fprintf(fp,".ce\n");
        fprintf(fp,"%s\n",testname);
    }
    else {
        fprintf(fp,".ce\n");
        fprintf(fp,"%s\n",title);
    }
    fprintf(fp,".sp 0.5i\n");

    /* begin the table */
    fprintf(fp,".TS\n");
    fprintf(fp,"center box tab (/) ;\n");

    /* the table format statements */
    fprintf(fp,"ce ");
    for (col=0; col < num_sys(sa_list); col++)
        fprintf(fp,"| ce ");
    fprintf(fp,"\n");
    fprintf(fp,"le ");
    for (col=0; col < num_sys(sa_list); col++)
        fprintf(fp,"| le ");
    fprintf(fp,".\n");

    /* column titles */
    for (col=0; col < num_sys(sa_list); col++)
        fprintf(fp,"/%s",sys_name(sys_i(sa_list,col)));
    fprintf(fp,"\n");
    
    /* begin data inclusion */
    for (row=0; row < num_sys(sa_list); row++){
        fprintf(fp,"_\n");   /* a line to be drawn */
        /* top seg */
        fprintf(fp," %s",sys_name(sys_i(sa_list,row)));
        for (col=0; col < num_sys(sa_list); col++)
            if (row < col)
                if (mtch_pr_winner[row][col] == 0)
                    fprintf(fp,"/P: same");
                else if (mtch_pr_winner[row][col] == 1)
                    fprintf(fp,"/P: %s",sys_name(sys_i(sa_list,col)));
                else 
                    fprintf(fp,"/P: %s",sys_name(sys_i(sa_list,row)));
            else
                fprintf(fp,"/");
        fprintf(fp,"\n");
    }
    /* end the table */
    fprintf(fp,".TE\n");
    print_name_desc(sa_list,fp);
    fclose(fp);
}

/**********************************************************************/
/*   a general routine to print the system names a descriptions to    */
/*   a troff text file                                                */
/**********************************************************************/
void print_name_desc(SYS_ALIGN_LIST *sa_list, FILE *fp)
{
    int i;

    fprintf(fp,".sp 2.0i\n");
    for (i=0;i<num_sys(sa_list);i++){
        fprintf(fp,"                      %4s  -->  %-40s\n",
                                      sys_name(sys_i(sa_list,i)),
                                      sys_desc(sys_i(sa_list,i)));
    }
    fprintf(fp,".sp 0.5i\n");
    fprintf(fp,".ce\n");
    fprintf(fp,"N denotes McNemar,     P denotes Matched Pairs\n");
}
