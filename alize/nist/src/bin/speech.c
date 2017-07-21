/* speech.c */

#include <snr/snrlib.h>

#define FROM 0
#define TO   1

/* speech.c */   void print_help_and_exit PROTO((void));

int main (int argc, char *argv[])
{
    SP_FILE *in_file,*out_file;
    double **snrs,**signal,**noise,**cross;
    int **output_list;
    int segments,i,k;
    int max_segment = 500;
    char *in_file_name,c;

    if ((argc==1)||(argc > 3))
	print_help_and_exit();

    in_file_name = argv[1];
    if (argc==3) out_file = sp_open(argv[2],"w");

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
    
    if ((in_file = sp_open(in_file_name,"r")) == NULL) {
	printf("I can't find <%s>\n",in_file_name);
	return -1;
    }
    
    if (snr_all_channels(in_file,&signal,&noise,
			 &snrs,&cross,DONT_FILTER,FALSE,0,"",0.95) < 0){
	fprintf(stderr,
		"Error: Computing signal to noise on all channels failed\n");
      return -1;
    }
    
    /* allocate space for output_list[max_segment][2] */
    alloc_2dimZ(output_list,max_segment,2,int,0);

    segments = segment_speech(in_file,signal[0][0],noise[0][0],&max_segment,
			      &output_list);
    
    for (i=0; i<segments; i++)
	printf("%d %d\n",output_list[i][FROM],output_list[i][TO]);

    if (argc==3) {
	concatenate_speech(in_file,output_list,segments,out_file);
	sp_close(out_file);
    }
    
    sp_close(in_file);
    return(0);
}


void print_help_and_exit (void)
{
  printf("Usage: speech [-h] <input file> [output file]\n\n");
  printf("  -h : prints this message.\n");
  printf("\n");
  printf("Prints to the standard output device a file of the form:\n");
  printf("  <from> <to>\n");
  printf("  <from> <to>\n");
  printf("     ...     \n");
  printf("where <from> and <to> are 10 msec frame numbers that give\n");
  printf("the ranges of <input file> that contain speech. If an\n");
  printf("[output file] is specified, these speech regions are\n");
  printf("concatenated into one file and saved in [output file].\n");
  exit(-1);
}

