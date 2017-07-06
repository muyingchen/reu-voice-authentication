/* clp.c */

/* Checks for clipping */

#include <snr/snrlib.h>

#define BLOCKSIZE 2048

/* clp.c */	void print_help_and_exit  PROTO((void));

int main (int argc, char **argv)
{
    SP_FILE *in_file;
    short buffer[BLOCKSIZE];
    SPFILE_CHARS *sp_char;
    int read,clip_high,clip_low,high_ceiling,low_ceiling,i,k;
    short min,max;
    char *in_file_name,c;
    
    if (argc !=2)
	print_help_and_exit();
    
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
	else
	    in_file_name = argv[k];
    
    if ((in_file = sp_open(in_file_name,"r")) == SPNULL) {
	printf("I can't find %s.\n\n",in_file_name);
	exit(-1);
    }
    sp_char = (SPFILE_CHARS *) malloc (sizeof(SPFILE_CHARS));
    load_sp_char(in_file,sp_char);
    
    min=32767; max=-32767;
    
    while ((read=sp_read_data((char *)buffer,BLOCKSIZE,in_file)) != 0)
	for (i=0; i<read; i++) {
	    if (buffer[i] > max) max=buffer[i];
	    if (buffer[i] < min) min=buffer[i];
	}
    
    printf("Minimum sample=%d; maximum sample=%d\n",min,max);
    
    sp_rewind(in_file);
    
    clip_high=clip_low=0;
    high_ceiling=low_ceiling=0;
    while ((read=sp_read_data((char *)buffer,BLOCKSIZE,in_file)) != 0)
	for (i=0; i<read; i++) {
	    if ((buffer[i]!=max)&&(buffer[i]!=min))
		high_ceiling=low_ceiling=0;
	    else if (buffer[i]==max) {
		high_ceiling++;
		low_ceiling=0;
	    } else {
		low_ceiling++;
		high_ceiling=0;
	    }
	    if (high_ceiling==2) clip_high++;
	    if (low_ceiling==2) clip_low++;
	}
    
    if (clip_high) 
	printf("This file appears to clip %d times at a ceiling value of %d.\n",
	       clip_high,max);
    if (clip_low)
	printf("This file appears to clip %d times at a floor value of %d.\n",
	       clip_low,min);
    if (!(clip_high || clip_low))
	printf("This file does not appear to clip anywhere.\n");
    
    sp_close(in_file);
    return(0);
}
    
      
void print_help_and_exit (void)
{
  printf("Usage: clp [-h] <filename>\n\n");
  printf("  -h : prints this message.\n");
  printf("\n");
  printf("Checks for clipping in <filename>.\n");
  exit(-1);
}
