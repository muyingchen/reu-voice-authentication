/* wavmd.c */
/* Wave M.D.: The Wave Doctor. */
/* This program checks a batch of files, characterizes their speech and noise */
/* properties, and investigates outliers. */

#include <snr/snrlib.h>

#define MAXFILES 5000      /* the most files we can read at once. */
#define BLOCKSIZE 2048     /* buffer size for reading samples */

#define OUTLIER_LOW -1            /* classification of outliers */
#define OUTLIER_HIGH 1

#define UPDATE_SUMS(s,x) (s).sum += ((double)(x));   \
                         (s).sum_sqr += ((double)(x))*((double)(x))

#define RUN_PROCESS(command,pipe,out)     printf("  | %s\n",command);         \
                                          printf("  | \n");                   \
                                          pipe = (FILE *)popen(command,"r");  \
                                          while (fgets(out,100,pipe) != NULL) \
                                            printf("  | %s",out);             \
                                          pclose(pipe)

#define OUTLIER(index,feature) (results[index].outlier[feature])

#define FEA_NUM 7   /* the number of features we're measuring */
static char *fea_name[] = {  "speech power", "noise power",
			     "speech to noise ratio","duration",
			       "speech duration as % of total",
			     "beginning silence duration",
			     "trailing silence duration"};

static char *fea_high[] = {"HIGH","HIGH","HIGH","LONG","HIGH","LONG","LONG"};
static char *fea_low[] = {"LOW","LOW","LOW","SHORT","LOW","SHORT","SHORT"};

static char *fea_unit[] = {"dB","dB","dB","sec","%","sec","sec"};

typedef struct dat {
  char name[50];
  int channel;
  double feature[FEA_NUM];
  int length;
  int outlier[FEA_NUM];
} Datum;

typedef struct stat {
  double sum,sum_sqr;
  double mean,sd; 
} Statistic;

int channel_count = 0;
Statistic stats[FEA_NUM];

int file_count = 0;
char filename[MAXFILES][200];
int current_file;

char group_name[50];
FILE *group;

Datum *results;
int quick,whole_dir,primary,secondary,codec,verbose;

/* wavmd.c */	double samples_to_seconds PROTO((int samples, int rate));
/* wavmd.c */	void summarize_outliers PROTO((void));
/* wavmd.c */	SP_FILE *grab_next_file PROTO((char *name));
/* wavmd.c */	void print_sig_noise_snr PROTO((char *name, double signal, double noise, double snr));
/* wavmd.c */	void initialize_statistics PROTO((void));
/* wavmd.c */	void initialize_file_grabber PROTO((void));
/* wavmd.c */	int samples PROTO((SP_FILE *in_file));
/* wavmd.c */	int sample_rate PROTO((SP_FILE *in_file));
/* wavmd.c */	void investigate_outlier PROTO((int index));
/* wavmd.c */	double samples_to_seconds PROTO((int samples, int rate));
/* wavmd.c */	void print_help_and_exit  PROTO((void));
/* wavmd.c */	void compile_statistics  PROTO((char *name, int channels, double **signal, double **noise, double **snr, int samp_rate, int sample_count, int speech_frame_count, double begin_silence, double end_of_speech));
/* wavmd.c */	void print_statistics  PROTO((void));
/* wavmd.c */	void compute_statistics  PROTO((void));
/* wavmd.c */	int identify_outlier  PROTO((int i));
/* wavmd.c */	int speech_frames  PROTO((SP_FILE *in_file, double speech, double noise, double *begin_silence, double *end_of_speech));

