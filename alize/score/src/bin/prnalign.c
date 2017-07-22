/**********************************************************************/
/*                                                                    */
/*             FILENAME:  prnalign.c                                  */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This program will print to stdout the contents    */
/*                  of upto 10 align_files with each corresponding    */
/*                  sentence for each system printed together         */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvp.h>
#include <score/scorelib.h>


#define MAX_FILENAME_LEN	80

#define DEFAULT_LEXICON		"drivers/rm/lexicon.snr"
#define DEFAULT_HOMO		"drivers/rm/homophn.txt"
#define PROG_NAME	"prnalign"
#define CFG_WORD_INDEX  "ind"

CONFIG score_config = {
  /*   intern id,     count,   type,    	groups, value  */
  {  { CFG_LEX,		1,	CFG_C,		2,	NULL} ,
     { CFG_CODESET,	1,	CFG_C,		2,	NULL} ,
     { CFG_LSUBS,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_ALI_FILE,	10,	CFG_C2,		NO_GRP,	NULL} ,
     { CFG_DUMPARG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_ALL_SENT,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_DIFF_SENT,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_DIFF_SCORE,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_ERRORS,	1,	CFG_TGL,	1,	TOGGLE_OFF} ,
     { CFG_WORD_INDEX,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SHLSUBS,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { "",		0,	CFG_TGL,	NO_GRP,	TOGGLE_OFF}}
};
CONFIG *cfg = &score_config;

TABLE_OF_CODESETS codesets, *pcs = &(codesets);

int main(int argc, char **argv)
{
    SYS_ALIGN_LIST sa_list;
    LEX_SUBSET *lsub = (LEX_SUBSET *)0;
    int error, *perr = &error;

    print_version(argv[0]);
    alloc_CONFIG(cfg);
    set_CONFIG_elem_TGL(cfg,CFG_ALL_SENT,TOGGLE_ON);
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

    if (strcmp(CONFIG_elem_C(cfg,CFG_CODESET),"") != 0){
        get_code_table2(pcs, CONFIG_elem_C(cfg,CFG_CODESET), 
			TRUE /*USE Add File*/,perr);
        if (*perr != 0){
            fprintf(stderr,"Error: Unable to load codeset %s, error code %d\n",
                       CONFIG_elem_C(cfg,CFG_CODESET),*perr);
            exit(-1);
        } else {
            printf("codeset loaded in ok\n");
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
      
    load_LEX_SUBSET(&lsub,pcs,CONFIG_elem_C(cfg,CFG_LSUBS));
    if (CONFIG_elem_TGL(cfg,CFG_SHLSUBS) == TOGGLE_ON) dump_LEX_SUBSET(lsub,pcs);

    dump_SA_LIST(&sa_list,pcs,lsub,CONFIG_elem_TGL(cfg,CFG_ALL_SENT) == TOGGLE_ON,
                              CONFIG_elem_TGL(cfg,CFG_DIFF_SENT) == TOGGLE_ON,
                              CONFIG_elem_TGL(cfg,CFG_DIFF_SCORE) == TOGGLE_ON,
                              CONFIG_elem_TGL(cfg,CFG_ERRORS) == TOGGLE_ON,
                              CONFIG_elem_TGL(cfg,CFG_WORD_INDEX) == TOGGLE_ON);

    return(0);
}


