/**********************************************************************/
/*                                                                    */
/*             FILENAME:  pralign.c                                   */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This program will print to stdout the contents    */
/*                  of an align_file                                  */
/*           CHANGES:  05-14-90 Added the ability to use an alternate */
/*                     lexicon and sentence type file.                */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvp.h>
#include <score/scorelib.h>

#define MAX_FILENAME_LEN	80

#define DEFAULT_LEXICON		"drivers/rm/lexicon.snr"

#define PROG_NAME	"pralign"
#define CFG_DUMP_SCORE     "scores"
#define CFG_DUMP_TIMES     "times"

CONFIG score_config = {
  /*   intern id,     count,   type,    	groups, value  */
  {  { CFG_LEX,		1,	CFG_C,		1,	NULL} ,
     { CFG_CODESET,	1,	CFG_C,		1,	NULL} ,
     { CFG_LSUBS,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_ALI_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_DUMPARG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SHLSUBS,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_DUMP_SCORE,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_DUMP_TIMES,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { "",		0,	CFG_TGL,	NO_GRP,	TOGGLE_OFF}}
};
CONFIG *cfg = &score_config;


TABLE_OF_CODESETS codesets, *pcs = &(codesets);


int main(int argc, char **argv)
{
    SYS_ALIGN *align_str;
    int error, *perr = &error;
    LEX_SUBSET *lsub;
   
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
    load_LEX_SUBSET(&lsub,pcs,CONFIG_elem_C(cfg,CFG_LSUBS));
    if (CONFIG_elem_TGL(cfg,CFG_SHLSUBS) == TOGGLE_ON)
	dump_LEX_SUBSET(lsub,pcs);

    if (load_in_SYS_ALIGN(&align_str,CONFIG_elem_C(cfg,CFG_ALI_FILE),pcs) < 0){
        print_usage_CONFIG(cfg,PROG_NAME);
        exit(-1);
    }

    printf("dump of file %s\n\n",CONFIG_elem_C(cfg,CFG_ALI_FILE));
    dump_SYS_ALIGN(align_str,pcs,lsub,
		   CONFIG_elem_TGL(cfg,CFG_DUMP_SCORE) == TOGGLE_ON,
		   CONFIG_elem_TGL(cfg,CFG_DUMP_TIMES) == TOGGLE_ON);
    return(0);
}