int main(int argc, char **argv)
{
  SP_FILE *in_file;
  char in_file_name[50],c;
  int k,i,channels,sample_count,samp_rate,outlier;
  double **signal,**noise,**snrs,**cross;
  int count,speech_frame_count;
  double begin_silence,end_of_speech;

  if (argc > 3)
    print_help_and_exit();


  /* parse the options */

  quick=0;     /* not quick by default */
  whole_dir=1; /* analyze entire directory by default. */
  primary=0;   /* not just primary mikes */
  secondary=0; /* not just secondary mikes */
  codec=0;     /* assume they aren't CODEC files */
  verbose=0;   /* don't print verbose report */

  for (k=1; k<argc; k++) {
    if (argv[k][0] == '-')
      while ((c = *++argv[k])) /* leave this structure general in case we add more options. */
	switch (c) {
	case 'c':
	  codec = 1;
	  break;
	case 'q':
	  quick = 1;
	  break;
	case 'p':
	  primary=1;
	  if (secondary) primary=secondary=0;
	  break;
	case 's':
	  secondary=1;
	  if (primary) primary=secondary=0;
	  break;
	case 'v':
	  verbose=1;
	  break;
	case 'h':
	  print_help_and_exit();
	default:
	  printf("I don't know the option -%c. I only know -chpqs. Try -h for help.\n",c);
	  exit(-1);
	}
    else {
      sprintf(group_name,"%s",argv[k]);
      whole_dir=0;
    }
  }

  initialize_file_grabber();

  initialize_statistics();


  /* loop through all the files */

  while (current_file < file_count) {
    if ((in_file = grab_next_file(in_file_name)) == SPNULL) {
      printf("I can't find <%s>. I'll skip it.\n",in_file_name);
      continue;
    }
    channels = snr_all_channels (in_file,&signal,&noise,&snrs,&cross,DONT_FILTER,codec,0,(char *)0,0.95);
    sample_count = samples (in_file);
    samp_rate = sample_rate(in_file);
    speech_frame_count = speech_frames (in_file,signal[0][0],noise[0][0],
					&begin_silence,&end_of_speech);
    compile_statistics (in_file_name,channels,signal,noise,snrs,samp_rate,
			sample_count,speech_frame_count,begin_silence,end_of_speech);
    if (verbose)
      print_sig_noise_snr(in_file_name,signal[0][0],noise[0][0],snrs[0][0]);
    
    sp_close(in_file);
  }

  compute_statistics();

  printf("\n*******************************************\n");
  printf("*                                         *\n");
  printf("*      STATISTICS FOR THIS ENSEMBLE       *\n");
  printf("*                                         *\n");
  printf("*******************************************\n");

  print_statistics();

  printf("\n*******************************************\n");
  printf("*                                         *\n");
  printf("*      SUMMARY OF OUTLIERS                *\n");
  printf("*                                         *\n");
  printf("*******************************************\n");

  summarize_outliers();

  printf("\n*******************************************\n");
  printf("*                                         *\n");
  printf("*      IDENTIFICATION OF OUTLIERS         *\n");
  printf("*        AND FURTHER ANALYSIS             *\n");
  printf("*                                         *\n");
  printf("*******************************************\n");

  count=0;
  for (i=0; i < channel_count; i++) {
    outlier = identify_outlier(i);
    count += outlier;
    if (outlier && !quick)
      investigate_outlier(i);
  }

  if (!count) printf("\n\nThere are no outliers to report on in this set.\n");
  return 0;
}

void summarize_outliers(void)
{
  int i,f,high,low;

  for (i=0; i<channel_count; i++) 
    for (f=0; f < FEA_NUM; f++) {
      results[i].outlier[f]=0;
      if ((results[i].feature[f] - stats[f].mean) > 2.0*stats[f].sd) {
	results[i].outlier[f]=OUTLIER_HIGH;
      }
      if ((results[i].feature[f] - stats[f].mean) < -2.0*stats[f].sd) {
	results[i].outlier[f]=OUTLIER_LOW;
      }
    }

  for (f=0; f<FEA_NUM; f++) {
    printf("\n\n---------%s----------\n",fea_name[f]);
    high=0; 
    low=0;
    for (i=0; i<channel_count; i++) {
      if (results[i].outlier[f]==OUTLIER_HIGH) high++;
      if (results[i].outlier[f]==OUTLIER_LOW) low++;
    }
    printf("%d of %d channels had an unusually %s %s.\n",
	   high,channel_count,fea_high[f],fea_name[f]);
    printf("%d of %d channels had an unusually %s %s.\n",
	   low,channel_count,fea_low[f],fea_name[f]);
  }
  printf("\n\n");
}


