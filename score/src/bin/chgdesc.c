/**********************************************************************/
/*                                                                    */
/*             FILENAME:  change_name.c                               */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This program will change the system name of a     */
/*                  SYS_ALIGN file to the name on the command line    */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvp.h>
#include <score/scorelib.h>


#define MAX_FILENAME_LEN	80

#define DEFAULT_LEXICON		"drivers/rm/lexicon.snr"
#define DEFAULT_HOMO		"drivers/rm/homophn.txt"

void procargs PROTO((int, char **, char **, char **));

int main(int argc, char **argv)
{
    SYS_ALIGN *align_str;
    char *align_filename, *new_desc;
   
    procargs(argc,argv,&align_filename,&new_desc);

    if (read_SYS_ALIGN(&align_str,align_filename) < 0){
        exit(-1);
    }

    printf("Changing sys desc %s to %s \n",sys_desc(align_str),new_desc);
    strncpy(sys_desc(align_str),new_desc,MAX_DESC_LEN);
    write_SYS_ALIGN(align_str,align_filename);

    return(0);
}

void procargs(int argc, char **argv, char **align_filename, char **new_desc)
{
    /* find arguments */
    if (argc == 3){
        *align_filename = argv[1];
        *new_desc = argv[2];
        return;
    }
    fprintf(stderr,"Usage: chgdesc align_filename desc\n\n");
    exit(-1);
}
