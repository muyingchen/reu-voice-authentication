/* autohum.c */
/* detects the presence of weird AC components via autocorrelation */

#include <snr/snrlib.h>

#ifndef PI
#define PI (double) 3.14159265358979
#endif

#define MILISECONDS 10.0 /* size of window */
#define AUTO_WINDOW 10 /* number of 10ms windows in the auto-correlation window */
#define FROM 0
#define TO 1

/* #define OUTPUT_PLOT 1 */

#define PEAK_THRESHOLD 0.4

/* autohum.c */	int first_zero_crossing  PROTO((double *array, int size));
/* autohum.c */	int find_peak  PROTO((int offset, double *array, int size, double *value));
/* autohum.c */	double std_dev_distance PROTO((double *array, int index, int size));
/* autohum.c */	void unbias_and_centerclip PROTO((short int *x, complex *y, int size, double clip_percentile));
/* autohum.c */	void auto_correlation PROTO((short int *s, double *r, int size, double clip));
/* autohum.c */	void log_harmonic_product PROTO((double *r, int size, int depth));
/* autohum.c */	void smooth  PROTO((double *x, int window, int size));
/* autohum.c */	void hann_window PROTO((double *data, int size));
/* autohum.c */	void print_help_and_exit  PROTO((void));

/**********************************************************************/
/*  Bug Fix:                                                          */
/*     11/10/92  Reference to outfile not #ifdef-#endif properly for  */
/*               conditional compile                                  */
int main (int argc, char **argv)
{
    FILE *seg_file;
    SP_FILE *in_file;
    SPFILE_CHARS *sp_char;
    short *buffer, *junk;
    char c,filename[80],seg_name[80];
    int i,j,k;
    double *auto_corr,*mean_auto_corr;
    int frame,current_segment,start_silence,end_silence,count,win,autowin,segments;
    double speech,noise,snr;
    int **list;         /* list of speech segments */
    int max_segment = 500;
    int ac_num=0,read;
    
    double clip,norm_value,peak_freq;
    int smooth_size,peak_index,origin_peak,first_zc,silence_at_the_end;
    
    int verbose=0,seg=0,seg_name_provided=0;
    int next_arg_please;
    
    if (argc==1 || argc > 5) 
	print_help_and_exit();
    
    clip=0.0;   /* for now; these seem to work pretty well. */
    smooth_size=5;
    
    next_arg_please=0;
    for (k=1; k<argc; k++) 
	if (argv[k][0] == '-')
	    while ((c = *++argv[k])) {
		switch (c) {
		  case 'v':
		    verbose = 1;
		    break;
		  case 'c':
		    k++;
		    clip = atof(argv[k]);
		    next_arg_please=1;
		    break;
		  case 's':
		    k++;
		    seg=1;
		    if (k < argc) {
			sprintf(seg_name,"%s",argv[k]);
			seg_name_provided=1;
		    }
		    next_arg_please=1;
		    break;
		  case 'h':
		    print_help_and_exit();
		  default:
		    printf("I don't know option -%c. I only know -chsv. Try -h for help.\n",c);
		    exit(-1);
		}
		if (next_arg_please) {
		    next_arg_please=0;
		    break;
		}
	    }
	else sprintf(filename,"%s",argv[k]);
    
    printf("\n");
    
    /* allocate memory */
    sp_char = (SPFILE_CHARS *) malloc (sizeof(SPFILE_CHARS));

    alloc_2dimZ(list,max_segment,2,int,0);    
    
    if ((in_file = sp_open(filename,"r")) == SPNULL) {
	printf("I can't find the speech file <%s>.\n",filename);
	return -1;
    }

    load_sp_char(in_file,sp_char);

    win = window_size(sp_char->sample_rate,MILISECONDS);
    autowin = AUTO_WINDOW*win;

    junk = (short *) malloc (win*sizeof(double));
    auto_corr = (double *) malloc (autowin*sizeof(double));
    mean_auto_corr = (double *) malloc (autowin*sizeof(double));
    buffer = (short *) malloc (autowin*sizeof(short));


    if (seg) {  /* if we're supposed to read in a segmentation file */
	if (seg_name_provided) {
	    if ((seg_file = fopen(seg_name,"r")) == NULL) {
		printf("I can't find the segmentation file <%s>.\n",seg_name);
		return -1;
	    }
	}
	else
	    seg_file = stdin;
	
	/* read in segmentation */
	segments=0;
	while (fscanf(seg_file,"%d %d",
		      &list[segments][FROM],&list[segments][TO])!=EOF) 
	    segments++;
    } else /* we have to do our own segmentation */ {
	snr_single_channel(in_file,0,&noise,&speech,&snr,(double *)0,0,
			   NULL_FILTER,0,0,0,"",0.95);
	segments = segment_speech(in_file,speech,noise,&max_segment, &list);
    }

    if (segments==0) {
	printf("I can't discern the speech from the noise in this file.\n");
	exit(-1);
    }
    
    if (verbose) {
	printf("result of segmentation:\n");
	printf("----------------------\n");
	for (i=0; i<segments; i++)
	    printf("%d -- %d\n",list[i][FROM],list[i][TO]);
	printf("\nNumber correspond to ranges of frame numbers that have been \n");
	printf("identified as containing speech. Frames are 10ms long.\n");
    }
    
    sp_rewind(in_file);

    for (i=0; i<autowin; i++) mean_auto_corr[i]=0.0;

#ifdef OUTPUT_PLOT
    out_file = fopen(strcat(getenv("HOME"),"/auto"),"w");
#endif

    frame = 0;
    ac_num = 0;

    for (current_segment=0; current_segment<segments; current_segment++) {
	start_silence=(current_segment==0) ? 0 : list[current_segment-1][TO]+1;
	end_silence = list[current_segment][FROM]-1;
	for (i=frame; i <= start_silence; i++) {
	    sp_read_data((char *)junk,win,in_file);  /* skip non-silence */
	}
	for (i=0; i< DIV((end_silence-start_silence+1),AUTO_WINDOW); i++) {
	    /* read the stuff, do auto */
	    for (j=0; j < AUTO_WINDOW; j++) {
		sp_read_data((char *)(buffer+j*win),win,in_file);
	    }
	    auto_correlation(buffer,auto_corr,autowin,clip);
	    for (j=0; j<autowin; j++) mean_auto_corr[j] += auto_corr[j];
	    ac_num++;
	}
	for (i=0; i<MOD((end_silence-start_silence+1),AUTO_WINDOW); i++) {
	    sp_read_data((char *)junk,win,in_file);  /* skip trailing part */
	}
	frame=end_silence+1;
    }

    /* handle case of last segment */
  
    /* first skip over last speech part */
    for (i=frame; i <= list[segments-1][TO]; i++) 
	sp_read_data((char*)junk,win,in_file);
  
    count=0;
    silence_at_the_end=0;
    do {
	read = sp_read_data((char *)(buffer+count*win),win,in_file);
	count++;
	silence_at_the_end++;
	if ((count==AUTO_WINDOW)&&(read==win)) {
	    count = 0;
	    auto_correlation(buffer,auto_corr,autowin,clip);
	    for (i=0; i<autowin; i++) mean_auto_corr[i] += auto_corr[i];
	    ac_num++;
	}
    } while (read==win);
    
    if (!ac_num) {
	printf("\nI could not find a period of silence long enough to analyze.\n");
	exit(1);
    }
    
    if (verbose) {
	printf("\nIdentified %d frames of silence at the end.\n",
	       silence_at_the_end);
	printf("\nComputed %d autocorrelations.\n",ac_num);
    }

    if (smooth_size) smooth(mean_auto_corr,smooth_size,autowin); 

    origin_peak = find_peak (0,mean_auto_corr,autowin,&norm_value);

    first_zc = first_zero_crossing(mean_auto_corr,autowin);

    peak_index = find_peak (first_zc,mean_auto_corr,autowin,&norm_value);
    peak_freq = 16000.0/((double) peak_index);
    
    printf("Peak detected at %.1f Hz.\n",peak_freq);
    if ((mean_auto_corr[peak_index]/mean_auto_corr[origin_peak] > 
	 PEAK_THRESHOLD) &&
	((fabs(peak_freq-30.0) < 10.0) ||
	 (fabs(peak_freq-60.0) < 10.0) ||
	 (fabs(peak_freq-90.0) < 10.0) ||
	 (fabs(peak_freq-120.0) < 10.0) ||
	 (fabs(peak_freq-180.0) < 10.0) ||
	 (fabs(peak_freq-240.0) < 10.0) ||
	 (fabs(peak_freq-300.0) < 10.0)
	 ))
	printf("This file looks like it's contaminated by hum. I suggest you check it out.\n");
    else
	printf("I don't think this file suffers from ground loops.\n");
    
#ifdef OUTPUT_PLOT
    for (i=0; i<autowin; i++) 
	fprintf(out_file,"%d %12.12f\n",i,mean_auto_corr[i]/mean_auto_corr[origin_peak]);
    fclose(out_file);
#endif
    return(0);
}