SP_FILE *grab_next_file(char *name)
{
  SP_FILE *fp;

  strcpy(name,filename[current_file++]);
  fp = sp_open(name,"r");
  return fp;
}
  

int identify_outlier (int i)
{
  int f;
  int noteworthy;

  noteworthy=0; /* asssume this result is not an outlier in any dimension */
  for (f=0; f<FEA_NUM; f++)
    if (results[i].outlier[f]) {
      noteworthy=1;
      break;
    }
    
  if (noteworthy) {
    printf("\n===========================================\n");
    printf("<%s channel %d>\n",results[i].name,results[i].channel);
    printf("===========================================\n\n");
    for (f=0; f<FEA_NUM; f++) {
      switch (results[i].outlier[f]) {
      case OUTLIER_LOW:
	printf("  * %s %s",fea_low[f],fea_name[f]);
	break;
      case OUTLIER_HIGH:
	printf("  * %s %s",fea_high[f],fea_name[f]);
	break;
      }
      if (results[i].outlier[f]) {
	printf("  (%.2f %s) mean val. = %.2f %s\n",results[i].feature[f],fea_unit[f],
	       stats[f].mean,fea_unit[f]);
      }
    }
  }
  return noteworthy;
}

void print_statistics (void)
{
  int i;
  
  printf("This ensemble of %d files contained %d audio channels.\n\n",file_count,channel_count);

  for (i=0; i<FEA_NUM; i++) 
    printf("mean %s = %5.2f %s     standard dev. = %5.2f %s\n",
	   fea_name[i],stats[i].mean,fea_unit[i],stats[i].sd,fea_unit[i]);
}

void compute_statistics (void)
{
  int i;
  double second_moment;

  /* uses trick that variance = E(x**2) - E(x)**2 */

  for (i=0; i<FEA_NUM; i++) {
    stats[i].mean = stats[i].sum / ((double) channel_count);
    second_moment = stats[i].sum_sqr / ((double) channel_count);
    if (second_moment - stats[i].mean*stats[i].mean <= 0.0)
	stats[i].sd = 0.0;
    else
	stats[i].sd = (double) sqrt(second_moment - stats[i].mean*stats[i].mean);
  }
}
  

