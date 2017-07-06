/* headcheck.c */
/* Checks the header info of a wave file and tries to correct */
/* as much info as possible. */


#include <snr/snrlib.h>

#define BUFSIZE 262144

/* headcheck.c */	double square_magnitude PROTO((short int *array, int size));
/* headcheck.c */	int zero_cross PROTO((short int *array, int size));
/* headcheck.c */	void print_help_and_exit  PROTO((void));
/* headcheck.c */	void minmax PROTO((short int *array, int size, int *min, int *max));

int main (int argc, char **argv)
{
    SP_FILE *in_file;
    SPFILE_CHARS *sp_char;
    short buffer[BUFSIZE];
    int k,read,sample_count;
    char command_line[100],filename[50];
    int zero_crossings;
    double power,normalized_power,normalized_zero_crossings;
    double square_magnitude(short int *array, int size);
    char c;
    int max,min,claimed_max,claimed_min,local_max,local_min;
    
    int correct=0,quiet=0; /* boolean run-time options. */
    
    sp_char = (SPFILE_CHARS *) malloc (sizeof(SPFILE_CHARS));
    
    if ((argc > 3) || (argc < 2))
	print_help_and_exit();
    
    for (k=1; k<argc; k++) {
	if (argv[k][0] == '-')
	    while ((c = *++argv[k])) /* leave this structure general in case we add more options. */
		switch (c) {
		  case 'c':
		    correct = 1;
		    break;
		  case 'q':
		    quiet = 1;
		    break;
		  case 'h':
		    print_help_and_exit();
		  default:
		    printf("I don't know the option -%c. I only know -cqh. Try -h for help.\n",c);
		    exit(-1);
		}
	else sprintf(filename,"%s",argv[k]);
    }
    
    if ((in_file = sp_open(filename,"r")) == SPNULL) {
	printf("I can't find %s.\n",filename);
	exit(-1);
    }
    
    load_sp_char(in_file,sp_char);
    if (strstr(sp_char->sample_coding,"pcm") == (char *)0 &&
	strstr(sp_char->sample_coding,"linear") == (char *)0){
	fprintf(stderr,"Error: Unable to 'headcheck' a file encoded as '%s'\n",
		sp_char->sample_coding);
	exit(-1);
    }
    
    sample_count=0; power=0.0;
    min = 32767; max = -32767;
    
    while ((read = sp_read_data(buffer,BUFSIZE,in_file)) > 0) {
	sample_count += read;
	zero_crossings += zero_cross(buffer,read);
	power += square_magnitude(buffer,read);
	minmax(buffer,read,&local_min,&local_max);
	min = MIN(min,local_min);
	max = MAX(max,local_max);
    }
    
    /**** check byte order ****/
    
    normalized_zero_crossings = (double) zero_crossings / (double) sample_count;
    normalized_power = power / (double) sample_count;
    
    if ((normalized_zero_crossings > 0.3)&&(normalized_power > 100000000)) {
	if (!quiet) printf("<%s>: incorrect byte order?\n",filename);
	if (correct) {
	    sprintf(command_line,"h_edit -S sample_byte_format=%s %s",
		    sp_char->swap_bytes ? "10" : "01",
		    filename);
	    system(command_line);
	}
    } else
	if (!quiet) printf("Byte order looks OK.\n");
    
    if (sp_char->sample_count != sample_count) {
	if (!quiet) 
	    printf("<%s>: incorrect sample_count field (%ld instead of %d).\n",
		   filename,sp_char->sample_count,sample_count);
	if (correct) {
	    sprintf(command_line,"h_edit -I sample_count=%d %s",sample_count,filename);
	    system(command_line);
	}
    }
    
    /**** check min and max samples ****/
    
    if (sp_h_get_field(in_file,"sample_max",T_INTEGER,(void *)&claimed_max)>100){
	/* the field doesn't exist, so just add it */
	if (!quiet) printf("<%s>: missing sample_max field.\n",filename);
	if (correct) {
	    sprintf(command_line,"h_edit -I sample_max=%d %s",max,filename);
	    system(command_line);
	}
    } else {
	if (claimed_max != max) {
	    /* the header has the wrong value */
	    if (!quiet) printf("<%s>: incorrect sample_max field (%d instead of %d).\n",
			       filename,claimed_max,max);
	    if (correct) {
		sprintf(command_line,"h_edit -I sample_max=%d %s",max,filename);
		system(command_line);
	    }
	}
    }
    
    if (sp_h_get_field(in_file,"sample_min",T_INTEGER,(void *)&claimed_min)>100){
	/* the field doesn't exist, so just add it */
	if (!quiet) printf("<%s>: missing sample_min field.\n",filename);
	if (correct) {
	    sprintf(command_line,"h_edit -I sample_min=%d %s",min,filename);
	    system(command_line);
	}
    } else {
	/* field exists; look to see if it holds the right value. */
	if (claimed_min != min) {
	    /* the header has the wrong value */
	    if (!quiet) printf("<%s>: incorrect sample_min field (%d instead of %d).\n",
			       filename,claimed_min,min);
	    if (correct) {
		sprintf(command_line,"h_edit -I sample_min=%d %s",min,filename);
		system(command_line);
	    }
	}
    }
    
    sp_close(in_file);
    return(0);
}



double square_magnitude(short int *array, int size)
{
  int i;
  double total=0.0;

  for (i=0; i<size; i++)
    total += (double) array[i]*array[i];

  return total;
}



int zero_cross(short int *array, int size)
{
  int i,total=0;

  for (i=0; i<(size-1); i++) 
    total += (((array[i]==0)&&(array[i+1]!=0))
	      ||(array[i]*array[i+1]<0)) ? 1 : 0;
  return total;
}



void minmax(short int *array, int size, int *min, int *max)
{
  int i;

  *min=32767; 
  *max=-32767;

  for (i=0; i<size; i++) {
    *min = MIN(*min,array[i]);
    *max = MAX(*max,array[i]);
  }
}

void print_help_and_exit (void)
{
  printf("\n");
  printf("Usage: headcheck [-chq] <input file>\n\n");
  printf("  -c : correct any errors found in the header file, if possible.\n");
  printf("  -q : run in quiet mode, i.e., without text output.\n");
  printf("  -h : print this message.\n");
  printf("\n");
  printf("Checks the header file against the actual speech data and looks for\n");
  printf("discrepancies. If the -c option is selected, the discrepancies are\n");
  printf("corrected. Possible discrepancies include:\n");
  printf("\n");
  printf("            * incorrect sample count\n");
  printf("            * incorrect byte order (big vs. little endian)\n");
  printf("            * incorrect max and min samples\n");
  exit(-1);
}