int first_zero_crossing (double *array, int size)
{
  int i;

  for (i=0; i<size; i++) {
    if (array[i]==0) return i;
    if (i) 
      if (array[i]*array[i-1] < 0) return i;  /* look for sign change */
  }
  printf("No ZC found.\n");
  return i;
}

int find_peak (int offset, double *array, int size, double *value)
{
  double max = -999999999;
  int i,index;

  for (i=offset; i<size; i++) {
    if (array[i]>max) {
      max=array[i];
      index=i;
    }
  }

  *value = (array[0] == 0) ? 0 : array[index]/array[0];

  return index;
}

double std_dev_distance(double *array, int index, int size)
{
  int i;
  double sum=0.0, mean;

  for (i=0; i< size; i++) sum += array[i];

  mean = sum/(double)size;

  sum=0.0;
  for (i=0; i<size; i++) sum += (array[i]-mean)*(array[i]-mean);

  return fabs(array[index]-mean)/sqrt(sum);
}
    
void unbias_and_centerclip(short int *x, complex *y, int size, double clip_percentile)
{
  int i;
  double sum=0.0,max=0.0;

  for (i=0; i<size; i++) sum += (double) x[i];
  for (i=0; i<size; i++) {
    y[i].re = (double) x[i] - sum/((double)size);
    if (fabs(y[i].re)>max) max=fabs(y[i].re);
  }
  for (i=0; i<size; i++) 
    if (fabs(y[i].re) < clip_percentile*max/100.0) y[i].re= 0.0;
}
  

