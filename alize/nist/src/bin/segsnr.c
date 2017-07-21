/* segsnr.c */
/* Takes as input two files: a speech file and a segmentation file */
/* Returns the mean noise power, the 95% speech power, and the     */
/* speech to noise ratio. This should be used as a double check to */
/* the usual stnr results. */

#include <snr/snrlib.h>

/* segsnr.c */	void print_help_and_exit  PROTO((void));

int main (int argc, char **argv)
{
    SP_FILE *speech_file;
    FILE *seg_file;
    int **seg_list,i,k,segments=0;
    double noise,speech,snr; 
    char *speech_file_name,*seg_file_name,c;
    
    if ((argc==1) || (argc > 3))
	print_help_and_exit();
    
    speech_file_name = argv[1];
    if (argc==3) seg_file_name = argv[2];
    
    for (k=1; k<argc; k++) 
	if (argv[k][0] == '-')
	    while ((c = *++argv[k])) {
		switch (c) {
		  case 'h':
		    print_help_and_exit();
		    break;
		  default:
		    printf("I don't know option -%c. I only know -h. Try -h for help.\n",c);
		    exit(-1);
		}
	    }
    
    if ((speech_file = sp_open(speech_file_name,"r")) == SPNULL) {
	printf("I can't find the speech file <%s>\n",speech_file_name);
	exit(-1);
    }

    if (argc==3) {
	if ((seg_file = fopen(seg_file_name,"r")) == NULL) {
	    printf("I can't find the segmentation file <%s>\n",seg_file_name);
	    exit(-1);
	}
    } else seg_file=stdin;
    
    /* allocate space for seg_list[MAX_SEGMENT][2] */
    seg_list = (int **) malloc (MAX_SEGMENT*sizeof(int *));
    for (i=0; i < MAX_SEGMENT; i++) 
	seg_list[i] = (int *) malloc (2*sizeof(int));
    
    /* read in segmentation */
    while (fscanf(seg_file,"%d %d",
		  &seg_list[segments][FROM],&seg_list[segments][TO])!=EOF) {
	segments++;
    }
    
    
    segsnr_single_channel(speech_file,0,&noise,&speech,&snr,seg_list,segments,NULL_FILTER,0);
    
    printf("<%s> SegSNR S = %4.2f dB  N= %4.2f dB  SNR = %4.2f dB\n",speech_file_name,speech,noise,snr);
    
    sp_close(speech_file);
    fclose(seg_file);
    return(0);
}

void print_help_and_exit (void)
{
  printf("Usage: segsnr [-h] <speech file> [segmentation file]\n");
  printf("\n");
  printf("  -h : prints this message.\n");
  printf("\n");
  printf("Takes a speech file and a segmentation file such as one\n");
  printf("produced by speech.c and measures mean noise power, peak\n");
  printf("speech power, and speech to noise ratios. Assuming the\n");
  printf("segmentation is correct, these values should be more reliable\n");
  printf("than those produced by stnr.c \n");
  printf("\n");
  printf("If no segmentation file is specified, the standard input is\n");
  printf("read instead, allowing one to pipe speech.c and segsnr.c\n");
  printf("together.\n");
  exit(-1);
}
