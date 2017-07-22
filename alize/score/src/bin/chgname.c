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

void procargs PROTO((int argc, char **argv, char **align_filename, char **new_name));

int main(int argc, char **argv)
{
    SYS_ALIGN *align_str;
    char *align_filename, *new_name;
   
    procargs(argc,argv,&align_filename,&new_name);

    if (read_SYS_ALIGN(&align_str,align_filename) < 0){
        exit(-1);
    }
 
    printf("Changing sys name %s to %s \n",sys_name(align_str),new_name);
    strncpy(sys_name(align_str),new_name,MAX_NAME_LEN);
    write_SYS_ALIGN(align_str,align_filename);
    return(0);
}

void procargs(int argc, char **argv, char **align_filename, char **new_name)
{

    /* find arguments */
    if (argc == 3){
        *align_filename = argv[1];
        *new_name = argv[2];
        return;
    }
    fprintf(stderr,"Usage: chgname align_filename name\n\n");
    exit(-1);
}
