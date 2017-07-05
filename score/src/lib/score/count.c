/**********************************************************************/
/*                                                                    */
/*             FILENAME:  count.c                                     */
/*             BY:  Jonathan G. Fiscus                                */
/*             DATE: May 31 1989                                      */
/*                   NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY   */
/*                   SPEECH RECOGNITION GROUP                         */
/*                                                                    */
/*             USAGE:  This file contains procedures to load a COUNT  */
/*                     structure as well as routines to prints it out */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/**********************************************************************/
/*   this procedure takes a SYS_ALIGN_LIST and count the number of    */
/*   correct, substituted, inserted, and deleted words for every sent */
/*   for all systems represented in the SYS_ALIGN_LIST                */
/**********************************************************************/
void count_errors(SYS_ALIGN_LIST *sa_list, COUNT ****count)
{
    int sys, spkr, snt, wrd; 

    fprintf(stderr,"counting errors \n");
    for (sys=0;sys<num_sys(sa_list);sys++){
        for (spkr=0;spkr < num_spkr(sys_i(sa_list,sys)); spkr++){
            for (snt=0;snt < num_sents(sys_i(sa_list,sys),spkr); snt++){
                for (wrd = 0;ref_word(sys_i(sa_list,sys),spkr,snt,wrd) !=
                             END_OF_WORDS; wrd ++){
                    if (is_CORR(eval_wrd(sys_i(sa_list,sys),spkr,snt,wrd)))
                        corr(count,sys,spkr,snt)++;
                    else if (is_INS(eval_wrd(sys_i(sa_list,sys),spkr,snt,wrd)))
                        inserts(count,sys,spkr,snt)++;
                    else if (is_DEL(eval_wrd(sys_i(sa_list,sys),spkr,snt,wrd)))
                        del(count,sys,spkr,snt)++;
                    else if (is_SUB(eval_wrd(sys_i(sa_list,sys),spkr,snt,wrd)))
                        subs(count,sys,spkr,snt)++;
          	    else if (is_MERGE(eval_wrd(sys_i(sa_list,sys),spkr,snt,wrd)))
                        mrgs(count,sys,spkr,snt)++;	
		    else if (is_SPLIT(eval_wrd(sys_i(sa_list,sys),spkr,snt,wrd)))
                        spls(count,sys,spkr,snt)++;	
                    else {
                        fprintf(stderr,"Error: unknown evaluation type %x\n",
                                  eval_wrd(sys_i(sa_list,sys),spkr,snt,wrd));
                        exit(-1);
                    }
		}
	    }
	}
    }
}

/**********************************************************************/
/*   this routine calculates the numbers of matching correct sentences*/
/*   for every  system comparison.                                    */
/**********************************************************************/
void count_mtch_snt_err(SYS_ALIGN_LIST *sa_list, COUNT ****count)
{
    int sys, spkr, snt;
    int to_sys;

    fprintf(stderr,"counting matching correct sentences.\n");
    for (sys=0;sys<num_sys(sa_list);sys++){
        for (spkr=0;spkr < num_spkr(sys_i(sa_list,sys)); spkr++){
            for (snt=0;snt < num_sents(sys_i(sa_list,sys),spkr); snt++)
                /**** if there were no errors for this sentence */
                if (inserts(count,sys,spkr,snt) + del(count,sys,spkr,snt) +
                    subs(count,sys,spkr,snt) + mrgs(count,sys,spkr,snt) +
		    spls(count,sys,spkr,snt) == 0)
                    /**** check all the other systems to see if they */
                    /**** had it right                               */
                    for (to_sys=0;to_sys<num_sys(sa_list);to_sys++)
                        if (inserts(count,to_sys,spkr,snt) +
			    del(count,to_sys,spkr,snt) +
			    mrgs(count,to_sys,spkr,snt) +
			    spls(count,to_sys,spkr,snt) +
			    subs(count,to_sys,spkr,snt) == 0)
			  m_corr(count,sys,spkr,snt,to_sys)++;
	}
    }
}

