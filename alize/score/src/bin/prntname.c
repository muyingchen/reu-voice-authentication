/**********************************************************************/
/*                                                                    */
/*             FILENAME:  prntname.c                                  */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This program will print the system name and       */
/*                  description of the SYS_ALIGN file on the command  */
/*                  line                                              */
/*                                                                    */
/**********************************************************************/
#include <phone/stdcenvp.h>
#include <score/scorelib.h>

#define MAX_FILENAME_LEN	80

#define DEFAULT_LEXICON		"drivers/rm/lexicon.snr"
#define DEFAULT_HOMO		"drivers/rm/homophn.txt"

void procargs PROTO((int, char**, char**));

int main(int argc,char **argv)
{
    SYS_ALIGN *align_str;
    char *align_filename;
   
    procargs(argc,argv,&align_filename);

    if (read_SYS_ALIGN(&align_str,align_filename) < 0){
        exit(-1);
    }
    printf("%s -> %s\n",sys_name(align_str),sys_desc(align_str));
    return(0);
}

void procargs(int argc, char **argv, char **align_filename)
{
    /* find arguments */
    if (argc == 2){
        *align_filename = argv[1];
        return;
    }
    fprintf(stderr,"Usage: print_name align_filename\n\n");
    exit(-1);
}
