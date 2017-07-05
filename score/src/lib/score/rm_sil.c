/**********************************************************************/
/*                                                                    */
/*             FILENAME:  rm_sil.c                                    */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*             DESC: This procedure removes the silence tokens from   */
/*                   string.                                          */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvf.h>
#include <score/scorelib.h>

void remove_silence(char *buff, TABLE_OF_CODESETS *pcs)
{
    static char sil_str[2][10] = {"-PAU-", "SILENCE"}, *cptr;
    int num_sil=2, sil, chr, dbg=0, ignore;
    char *cbuff;
    
    if (dbg) printf("  remove silence %s\n",buff);
    for (sil=0; sil<num_sil; sil++){
	cbuff = buff;
	adjust_type_case(sil_str[sil],pcs->cd[1].pc);
        while ((cptr=strstr(cbuff,sil_str[sil])) != NULL){
	    ignore = FALSE;
	    if (dbg) printf("   hit '%s'\n",sil_str[sil]);
	    if (dbg) printf("   beg '%s'\n",cptr);
	    if (cptr == buff){
		if (! isspace(*(cptr+strlen(sil_str[sil]))))
		    ignore=TRUE;
	    } else if (cptr+strlen(sil_str[sil]) == buff+strlen(buff)){
		if (! isspace(*(cptr-1)) && cptr!=buff)
		    ignore=TRUE;
	    } else { 
		if (! isspace(*(cptr-1)) ||
		    ! isspace(*(cptr+strlen(sil_str[sil]))))
		    ignore=TRUE;
	    }
	    if (!ignore)
		for (chr=0; chr < strlen(sil_str[sil]); chr++)
		    *cptr++ = ' ';
	    else {
		cptr += strlen(sil_str[sil]);
		if (dbg) printf("   IGNORED\n");
	    }
	    cbuff = cptr;
	}
    }
    if (dbg) printf("     final string '%s'\n",buff);
}
