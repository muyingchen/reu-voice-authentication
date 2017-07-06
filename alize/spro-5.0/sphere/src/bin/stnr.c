
#include <snr/snrlib.h>

#define CROSS_THRESH 12

/* stnr.c */	Filterbank *get_fbank PROTO((char *fname));
/* stnr.c */	void print_help_and_exit  PROTO((void));
/* stnr.c */	void report_snr PROTO((char *filename, int quick, int subband, int codec, int graph));
/* strn.c */	Filterbank *get_filters PROTO((char *fname));


int main(int argc, char **argv)
{
  FILE *list_fp;
  char filename[200],subname[200],c;
  int k;

  int subband=0,quick=0,list=0;   /* boolean options */
  int codec=0, graph=0;

  if (argc==1) 
    print_help_and_exit ();

  for (k=1; k<argc; k++) {
    if (argv[k][0] == '-')
      while ((c = *++argv[k])) /* leave this structure general in case we add more options. */
	switch (c) {
	case 'q':
	  if (codec) {
	    printf("I can't make a quick SNR estimate and a CODEC estimate\n");
	    printf("at the same time. Try either one option or the other.\n\n");
	    exit(-1);
	  }
	  quick = 1;
	  break;
	case 'f':
	  list = 1;
	  break;
	case 'g':
	  graph = 1;
	  break;
	case 'h':
	  print_help_and_exit();
	case 's':
	  subband = 1;
	  break;
	case 'c':
	  if (quick) {
	    printf("I can't make a quick SNR estimate and a CODEC estimate\n");
	    printf("at the same time. Try either one option or the other.\n\n");
	    exit(-1);
	  }
	  codec = 1;
	  break;
	default:
	  printf("I don't know option -%c. I only know -cfhqs. Try -h for help.\n",c);
	  exit(-1);
	}
    else sprintf(filename,"%s",argv[k]);
  }

  if (list) {
    if ((list_fp = fopen(filename,"r")) == NULL) {
	printf("I can't find %s.\n",filename);
	return -1;
    }
    while (fscanf(list_fp,"%s",subname) != EOF)
	report_snr(subname,quick,subband,codec,graph);
    fclose(list_fp);
  } else report_snr(filename,quick,subband,codec,graph);
  return(0);
}

  
void report_snr(char *filename, int quick, int subband, int codec, int graph)
{
  int channels,bands,i,j;
  double **snrs,**signal,**noise,**cross;
  char entry[20];
  Filterbank *fbank;
  Filterbank *get_fbank(char *fname); 
  char filtername[50];
  SP_FILE *sp;

  sprintf(filtername,"%s/filterbank",getenv("HOME"));

  fbank = (subband) ? get_fbank(filtername) : DONT_FILTER;

  if (fbank==DONT_FILTER) subband=0;  /* turn off the subband flag */

  if ((sp = sp_open(filename,"rv")) == SPNULL){
      printf("I can't find <%s>, so I'll skip it.\n",filename);
      return;
  }
  if (sp_set_data_mode(sp,"SE-PCM:SBF-N") != 0){
	fprintf(spfp,"Error: sp_set_data_mode to 'SE-PCM:SBF-N'"
		" failed on file '%s'\n",filename);
	sp_close(sp);
  }

  channels = quick ? 
      quick_snr_all_channels(sp,&signal,&noise,&snrs,fbank,graph,filename) :
      snr_all_channels(sp,&signal,&noise,&snrs,&cross,fbank,codec,graph,
		       filename,0.95);

  bands = (subband) ? fbank->bands : 1;
  
  for (i=0; i<channels; i++)
    for (j=0; j<bands; j++) {
      if (subband) sprintf(entry,"band %d",j);
      printf("<%s> ch. %d %s%s: S = %4.2f dB  N= %4.2f dB  SNR = %4.2f dB\n",
	     filename, i, subband ? entry : "", quick ? " (quick) " : "",
	     signal[i][j],noise[i][j],snrs[i][j]);
      if (codec) 
	if ((cross[i][j] != NEGATIVE_INFINITY)&&
	    ((signal[i][j]-cross[i][j])>CROSS_THRESH))
	  printf("     >> There appears to be crosstalk %5.2f dB below the principal speech.\n",
		 signal[i][j]-cross[i][j]);
    }
  sp_close(sp);
}