void compile_statistics (char *name, int channels, double **signal, double **noise, double **snr, int samp_rate, int sample_count, int speech_frame_count, double begin_silence, double end_of_speech)
{
  int chan;
  double speech_ratio,seconds,end_silence;

  for (chan=0; chan<channels; chan++) {
    /* compile this datum */
    strcpy(results[channel_count].name,name);

    seconds = samples_to_seconds(sample_count,samp_rate);
    speech_ratio=((double) speech_frame_count / seconds);  /* percentage 0--100 */
                                                          /* N.B. 1 frame == .01 sec */
    end_silence=seconds-end_of_speech;
    
    results[channel_count].channel=chan;
    results[channel_count].feature[0] = signal[chan][0];  /* speech pow., subband 0 */
    results[channel_count].feature[1] = noise[chan][0];   /* noise pow., subband 0 */
    results[channel_count].feature[2] = snr[chan][0];     /* snr, subband 0 */
    results[channel_count].feature[3] = seconds;          /* duration */
    results[channel_count].feature[4] = speech_ratio;     /* speech percentage */
    results[channel_count].feature[5] = begin_silence;    /* silence at head */
    results[channel_count].feature[6] = end_silence;      /* silence at tail */
    /* update totals */
    UPDATE_SUMS(stats[0],signal[chan][0]);
    UPDATE_SUMS(stats[1],noise[chan][0]);
    UPDATE_SUMS(stats[2],snr[chan][0]);
    if (chan==0) {  /* don't want to update these twice with the same data */
      UPDATE_SUMS(stats[3],seconds);
      UPDATE_SUMS(stats[4],speech_ratio);
      UPDATE_SUMS(stats[5],begin_silence);
      UPDATE_SUMS(stats[6],end_silence);
    }
    channel_count++;
  }
}

    
void print_sig_noise_snr(char *name, double signal, double noise, double snr)
{
  FILE *fp1, *fp2, *fp3, *fp4;
  fp1=fopen("signal.his","a");
  if (fp1==NULL)
    {
      fprintf(stderr,"Error: Could not open \'signal.his\'\n");
      exit(-1);
    }
  fp2=fopen("noise.his","a");
  if (fp2==NULL)
    {
      fprintf(stderr,"Error: Could not open \'noise.his\'\n");
      exit(-1);
    }
  fp3=fopen("snr.his","a");
  if (fp3==NULL)
    {
      fprintf(stderr,"Error: Could not open \'snr.his\'\n");
      exit(-1);
    }
  fp4=fopen("stats.out","a");
  if (fp4==NULL)
    {
      fprintf(stderr,"Error: Could not open \'stats.out\'\n");
      exit(-1);
    }
  fprintf(fp1,"%.0f\n",signal);
  fprintf(fp2,"%.0f\n",noise);
  fprintf(fp3,"%.0f\n",snr);
  fprintf(fp4,"file=%s   sig=%.0f   noise=%.0f   snr=%.0f\n",name,signal,noise,snr);

  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp4);
}


void initialize_statistics(void)
{
  int i;

  for (i=0; i<FEA_NUM; i++) {
    stats[i].sum=0;
    stats[i].sum_sqr=0;
    stats[i].mean=0;
    stats[i].sd=0;
  }

  results = (Datum *) malloc (file_count * sizeof (Datum));
}

void initialize_file_grabber(void)
{
  char temp[L_tmpnam];
  char name[50];
  char command_line[100];

  if (!whole_dir) {
    /* read in the list of file names from a file */
    if ((group = fopen(group_name,"r")) == NULL) {
      printf("I can't find %s.\n",group_name);
      exit(-1);
    }
  } else {
    /* read in the current directory */
    tmpnam(temp);
    sprintf(command_line,"ls | grep %s.wav > %s",
	    (primary) ? "ss" : ((secondary) ? "sc" : ""),
	    temp);
    system(command_line);
    if ((group = fopen(temp,"r")) == NULL) {
      printf("wavmd: error opening temp file.\n");
      exit(-1);
    }
  }

  /* stick 'em in the array */
  while (fscanf(group,"%s",name) != EOF) 
    sprintf(filename[file_count++],"%s",name);
    
  
  if (whole_dir) {
    sprintf(command_line,"rm %s",temp);
    system(command_line);
  }

  fclose(group);
  current_file = 0;
}    

int samples(SP_FILE *sp)
{
    SP_INTEGER sample_count;
    
    sp_rewind(sp);

    if (sp_h_get_field(sp, "sample_count", T_INTEGER,(void *) &sample_count)){
	fprintf(spfp,"%s: unable to get sample_count field\n","samples");
	sp_print_return_status(spfp);
	return(-1);
    }
    return (int) sample_count;
}

int sample_rate(SP_FILE *sp)
{
    SP_INTEGER sample_rate;
    
    sp_rewind(sp);

    if (sp_h_get_field(sp, "sample_rate", T_INTEGER,(void *) &sample_rate)){
	fprintf(spfp,"%s: unable to get sample_rate field\n","samples");
	sp_print_return_status(spfp);
	return(-1);
    }
    return (int) sample_rate;
}

