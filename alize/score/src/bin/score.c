/**********************************************************************/
/*                                                                    */
/*             FILENAME:  score.c                                     */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains the main program for the       */
/*                  scoring program.  Basically the program takes     */
/*                  an alignment file and counts errors for report    */
/*                  generation.  There are three levels of reports:   */
/*                      1: sentence by sentence error reports         */
/*                      2: a summary for each speaker                 */
/*                      3: a summary for the entire system            */
/*                  If the OUTPUT_DIR is a valid directory, then all  */
/*                  reports are printed to files inside of that dir   */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvp.h>
#include <score/scorelib.h>

/**********************************************************************/
/*   general sizes and defines                                        */
#define MAX_FILENAME_LEN	80
#define PROG_NAME		"score"

void check_output_dir PROTO((char *out_dir));


CONFIG score_config = {
  /*   intern id,     count,   type,    	groups, value  */
  {  { CFG_CODESET,	1,	CFG_C,		1,	NULL} ,
     { CFG_LEX,		1,	CFG_C,		1,	NULL} ,
     { CFG_SPLTMRG,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_LSUBS,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_MONO,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_ALP_NUM,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_ALI_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_OUT_DIR,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_FILE,	1,	CFG_C,		NO_GRP,	NULL} ,
     { CFG_SHCODESET,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SHLSUBS,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SHALP_NUM,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SHMONO,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SHSPLT_MRG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_HOMO_ERROR,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_OVR_SCR,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_OVR_RAW,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_OVR_DTL,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SPKR_SCR,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_SENT_SCR,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { CFG_DUMPARG,	1,	CFG_TGL,	NO_GRP,	TOGGLE_OFF} ,
     { "",		0,	CFG_TGL,	NO_GRP,	TOGGLE_OFF}}
};
CONFIG *cfg = &score_config;

/**********************************************************************/
/*  main program for the scoring program.  the flow of the program is:*/
/*      initialize globals and checks the output_dir                  */
/*      allocate and load all text files and the ALIGNMENT_FILE       */
/*      score the ALIGNMENT_FILE                                      */
/**********************************************************************/

int main(int argc, char **argv)
{
    SYS_ALIGN *align_str;
    ALPHA_NUM *alpha_num;
    TABLE_OF_CODESETS codesets, *pcs = &(codesets);
    LEX_SUBSET *lsub;
    MONO_SYL *mono_syl;
    SPLT_MRG *splt_mrg;
    int error, *perr = &error;

    print_version(argv[0]);
    alloc_CONFIG(cfg);
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

    check_output_dir(CONFIG_elem_C(cfg,CFG_OUT_DIR));

    /*  load all files to score with */
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

    load_ALPHA_NUM(&alpha_num,pcs,CONFIG_elem_C(cfg,CFG_ALP_NUM));
    load_LEX_SUBSET(&lsub,pcs,CONFIG_elem_C(cfg,CFG_LSUBS));
    load_MONO_SYL(&mono_syl,pcs,CONFIG_elem_C(cfg,CFG_MONO));
    load_SPLT_MRG(&splt_mrg,pcs,CONFIG_elem_C(cfg,CFG_SPLTMRG));

    /* dump only of the structure used to score with */
    if (CONFIG_elem_TGL(cfg,CFG_SHALP_NUM) == TOGGLE_ON)
                                       dump_ALPHA_NUM(alpha_num,pcs);
    if (CONFIG_elem_TGL(cfg,CFG_SHLSUBS) == TOGGLE_ON)
                                       dump_LEX_SUBSET(lsub,pcs);
    if (CONFIG_elem_TGL(cfg,CFG_SHMONO) == TOGGLE_ON)
                                       dump_MONO_SYL(mono_syl,pcs);
    if (CONFIG_elem_TGL(cfg,CFG_SHSPLT_MRG) == TOGGLE_ON)
                                       dump_SPLT_MRG(splt_mrg,pcs);
    if (load_in_SYS_ALIGN(&align_str,CONFIG_elem_C(cfg,CFG_ALI_FILE),pcs) < 0){
        print_usage_CONFIG(cfg,PROG_NAME);
        exit(-1);
    }

    if (CONFIG_elem_TGL(cfg,CFG_HOMO_ERROR) == TOGGLE_ON)
	rm_HOMO_CORR_from_SYS_ALIGN(align_str);

    print_score_SYS_ALIGN(align_str,pcs,lsub,
                    alpha_num,mono_syl,splt_mrg,
                    CONFIG_elem_TGL(cfg,CFG_OVR_DTL) == TOGGLE_ON,
                    CONFIG_elem_TGL(cfg,CFG_OVR_SCR) == TOGGLE_ON,
                    CONFIG_elem_TGL(cfg,CFG_OVR_RAW) == TOGGLE_ON,
                    CONFIG_elem_TGL(cfg,CFG_SPKR_SCR) == TOGGLE_ON,
                    CONFIG_elem_TGL(cfg,CFG_SENT_SCR) == TOGGLE_ON,
                    CONFIG_elem_C(cfg,CFG_OUT_DIR),
                    CONFIG_elem_C(cfg,CFG_ALI_FILE));

    free_SYS_ALIGN(align_str);
    free_SPLT_MRG(splt_mrg,MAX_SPLT_MRG);
    free_ALPHA_NUM(alpha_num);
    free_MONO_SYL(mono_syl);
    free_LEX_SUBSET(lsub);
    free_code_table(pcs);
    return(0);
}

/**********************************************************************/
/*  Check to see if the output directory exists, if it does not exist */
/*  reset the character string to NULL                                */
/**********************************************************************/
void check_output_dir(char *out_dir)
{
    if (*out_dir == NULL_CHAR)
        return;
    if (!dir_exists(out_dir)){
        fprintf(stderr,"Could not find output dir -%s-.  Erasing filename\n",	                      out_dir);
        *(out_dir) = NULL_CHAR;
    }
}