/**********************************************************************/
/*   this function dumps out the contents of the COUNT structure      */
/*   by each speaker and system                                       */
/**********************************************************************/
void dump_COUNT(SYS_ALIGN_LIST *sa_list, COUNT ****count)
{
    int cor_trt,tmp,sys,spkr,snt;
    int sum_ins_row, sum_del_row, sum_subs_row, sum_corr_row;
    int sum_spls_row, sum_mrgs_row;
    int *sum_ins_col, *sum_del_col;
    int *sum_subs_col, *sum_corr_col;
    int *sum_mrgs_col, *sum_spls_col;
    int **sum_cc_col;
    char pad[FULL_SCREEN];

    alloc_singarr(sum_ins_col,num_sys(sa_list),int);
    alloc_singarr(sum_del_col,num_sys(sa_list),int);
    alloc_singarr(sum_subs_col,num_sys(sa_list),int);
    alloc_singarr(sum_mrgs_col,num_sys(sa_list),int);
    alloc_singarr(sum_spls_col,num_sys(sa_list),int);
    alloc_singarr(sum_corr_col,num_sys(sa_list),int);
    alloc_2dimarr(sum_cc_col,num_sys(sa_list),num_sys(sa_list),int);

/* top line */
    set_pad_cent_n(pad,num_sys(sa_list)*8 + 16 + 11);
    printf("%s%s",pad,"|---------------");
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("--------");
    printf("-----------|\n");
/* second line */
    printf("%s|\\  %-8s    |",pad,"SYSTEM");
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("       |");
    printf("| T Corr  |\n");
/* third line */
    printf("%s%s",pad,"| \\___________  |");
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("  %-4s |",sys_name(sys_i(sa_list,sys)));
    printf("| T Del   |\n");
/* fourth line */
    printf("%s%s",pad,"|             \\ |");
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("       |");
    printf("| T Ins   |\n");
/* fifth line */
    printf("%s| %-8s     \\|",pad,"SPKR");
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("       |");
    printf("| T Subs  |\n");
/* sixth line */
    printf("%s| %-8s      |",pad,"");
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("       |");
    printf("| T Split |\n");
/* seventh line */
    printf("%s| %-8s      |",pad,"");
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("       |");
    printf("| T Merge |\n");
/* reset all accumulators */
    for (sys=0;sys<num_sys(sa_list);sys++){
        sum_corr_col[sys]=0;
        sum_ins_col[sys]=0;
        sum_subs_col[sys]=0;
        sum_spls_col[sys]=0;
        sum_mrgs_col[sys]=0;
        sum_del_col[sys]=0;
        for (cor_trt=0;cor_trt<num_sys(sa_list);cor_trt++)
            sum_cc_col[sys][cor_trt] = 0;
    }
/* print the blocks and treatments */
    for (spkr=0;spkr<num_spkr(sys_i(sa_list,0));spkr++){
      /* reset all acumulators */
        sum_corr_row=sum_ins_row=sum_subs_row=0;
        sum_del_row=sum_spls_row=sum_mrgs_row=0;
         
      /* separation line */
        printf("%s%s",pad,"|---------------");
        for (sys=0;sys<num_sys(sa_list);sys++)
            printf("+-------");
        printf("++---------|\n");
      /* print number correct */
        printf("%s|      # Corr   ",pad);
        for (sys=0;sys<num_sys(sa_list);sys++){
            for (snt=0,tmp=0;snt<num_sents(sys_i(sa_list,sys),spkr);snt++)
                tmp+=corr(count,sys,spkr,snt);
            printf("| %4d  ",tmp);
            sum_corr_row+=tmp;
            sum_corr_col[sys]+=tmp;
	}
        printf("||  %4d   |\n",sum_corr_row);
      /* print number deletions */
        printf("%s| ",pad);
        print_substr(stdout,5,spkr_name(sys_i(sa_list,0),spkr));
        printf("# Del    ");
        for (sys=0;sys<num_sys(sa_list);sys++){
            for (snt=0,tmp=0;snt<num_sents(sys_i(sa_list,sys),spkr);snt++)
                tmp+=del(count,sys,spkr,snt);
            printf("| %4d  ",tmp);
            sum_del_row += tmp;
            sum_del_col[sys] += tmp;
	}
        printf("||  %4d   |\n",sum_del_row);
      /* print number insertions  */
        printf("%s|      # Ins    ",pad);
        for (sys=0;sys<num_sys(sa_list);sys++){
            for (snt=0,tmp=0;snt<num_sents(sys_i(sa_list,sys),spkr);snt++)
                tmp+=inserts(count,sys,spkr,snt);
            printf("| %4d  ",tmp);
            sum_ins_row += tmp;
            sum_ins_col[sys] += tmp;
	}
        printf("||  %4d   |\n",sum_ins_row);
      /* print number substitutions  */
        printf("%s|      # Subs   ",pad);
        for (sys=0;sys<num_sys(sa_list);sys++){
            for (snt=0,tmp=0;snt<num_sents(sys_i(sa_list,sys),spkr);snt++)
                tmp+=subs(count,sys,spkr,snt);
            printf("| %4d  ",tmp);
            sum_subs_row += tmp;
            sum_subs_col[sys] += tmp;
	}
        printf("||  %4d   |\n",sum_subs_row);
      /* print number splits  */
        printf("%s|      # Spilts ",pad);
        for (sys=0;sys<num_sys(sa_list);sys++){
            for (snt=0,tmp=0;snt<num_sents(sys_i(sa_list,sys),spkr);snt++)
                tmp+=spls(count,sys,spkr,snt);
            printf("| %4d  ",tmp);
            sum_spls_row += tmp;
            sum_spls_col[sys] += tmp;
	}
        printf("||  %4d   |\n",sum_spls_row);
      /* print number merges  */
        printf("%s|      # Merges ",pad);
        for (sys=0;sys<num_sys(sa_list);sys++){
            for (snt=0,tmp=0;snt<num_sents(sys_i(sa_list,sys),spkr);snt++)
                tmp+=mrgs(count,sys,spkr,snt);
            printf("| %4d  ",tmp);
            sum_mrgs_row += tmp;
            sum_mrgs_col[sys] += tmp;
	}
        printf("||  %4d   |\n",sum_mrgs_row);
        if (TRUE)
            for (cor_trt=0;cor_trt<num_sys(sa_list);cor_trt++){
                printf("%s|     CCor %4s ",pad,
                             sys_name(sys_i(sa_list,cor_trt)));
                for (sys=0;sys<num_sys(sa_list);sys++){
                    for (snt=0,tmp=0;snt<num_sents(sys_i(sa_list,sys),spkr);
                                                                        snt++)
                        tmp+=m_corr(count,sys,spkr,snt,cor_trt);
                    printf("| %4d  ",tmp);
                    sum_cc_col[cor_trt][sys] += tmp;
		}
                printf("||         |\n");
	    }

    }
/* total line */
    printf("%s%s",pad,"|===============");
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("========");
    printf("===========|\n");
/* percent sum of correct for recognizer */
    printf("%s| Total # Corr  ",pad);
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("| %4d  ",sum_corr_col[sys]);
    printf("||         |\n");
/* percent sum of deletions for recognizer */
    printf("%s| Total # Del   ",pad);
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("| %4d  ",sum_del_col[sys]);
    printf("||         |\n");
/* percent sum of insertions for recognizer */
    printf("%s| Total # Ins   ",pad);
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("| %4d  ",sum_ins_col[sys]);
    printf("||         |\n");
/* percent sum of substitutions for recognizer */
    printf("%s| Total # Subs  ",pad);
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("| %4d  ",sum_subs_col[sys]);
    printf("||         |\n");
/* percent sum of splits for recognizer */
    printf("%s| Total # Split ",pad);
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("| %4d  ",sum_spls_col[sys]);
    printf("||         |\n");
/* percent sum of merges for recognizer */
    printf("%s| Total # Merge ",pad);
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("| %4d  ",sum_mrgs_col[sys]);
    printf("||         |\n");
/* optional corresponding sentences totals */
    if (TRUE)
        for (cor_trt=0;cor_trt<num_sys(sa_list);cor_trt++){
            printf("%s| Tot CCor %4s ",pad,
                         sys_name(sys_i(sa_list,cor_trt)));
            for (sys=0;sys<num_sys(sa_list);sys++)
                printf("| %4d  ",sum_cc_col[cor_trt][sys]);
            printf("||         |\n");
	}
/* bottom line */
    printf("%s%s",pad,"|---------------");
    for (sys=0;sys<num_sys(sa_list);sys++)
        printf("--------");
    printf("-----------|\n");


    free_singarr(sum_ins_col,int);
    free_singarr(sum_del_col,int);
    free_singarr(sum_spls_col,int);
    free_singarr(sum_mrgs_col,int);
    free_singarr(sum_subs_col,int);
    free_singarr(sum_corr_col,int);
    free_2dimarr(sum_cc_col,num_sys(sa_list),int);
}