/* This function isn't used but I left it in because it might be useful */
Filterbank *get_filters(char *fname)
/* loads filter coefficients */
/* format: <num of bands> <num of taps(1)> <coeff(1)..> <num of taps(2)> <coeff(2)..> */

            
{
  FILE *fp;
  Filterbank *fb;
  int i,j;

  if ((fp = fopen(fname,"r")) == NULL) {
    printf("I can't find the filter bank data file <%s>.\n",fname);
    return DONT_FILTER;
  }

  fb = (Filterbank *) malloc(sizeof(Filterbank));

  fscanf(fp,"%d",&(fb->bands));

  if (fb->bands < 1) {
    printf("The first entry number of <%s>, which should tell me the\n",fname);
    printf("number of bands in the filter bank, reads %d. There won't\n",fb->bands);
    printf("be any filtering, therefore, in this run.\n");
    return DONT_FILTER;
  }

  fb->coeff = (double **) malloc (fb->bands*sizeof(double *));
  fb->taps = (int *) malloc (fb->bands*sizeof(int));

  for (i=0; i<fb->bands; i++) {
    fscanf(fp,"%d",&(fb->taps[i])); /* how many taps in this filter? */
    fb->coeff[i] = (double *) malloc (fb->taps[i]*sizeof(double));
    for (j=0; j<fb->taps[i]; j++)
      fscanf(fp,"%lf",&(fb->coeff[i][j]));
  }
  
  fclose(fp);
  return fb;
}


Filterbank *get_fbank(char *fname)
{
  FILE *fp;
  Filterbank *fb;
  double start,end;
  int i,taps;
  double weight;

  if ((fp = fopen(fname,"r")) == NULL) {
    printf("I can't find the filter bank data file <%s>.\n",fname);
    return DONT_FILTER;
  }

  fb = (Filterbank *) malloc(sizeof(Filterbank));

  fscanf(fp,"%d",&(fb->bands));

  if (fb->bands < 1) {
    printf("The first entry number of <%s>, which should tell me the\n",fname);
    printf("number of bands in the filter bank, reads %d. There won't\n",fb->bands);
    printf("be any filtering, therefore, in this run.\n");
    return DONT_FILTER;
  }

  fb->coeff = (double **) malloc (fb->bands*sizeof(double *));
  fb->taps = (int *) malloc (fb->bands*sizeof(int));
  fb->weights = (double *) malloc (fb->bands*sizeof(double));

  for (i=0; i<fb->bands; i++) {
    if (fscanf(fp,"%lf %lf %d %lf",&start,&end,&taps,&weight) == EOF) {
      printf("There weren't enough entries in <%s> to completely specify all %d filters.\n",
	     fname,fb->bands);
      printf("I only got up to filter #%d. There won't be any filtering, therefore, \n",i);
      printf("in this run.\n");
      return DONT_FILTER;
    }

    fb->taps[i] = taps;
    fb->weights[i] = weight;
    fb->coeff[i] = (double *) malloc ((taps + 1)*sizeof(double));
    design_bandpass_filter(fb->coeff[i],start,end,taps,weight);
  }

  return fb;
}


void print_help_and_exit (void)
{
  printf("\nUsage: stnr [-fhqs] <filename>\n");
  printf("  -f : run on a list of files. Filename must contain an ASCII list of\n");
  printf("       filenames separated by whitespace (tabs, spaces, or newlines).\n");
  printf("  -h : prints this message.\n");
  printf("  -g : prepare a graph for use in gnuplot.\n");
  printf("  -q : quick SNR algorithm (default is slower but more accurate).\n\n");
  printf("  -s : compute SNR on a sub-band basis. stnr reads an ASCII file called\n");
  printf("       'filterbank' which should contain the following information:\n");
  printf("\n");
  printf("       <number of subbands> \n");
  printf("       <subband 1 start freq> <subband 1 end freq> <number of taps> <weight 1>\n");
  printf("       <subband 2 start freq> <subband 2 end freq> <number of taps> <weight 2>\n");
  printf("             ....\n");
  printf("       <subband n start freq> <subband n end freq> <number of taps> <weight n>\n");
  printf("\n");
  printf("       Frequencies should be specified between 0 and 0.5, where 0 is DC\n");
  printf("       and 0.5 is half of the sampling rate (pi radians/sec). The number\n");
  printf("       of taps corresponds to the length of the linear phase FIR filter\n");
  printf("       that should be used. <weight> specifies the desired ratio of passband\n");
  printf("       to stopband ripple in the design process (Parks-McClellan algorithm).\n");
  printf("       Note that the subbands do not need to be specified in any particular\n");
  printf("       frequency order, and they may overlap.\n");
  printf("\n");
  printf("stnr estimates the speech to noise ratio in dB of .wav files.\n\n");
  exit(-1);
}
