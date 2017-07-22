/**********************************************************************/
/*                                                                    */
/*             FILENAME:  wwscore.c                                   */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This program will compute a weighted word scoring */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvp.h>
#include <score/scorelib.h>

#define MAX_FILENAME_LEN	80

#define DEFAULT_LEXICON		"drivers/rm/lexicon.snr"

#define PROG_NAME	"wwscor"
#define CFG_W_1         "W_1"
#define CFG_W_SYL       "W_SYL"
#define CFG_W_PHN       "W_PHN"
#define CFG_OVR_SUM     "SUM"

CONFIG score_config = {
  /*   intern id,     count,   type,    	groups, value  */
  {  { CFG_LEX,		1,	CFG_C,		1,	NULL} ,
     { CFG_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_CODESET,	1,	CFG_C,		1,	NULL} ,
     { CFG_ALI_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_DUMPARG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_W_1,		1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_W_PHN,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_W_SYL,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_OVR_SCR,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_OVR_SUM,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_WWL_FILE,	20,	CFG_C2,		NO_GRP,	NULL} ,
     { "",		0,	CFG_TGL,	NO_GRP,	TOGGLE_OFF}}
};
CONFIG *cfg = &score_config;

int main(int argc, char **argv)
{
    SYS_ALIGN *align_str;
    int error, *perr = &error;
    TABLE_OF_CODESETS codesets, *pcs = &(codesets);
   
    print_version(argv[0]);
    alloc_CONFIG(cfg);
    set_CONFIG_elem_C(cfg,CFG_ALI_FILE,"ALIGNED_FILE");
    cfg_get_args(cfg,argc,argv,"");

    if (CONFIG_elem_TGL(cfg,CFG_DUMPARG) == TOGGLE_ON) dump_CONFIG(cfg);

    /* Check to make sure all the necessary arguements are set */
    {   int failure=FALSE;

	if (! is_CONFIG_group_set(cfg,1,TRUE))
	    failure=TRUE;
	if (! is_CONFIG_set(cfg,CFG_ALI_FILE,TRUE))
	    failure=TRUE;
	
	if (failure){
	    print_usage_CONFIG(cfg, PROG_NAME);
	    exit(-1);
	}
	if (CONFIG_elem_TGL(cfg,CFG_OVR_SCR) == TOGGLE_OFF &&
	    CONFIG_elem_TGL(cfg,CFG_OVR_SUM) == TOGGLE_OFF)
	    set_CONFIG_elem_TGL(cfg,CFG_OVR_SUM,TOGGLE_ON);

    }

    if (strcmp(CONFIG_elem_C(cfg,CFG_CODESET),"") != 0){
        get_code_table2(pcs, CONFIG_elem_C(cfg,CFG_CODESET),
			TRUE /*USE Add File*/,perr);
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

    if (load_in_SYS_ALIGN(&align_str,CONFIG_elem_C(cfg,CFG_ALI_FILE),pcs) < 0){
        print_usage_CONFIG(cfg,PROG_NAME);
        exit(-1);
    }
    
    perform_word_weighted_scoring(align_str, pcs, 
				  (char *)0,
				  CONFIG_elem_TGL(cfg,CFG_W_1) ==TOGGLE_ON,
				  CONFIG_elem_TGL(cfg,CFG_W_PHN) ==TOGGLE_ON,
				  CONFIG_elem_TGL(cfg,CFG_W_SYL) ==TOGGLE_ON,
				  CONFIG_elem_present_C2(cfg,CFG_WWL_FILE),
				  CONFIG_elem_C2(cfg,CFG_WWL_FILE),
				  CONFIG_elem_TGL(cfg,CFG_OVR_SUM) ==TOGGLE_ON,
				  CONFIG_elem_TGL(cfg,CFG_OVR_SCR)==TOGGLE_ON);
				  
    free_SYS_ALIGN(align_str);
    free_code_table(pcs);

    return(0);
}