int speech_frames (SP_FILE *in_file, double speech, double noise, double *begin_silence, double *end_of_speech)
{
  int **list;
  int segments,sum=0,i;
  int max_segment = 500;

  alloc_2dimZ(list,max_segment,2,int,0);

  segments = segment_speech (in_file,speech,noise,&max_segment, &list);

  *begin_silence = (segments==0) ? 0.0 : list[0][FROM]/100.0;
  *end_of_speech = (segments==0) ? 0.0 : list[segments-1][TO]/100.0;

  for (i=0; i<segments; i++) 
    sum += list[i][TO] - list[i][FROM] + 1;
  
  free_2dimarr(list,max_segment,int);

  return sum;
}
 

void investigate_outlier(int index)
{
  char command_line[100];
  char output_line[100];
  int outlier=0;
  FILE *pipe;

  outlier = ((OUTLIER(index,0)==OUTLIER_HIGH) ||
	     (OUTLIER(index,1)==OUTLIER_HIGH) ||
	     (OUTLIER(index,2)==OUTLIER_LOW));

  if (!outlier) return;


  /* check the header */

  /* if the noise or speech powers are high, or SNR is low */
  if ((OUTLIER(index,0)==OUTLIER_HIGH) || (OUTLIER(index,1)==OUTLIER_HIGH) ||
      (OUTLIER(index,2)==OUTLIER_LOW)) {
    sprintf(command_line,"headcheck %s",results[index].name);
    printf("\n  running diagnostic to check header information:");
    printf("\n  +---------------------------------------------\n");
    RUN_PROCESS(command_line,pipe,output_line);
  }

  /* check for ground hum */

  /* if the noise is high or the SNR is low */
  if ((OUTLIER(index,0)==OUTLIER_HIGH) || (OUTLIER (index,2)==OUTLIER_LOW)) {
    sprintf(command_line,"autohum %s",results[index].name);
    printf("\n  running diagnostic to check for 60-Hz related line components:");
    printf("\n  +------------------------------------------------------------\n");
    RUN_PROCESS(command_line,pipe,output_line);
  }

  /* check for clipping */

  /* if the speech is hot  */
  if (OUTLIER(index,1)==OUTLIER_HIGH) {
    sprintf(command_line,"clp %s",results[index].name);
    printf("\n  running diagnostic to check for clipping:");
    printf("\n  +---------------------------------------\n");
    RUN_PROCESS(command_line,pipe,output_line);
  }

  printf("\n\n");
}

double samples_to_seconds(int samples, int rate)
{
  return (((double) samples) / (double) rate);
}

void print_help_and_exit (void)
{
  printf("\n");
  printf("Usage: wavmd [-chpqs] [input file]\n\n");
  printf("\n");
  printf("  -c : use the SNR estimation algorithm that has been\n");
  printf("       modified for use with CODEC files from the    \n");
  printf("       Switchboard Corpus.");
  printf("  -h : prints this message.\n");
  printf("  -p : primary microphone recordings only.\n");
  printf("  -q : quick option. Skip the follow-up tests on outliers.\n");
  printf("  -s : secondary microphone recordings only.\n");
  printf("\n");
  printf("Wavmd (the Wave Doctor) checks an ensemble of .wav files, tallies\n");
  printf("statistics on some of their features, and investigates the outliers\n");
  printf("with a more sophisticated battery of tests. The -q (\"quick\") option\n");
  printf("skips the follow-up tests. The features currently measured are:\n");
  printf("\n");
  printf("            * speech power\n");
  printf("            * noise power\n");
  printf("            * speech to noise ratio\n");
  printf("            * file length\n");
  printf("            * ratio of speech activity duration to total duration\n");
  printf("            * length of initial silence\n");
  printf("            * length of trailing silence\n");
  printf("\n");
  printf("If an input file is specified, this file is assumed to contain a list\n");
  printf("of file names that should be checked. Otherwise, by default, wavmd\n");
  printf("checks every file with a .wav extension in the current directory.\n");
  printf("\n");
  exit(-1);
}