void auto_correlation(short int *s, double *r, int size, double clip)
{
  int i;
  complex *x;
  int log_fft_size,fft_size;

  log_fft_size=ceil(log10((double)size)/log10(2.0))+1;
  fft_size = (int) pow(2.0,(double)log_fft_size);

  x = (complex *) malloc (fft_size*sizeof(complex));

  unbias_and_centerclip(s,x,size,clip);

  for (i=0; i<fft_size; i++) {
    if (i >= size) x[i].re=0;
    x[i].im=0.0;
  }

  fft_real(x,fft_size);

  for (i=0; i< fft_size; i++) {
    x[i].re=x[i].re*x[i].re + x[i].im*x[i].im;
    x[i].im=0.0;
  }

  ifft_real(x,fft_size);

  for (i=0; i<size; i++) r[i] = (x[0].re==0.0) ? 0.0 : x[i].re/x[0].re;
  
  free(x);
}

void log_harmonic_product(double *r, int size, int depth)
{
  int i,j;
  
  for (i=0; i< size/depth; i++) {
    for (j=2; j <= depth; j++)
      r[i] *= r[i*j];
    r[i]=(r[i] <= 0) ? -5.0 : log10(r[i]);

  for (i=size/depth; i<size; i++)
    r[i]=-5.0;
  }
}
    

void smooth (double *x, int window, int size)
{
  int i,j;
  double val;
  
  for (i=0; i<size; i++) {
    val=0.0;
    for (j=MAX(0,i-window); j<MIN(size,i+window); j++)
      val += x[j];
    x[i]=val/((double) 2*window+1);
  }
}


void hann_window(double *data, int size)
{
  int i;

  for (i=0; i<size; i++)
    data[i] *= 0.5*(1 - cos(2*PI*i/size));
}


void print_help_and_exit (void)
{
  printf("Usage: autohum [-chsv] <input file> [-c center_clip]\n");
  printf("                                    [-s [segmentation_file]]\n\n");
  printf("  -c : percentage of center clipping that should be\n");
  printf("       applied before autocorrelating. Should be followed\n");
  printf("       by a number between 0 (no clipping) to 1 (full\n");
  printf("       clipping. Default is 0.\n");
  printf("  -v : verbose option. Prints how the file has been\n");
  printf("       segmented into speech and noise regions.\n");
  printf("  -s : Use pre-determined segmentation data to separate\n");
  printf("       speech from silence. If no [segmentation_file] is\n");
  printf("       provided, then the segmentation data are read from\n");
  printf("       the standard input, allowing speech.c and autohum.c\n");
  printf("       to be piped together.\n");
  printf("  -h : print this message.\n");
  printf("Alerts to the presence of ground loops in the recording process.\n\n");
  exit(-1);
}
