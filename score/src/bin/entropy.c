/**********************************************************************/
/*                                                                    */
/*             FILENAME:  entropy.c                                   */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  computes the entropy of an alignment file         */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvp.h>
#include <score/scorelib.h>


#define MAX_FILENAME_LEN	80

#define DEFAULT_LEXICON		"drivers/rm/lexicon.snr"
#define DEFAULT_HOMO		"drivers/rm/homophn.txt"

void compute_entropy PROTO((SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, char *diskfile, int dbg));
void test_entropy PROTO((char *diskfile, int dbg));

#define PROG_NAME	"entropy"

#define CFG_DEBUG         "dbg"
#define CFG_DISK_FILE     "df"
#define CFG_ENTROPY_TEST  "test"

CONFIG score_config = {
  /*   intern id,     count,   type,    	groups, value  */
  {  { CFG_LEX,		1,	CFG_C,		2,	NULL} ,
     { CFG_CODESET,	1,	CFG_C,		2,	NULL} ,
     { CFG_LSUBS,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_ALI_FILE,	40,	CFG_C2,		NO_GRP,	NULL} ,
     { CFG_DUMPARG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_ENTROPY_TEST,1,	CFG_TGL,	NO_GRP,	NULL} ,
     { CFG_DEBUG,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_DISK_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { "",		0,	CFG_TGL,	NO_GRP,	TOGGLE_OFF}}
};
CONFIG *cfg = &score_config;

TABLE_OF_CODESETS codesets, *pcs = &(codesets);

int main(int argc, char **argv)
{
    SYS_ALIGN_LIST sa_list;
    int error, *perr = &error;

    print_version(argv[0]);
    alloc_CONFIG(cfg);
    set_CONFIG_elem_C(cfg,CFG_DEBUG,"0");
    set_CONFIG_elem_C(cfg,CFG_DISK_FILE,"/tmp/confusion_array");
    cfg_get_args(cfg,argc,argv,"");

    if (CONFIG_elem_TGL(cfg,CFG_DUMPARG) == TOGGLE_ON) dump_CONFIG(cfg);

    /* Check to make sure all the necessary arguements are set */
    {   int failure=FALSE;

	if (! is_CONFIG_group_set(cfg,2,TRUE))
	    failure=TRUE;
	if (! is_CONFIG_set(cfg,CFG_ALI_FILE,TRUE))
	    failure=TRUE;
	
	if (failure){
	    print_usage_CONFIG(cfg, PROG_NAME);
	    exit(-1);
	}
    }

    if (CONFIG_elem_TGL(cfg,CFG_ENTROPY_TEST) == TOGGLE_ON){
        test_entropy(CONFIG_elem_C(cfg,CFG_DISK_FILE),
                     atoi(CONFIG_elem_C(cfg,CFG_DEBUG)));
        exit(0);
    }

    if ((CONFIG_elem_C2(cfg,CFG_ALI_FILE) == NULL) ||
        (CONFIG_elem_C(cfg,CFG_CODESET) == NULL)){
            print_usage_CONFIG(cfg,PROG_NAME);
            exit(-1);
	}

    if (strcmp(CONFIG_elem_C(cfg,CFG_CODESET),"") != 0){
        get_code_table2(pcs, CONFIG_elem_C(cfg,CFG_CODESET),
			TRUE /*USE Add File*/,perr);
        /*dump_pcode2(pcs->cd[1].pc);*/
        if (*perr != 0){
            fprintf(stderr,"Error: Unable to load codeset %s, error code %d\n",
                       CONFIG_elem_C(cfg,CFG_CODESET),*perr);
            exit(-1);
        }
    } else {
        get_code_table_from_old_lex(pcs, CONFIG_elem_C(cfg,CFG_LEX), perr);
        if (*perr != 0){
            fprintf(stderr,"Error: Unable to load Lexicon %s, error code %d\n",
                       CONFIG_elem_C(cfg,CFG_LEX),*perr);
            exit(-1);
        }
    }

    if (load_in_SYS_ALIGN_LIST(&sa_list,CONFIG_elem_C2(cfg,CFG_ALI_FILE),
                 CONFIG_elem_present_C2(cfg,CFG_ALI_FILE),
                 CONFIG_elem_count(cfg,CFG_ALI_FILE),pcs) < 0){
        print_usage_CONFIG(cfg,PROG_NAME);
        exit(-1);
    }

    compute_entropy(&sa_list,pcs,CONFIG_elem_C(cfg,CFG_DISK_FILE),
		    atoi(CONFIG_elem_C(cfg,CFG_DEBUG)));
    return(0);
}

void compute_entropy(SYS_ALIGN_LIST *sa_list, TABLE_OF_CODESETS *pcs, char *diskfile, int dbg)
{
    PCIND_T *lex_ind;
    PCIND_T *lex_ind_link;
    int i, j, wnum, id, lex_id, sys;
    SYS_ALIGN *align_str;
    int count=0, pair_count=0;
    float entropy;

    /* allocate the memory to make an index array */
    alloc_singZ(lex_ind,lex_num(pcs),PCIND_T,0);
    alloc_singZ(lex_ind_link,lex_num(pcs),PCIND_T,0);

    if (dbg>0) printf("Compute_entropy: Search through the set of sentences\n");

    for (sys=0; sys<num_sys(sa_list); sys++){
        align_str=sys_i(sa_list,sys);
 
        /* search through all the sentences marking the used lexical items */
        for (i=0;i<num_spkr(align_str);i++){ /* for all speakers */
            for (j=0;j<num_sents(align_str,i);j++){ /* for every speaker sent */
                for (wnum=0; (wnum < MAX_NUM_WORDS) &&
                             ((lex_id=hyp_word(align_str,i,j,wnum)) != END_OF_WORDS);
                     wnum++)
                    if (id != DELETION)
                        lex_ind[lex_id]++;
                for (wnum=0; (wnum < MAX_NUM_WORDS) &&
                             ((lex_id=ref_word(align_str,i,j,wnum)) != END_OF_WORDS);
                     wnum++)
                    if (id != INSERTION)
                        lex_ind[lex_id]++;
            }
        }
    }
    

    for (i=0; i<lex_num(pcs); i++){          
        if (lex_ind[i] > 0){
            count++;
            lex_ind_link[i] = count; 
        }
    }
    if (dbg>0) printf("Compute_entropy: number of lex items %d\n",count);

    /* begin to write out the confusion pair matrix */
    init_disk_array(diskfile,count+1,count+1,dbg);

    for (sys=0; sys<num_sys(sa_list); sys++){
        int h_id, r_id, h_link, r_link;
        short value;
        align_str=sys_i(sa_list,sys);
 
        if (dbg>0) printf("Compute_entropy: Computing confusion matrix for system %s\n",
                           sys_name(align_str));
        fflush(stdout);
        /* search through all the sentences marking the used lexical items */
        for (i=0;i<num_spkr(align_str);i++){ /* for all speakers */
            for (j=0;j<num_sents(align_str,i);j++){ /* for every speaker sent */
                for (wnum=0; (wnum < MAX_NUM_WORDS) &&
                          (eval_wrd(align_str,i,j,wnum) != END_OF_WORDS);
                     wnum++){
                    h_id = hyp_word(align_str,i,j,wnum);
                    if (h_id == DELETION)
                        h_link = 0;
                    else
                         h_link = lex_ind_link[h_id];
                    r_id = ref_word(align_str,i,j,wnum);
                    if (r_id == INSERTION)
                        r_link = 0;
                    else
                        r_link = lex_ind_link[r_id];
                    if (dbg > 5)
                        printf("h_id %d, r_id %d, h_link %d, r_link %d",h_id,r_id,h_link,r_link);
                    get_disk_element(h_link,r_link,&value);
                    if (dbg > 5) printf("   old %d",value);
                    set_disk_element(h_link,r_link,value+1);
                    get_disk_element(h_link,r_link,&value);
                    if (dbg > 5) printf("   new %d\n",value);
                    pair_count ++;
                 }
             }
        }
        if (dbg > 5) printf("pairs loaded: %d\n",pair_count);
        disk_compute_entropy(dbg,&entropy);
        printf("Entropy = %6.4f\n",entropy);
   }
   close_disk_array();
}

void test_entropy(char *diskfile, int dbg)
{
    int dim_x = 5, dim_y = 5, x, y, pair_count=0;
    short value;
    float entropy;

    if (dbg > 0) printf("Test entropy:\n");
    if (dbg > 0) printf("     Full matrix:\n");

    /* begin to write out the confusion pair matrix */
    init_disk_array(diskfile,dim_x,dim_y,dbg);    
    for (x=0; x<dim_x; x++)
        for (y=0; y<dim_y; y++){
            set_disk_element(x,y,1);
            pair_count ++;
         }

    if (dbg > 5) printf("pairs loaded: %d\n",pair_count);
    disk_compute_entropy(dbg,&entropy);
    printf("Entropy = %6.4f\n",entropy);
    close_disk_array();

    if (dbg > 0) printf("Test entropy:\n");
    if (dbg > 0) printf("     Diagonal matrix:\n");

    /* begin to write out the confusion pair matrix */
    init_disk_array(diskfile,dim_x,dim_y,dbg);    
    for (x=0; x<dim_x; x++){
        if (dbg > 5) printf("x,y   (%d,%d)   ",x,x);
        get_disk_element(x,x,&value);
        if (dbg > 5) printf("   old %d",value);
        set_disk_element(x,x,value+1);
        get_disk_element(x,x,&value);
        if (dbg > 5) printf("   new %d\n",value);
        pair_count ++;
     }

    if (dbg > 5) printf("pairs loaded: %d\n",pair_count);
    disk_compute_entropy(dbg,&entropy);
    printf("Entropy = %6.4f\n",entropy);
    close_disk_array();

    if (dbg > 0) printf("Test entropy:\n");
    if (dbg > 0) printf("     Test case1:\n");

    /* begin to write out the confusion pair matrix */
    init_disk_array(diskfile,dim_x,dim_y,dbg);    
    set_disk_element(0,0,1);
    set_disk_element(0,1,1);
    set_disk_element(0,2,1);
    set_disk_element(0,3,1);
    set_disk_element(0,4,2);
    pair_count +=6;


    if (dbg > 5) printf("pairs loaded: %d\n",pair_count);
    disk_compute_entropy(dbg,&entropy);
    printf("Entropy = %6.4f\n",entropy);
    close_disk_array();

    if (dbg > 0) printf("Test entropy:\n");
    if (dbg > 0) printf("     Test case2:\n");

    /* begin to write out the confusion pair matrix */
    init_disk_array(diskfile,dim_x,dim_y,dbg);    
    set_disk_element(0,3,4);
    set_disk_element(0,4,2);
    pair_count +=6;


    if (dbg > 5) printf("pairs loaded: %d\n",pair_count);
    disk_compute_entropy(dbg,&entropy);
    printf("Entropy = %6.4f\n",entropy);
    close_disk_array();
}
