/* atismd.c */
/* ATIS M.D.: The ATIS Doctor. */
/* This program checks a batch of files, characterizes their speech and noise */
/* properties, and investigates outliers. */

#include <snr/snrlib.h>

#define MAX_FILES 5000     /* the most files we can read at once. */

#define VERY_LOW -1        /* classification of outliers */
#define VERY_HIGH 1

#define PRIMARY         0  /* microphone types */
#define SECONDARY       1
#define MIKE_UNKNOWN   -1

#define UPDATE_SUMS(s,x) (s).sum += (x);                                      \
                         (s).sum_sqr += (x)*(x);                              \
                         (s).number++


#define RUN_PROCESS(command,pipe,out)     printf("  | %s\n",command);         \
                                          printf("  | \n");                   \
                                          pipe = (FILE *)popen(command,"r");  \
                                          while (fgets(out,100,pipe) != NULL) \
                                            printf("  | %s",out);             \
                                          pclose(pipe)

#define OUTLIER(index,feature) (results[index].outlier[feature])

#define INITIALIZE_STAT(stat)                                                 \
{                                                                             \
  int _i;                                                                     \
                                                                              \
  for (_i=0; _i<FEA_NUM; _i++) {                                              \
    stat[_i].sum=0;                                                           \
    stat[_i].sum_sqr=0;                                                       \
    stat[_i].mean=0;                                                          \
    stat[_i].sd=0;                                                            \
    stat[_i].number=0;                                                        \
  }                                                                           \
}       


/* FEATURE INFORMATION */

#define FEA_NUM 7   /* the number of features we're measuring */
static char *fea_name[] = {  "speech power", "noise power",
			     "speech to noise ratio","duration",
			       "speech duration as % of total",
			     "beginning silence duration",
			     "trailing silence duration"};
static char *fea_high[] = {"HIGH","HIGH","HIGH","LONG","HIGH","LONG","LONG"};
static char *fea_low[] = {"LOW","LOW","LOW","SHORT","LOW","SHORT","SHORT"};
static char *fea_unit[] = {"dB","dB","dB","sec","%","sec","sec"};
static char *fea_lf[] = {"\n","\n\n","\n\n","\n","\n\n","\n","\n"};  /* formatting the report */
                                                                     /* linefeeds after item */


/* SITE INFORMATION */

#define SITE_NUM 9
#define SITE_UNKNOWN (SITE_NUM-1)
#define SITE_ATT 7
/* AT&T must be eighth; unknown must be last */
static char *site_name[] = {"MIT","TI","UNISYS","BBN","CMU","SRI","NIST","AT&T","unknown"};
/* for speaker id's: low and high boundaries */
static char *site_low[] = {"00","b0","d0","e0","i0","m0","x0"};  
static char *site_high[]= {"8z","cz","dz","gz","lz","tz","zz"};


typedef struct dat {
  char name[50];
  double *feature;
  int *outlier;
} Datum;

typedef struct stat {
  double sum,sum_sqr;
  double mean,sd; 
  int number;
} Statistic;


Statistic *stats;
Statistic *p_stats;
Statistic *s_stats;

Datum *results;

int file_count = 0;
int speaker_count = 0;
int session_count = 0;
int site_count = 0;

char **filename;
int current_file;

FILE *group;
char group_name[80];

int quick,name_specified,primary,secondary,root,enhanced;

/* atismd.c */	double samples_to_seconds PROTO((int samples));
/* atismd.c */	char *strip_pathname PROTO((char *name));
/* atismd.c */	void identify_bad_sessions PROTO((void));
/* atismd.c */	void summarize_outliers PROTO((void));
/* atismd.c */	SP_FILE *grab_next_file PROTO((char *name));
/* atismd.c */	void update PROTO((Statistic *s, double signal, double noise, double snr, double seconds, double speech_ratio, double begin_silence, double end_silence));
/* atismd.c */	void initialize_statistics PROTO((int how_many));
/* atismd.c */	int str_comp PROTO((const void *s1, const void *s2));
/* atismd.c */	int initialize_file_grabber PROTO((void));
/* atismd.c */	int samples PROTO((SP_FILE *in_file));
/* atismd.c */	void investigate_outlier PROTO((int index));
/* atismd.c */	double samples_to_seconds PROTO((int samples));
/* atismd.c */	char *strip_pathname PROTO((char *name));
/* atismd.c */	int get_site PROTO((char *name));
/* atismd.c */	int get_mike PROTO((char *name));
/* atismd.c */	void get_speaker PROTO((char *name, char *speaker));
/* atismd.c */	char get_session PROTO((char *name));
/* atismd.c */	SP_FILE *get_primary PROTO((char *name, char *outname));
/* atismd.c */	void print_statistics  PROTO((void));
/* atismd.c */	void print_site_statistics  PROTO((void));
/* atismd.c */	void compute_statistics  PROTO((Statistic *stat));
/* atismd.c */	void make_full_name  PROTO((char *name, char *full_name, char *root_dir));
/* atismd.c */	void compile_statistics  PROTO((char *name, double signal, double noise, double snr, int sample_count, int speech_frame_count, double begin_silence, double end_silence));
/* atismd.c */	void print_help_and_exit  PROTO((void));
/* atismd.c */	int speech_frames  PROTO((int **list, int segments));
/* atismd.c */	int identify_outlier  PROTO((int i));

int main (int argc, char **argv)
{
  SP_FILE *in_file,*primary_file;
  char in_file_name[1024],c,primary_file_name[1024],title[512];
  int k,i,sample_count,outlier,**list;
  double **signal,**noise,**snrs,**cross,seg_signal,seg_noise,seg_snr;
  double seconds,speech_ratio,begin_silence,end_silence;
  int count,speech_frame_count,segments;
  int enhancement_attempted=0,enhancement_successful=0;
  int primary_available,mike,report_title_specified;
  int max_segment = 500;

  /* parse the options */

  quick=0;     /* not quick by default */
  root=0;      /* not root by default */
  name_specified=0; /* analyze entire directory by default. */
  primary=0;
  secondary=0;
  enhanced=0;      /* by default, analyze both microphone types */
  report_title_specified=0;
  strcpy(title,"");

  for (k=1; k<argc; k++) {
    if ((argv[k][0] == '-') && !report_title_specified)
      while ((c = *++argv[k]))
	switch (c) {
	case 'h':
	  print_help_and_exit();
	case 'n':
	  report_title_specified=1;
	  break;
	case 'q':
	  quick = 1;
	  break;
	case 'r':
	  root = 1;
	  break;
	case 'p':
	  primary = 1;
	  enhanced=0;
	  if (secondary) {
	    primary=secondary=0;
	    enhanced=1;
	  }
	  break;
	case 's':
	  secondary = 1;
	  enhanced=0;
	  if (primary) {
	    primary=secondary=0;
	    enhanced=1;
	  }
	  break;
	case 'e':
	  enhanced=1;
	  primary=secondary=0;
	  break;
	default:
	  printf("I don't know the option -%c. I only know -hnpqrs.\n",c);
	  exit(-1);
	}
    else {
      if (report_title_specified) {
	strcat(title,argv[k]);
	strcat(title," ");
      } else {
	sprintf(group_name,"%s",argv[k]);
	name_specified=1;
      }
    }
  }

  if (!report_title_specified) {
    printf("Enter the title of this report, followed by a carriage return:\n");
    fgets(title, 512, stdin);
  }

  count = initialize_file_grabber();

  initialize_statistics(count);
  alloc_2dimZ(list,max_segment,2,int,0);

  /* loop through all the files */
  while (current_file < count) {

    if ((in_file = grab_next_file(in_file_name)) == SPNULL) {
      printf("I can't find <%s>. I'll skip it.\n",in_file_name);
      continue;
    }

    /* if we've already done this file, skip it. This may happen in */
    /* enhanced mode, when we process two files at once. */
    if (file_count != 0)
      if (!strcmp(in_file_name,results[file_count-1].name)) {
	  sp_close(in_file);
	continue;
      }
    
    primary_available=0;

    mike = get_mike(in_file_name);

    if (enhanced && (mike==SECONDARY)) {
      primary_available = ((primary_file = get_primary(in_file_name,primary_file_name)) != NULL);
      if (primary_available) enhancement_successful++;
      enhancement_attempted++;
    }

    snr_all_channels ((primary_available ? primary_file : in_file),
		      &signal,&noise,&snrs,&cross,DONT_FILTER,0,0,"",0.95); /* 0 means NOT CODEC */
    
    segments = segment_speech((primary_available ? primary_file : in_file),
			      signal[0][0],noise[0][0],&max_segment, &list);

    if (primary_available) 
      segsnr_single_channel(in_file,0,&seg_noise,&seg_signal,&seg_snr,list,segments,
			    NULL_FILTER,0);

    sample_count = samples (in_file);
    speech_frame_count = speech_frames (list,segments);
    seconds = samples_to_seconds(sample_count);
    speech_ratio=((double) speech_frame_count / seconds);
    begin_silence = (segments==0) ? 0.0 : list[0][FROM]/100.0;
    end_silence = (segments==0) ? 0.0 : (seconds - (double) list[segments-1][TO]/100.0);

    if (primary_available) {
      compile_statistics (in_file_name,seg_signal,seg_noise,seg_snr,
			  sample_count,speech_frame_count,begin_silence,end_silence);
      compile_statistics (primary_file_name,signal[0][0],noise[0][0],snrs[0][0],
			  sample_count,speech_frame_count,begin_silence,end_silence);
      update(p_stats,signal[0][0],noise[0][0],snrs[0][0],seconds,speech_ratio,
	     begin_silence,end_silence);
      update(s_stats,seg_signal,seg_noise,seg_snr,seconds,speech_ratio,
	     begin_silence,end_silence);
    }
    else if (enhanced) {
      update((mike == SECONDARY) ? s_stats : p_stats,
	     signal[0][0],noise[0][0],snrs[0][0],seconds,speech_ratio,begin_silence,end_silence);
      compile_statistics (in_file_name,signal[0][0],noise[0][0],snrs[0][0],
			  sample_count,speech_frame_count,begin_silence,end_silence);
    } else       
      compile_statistics (in_file_name,signal[0][0],noise[0][0],snrs[0][0],
			  sample_count,speech_frame_count,begin_silence,end_silence);

    free(signal[0]); free(signal);
    free(noise[0]); free(noise);
    free(snrs[0]); free(snrs);


    sp_close(in_file);
    if (primary_available) sp_close(primary_file);
  }

  if (enhanced) {
    compute_statistics(p_stats);
    compute_statistics(s_stats);
  }
  else
    compute_statistics(stats);

  /* print title information */

  for (i=0; i<(strlen(title)+1); i++)                 
    printf("=");                                  
  printf("\n %s\n",title);                          
  for (i=0; i<(strlen(title)+1); i++)                 
    printf("=");                                  
  printf("\n");                                   
  printf("\n\n");
  if (primary) printf("Primary microphone recordings only.\n\n");
  if (secondary) printf("Secondary microphone recordings only.\n");
  if (enhanced) {
    printf("Both primary and secondary recordings.\n\n");
    printf("When possible, measurements were enhanced by using segmentation\n");
    printf("information from the primary microphone recordings.\n");
    printf("%d of %d secondary files had corresponding primary files.\n",
	   enhancement_successful,enhancement_attempted);
  }
  printf("\n");

  printf("\n*******************************************\n");
  printf("*                                         *\n");
  printf("*   GLOBAL STATISTICS FOR THIS ENSEMBLE   *\n");
  printf("*                                         *\n");
  printf("*******************************************\n");

  print_statistics();
  
  if (site_count > 1) {
    printf("\n*******************************************\n");
    printf("*                                         *\n");
    printf("*     STATISTICS FOR THIS ENSEMBLE        *\n");
    printf("*         BROKEN DOWN BY SITE             *\n");
    printf("*                                         *\n");
    printf("*******************************************\n");
    
    print_site_statistics();
  }

  printf("\n*******************************************\n");
  printf("*                                         *\n");
  printf("*          SUMMARY OF OUTLIERS            *\n");
  printf("*                                         *\n");
  printf("*******************************************\n");

  summarize_outliers();

  printf("\n*******************************************\n");
  printf("*                                         *\n");
  printf("*       IDENTIFICATION OF OUTLIERS        *\n");
  printf("*          AND FURTHER ANALYSIS           *\n");
  printf("*                                         *\n");
  printf("*******************************************\n");

  identify_bad_sessions();

  count=0;
  for (i=0; i < file_count; i++) {
    outlier = identify_outlier(i);
    count += outlier;
    if (outlier && !quick)
      investigate_outlier(i);
  }

  if (!count) printf("\n\nThere are no outliers to report on in this set.\n");

  free_2dimarr(list,max_segment,int);

  return(0);
}


void identify_bad_sessions(void)
{
  int i,f;
  char speak[3];
  char old_session='-',session;
  int found_one,p_found_one,s_found_one;
  Statistic *sess_stats,*p_sess_stats,*s_sess_stats;
  double distance,p_distance,s_distance;

  if (enhanced) {
    p_sess_stats = (Statistic *) malloc (FEA_NUM * sizeof(Statistic)); 
    s_sess_stats = (Statistic *) malloc (FEA_NUM * sizeof(Statistic)); 
    INITIALIZE_STAT(p_sess_stats);
    INITIALIZE_STAT(s_sess_stats);
  } else {
    sess_stats = (Statistic *) malloc (FEA_NUM * sizeof(Statistic)); 
    INITIALIZE_STAT(sess_stats);
  }

  for (i=0; i<file_count; i++)  {
    if ((((session = get_session(results[i].name)) != old_session) && 
	 (old_session != '-'))) {
      found_one=0; /* haven't found an outlier yet */
      p_found_one=s_found_one=0;

      if (enhanced) {
	compute_statistics(p_sess_stats);
	compute_statistics(s_sess_stats);
      } else
	compute_statistics(sess_stats);

      for (f=0; f < FEA_NUM; f++) {
	get_speaker(results[i-1].name,speak); 
	if (enhanced) {
	  p_distance = p_sess_stats[f].mean - p_stats[f].mean;
	  s_distance = s_sess_stats[f].mean - s_stats[f].mean;
	} else
	  distance = sess_stats[f].mean - stats[f].mean;

	if (enhanced) {
	  if (fabs(p_distance) > 2.0*p_stats[f].sd) {
	    if (!p_found_one) {
	      get_speaker(results[i-1].name,speak);
	      printf("\n< Session %s/%c > (primary microphone) :\n\n",speak,old_session);
	      p_found_one=1;
	    }
	    printf("  * %s %s  ",(p_distance < 0) ? fea_low[f] : fea_high[f],fea_name[f]);
	    printf("(%.2f %s)  mean val. = %.1f %s\n",p_sess_stats[f].mean,fea_unit[f],
		   p_stats[f].mean,fea_unit[f]);
	  }

	  if (fabs(s_distance) > 2.0*s_stats[f].sd) {
	    if (!s_found_one) {
	      get_speaker(results[i-1].name,speak);
	      printf("\n< Session %s/%c > (secondary microphone) :\n\n",speak,old_session);
	      s_found_one=1;
	    }
	    printf("  * %s %s  ",(s_distance < 0) ? fea_low[f] : fea_high[f],fea_name[f]);
	    printf("(%.2f %s)  mean val. = %.1f %s\n",s_sess_stats[f].mean,fea_unit[f],
		   s_stats[f].mean,fea_unit[f]);
	  }
	} else {

	  if (fabs(distance) > 2.0*stats[f].sd) {
	    if (!found_one) {
	      get_speaker(results[i-1].name,speak);
	      printf("\n< Session %s/%c > :\n\n",speak,old_session);
	      found_one=1;
	    }
	    printf("  * %s %s  ",(distance < 0) ? fea_low[f] : fea_high[f],fea_name[f]);
	    printf("(%.2f %s)  mean val. = %.1f %s\n",sess_stats[f].mean,fea_unit[f],
		   stats[f].mean,fea_unit[f]);
	  }
	}
      }
      if (enhanced) {
	INITIALIZE_STAT(p_sess_stats);
	INITIALIZE_STAT(s_sess_stats);
      } else
	INITIALIZE_STAT(sess_stats);
    }
    for (f=0; f<FEA_NUM; f++) {
      if (enhanced) {
	if (get_mike(results[i].name)==PRIMARY) {
	  UPDATE_SUMS(p_sess_stats[f],results[i].feature[f]);
	}
	else {
	  UPDATE_SUMS(s_sess_stats[f],results[i].feature[f]);
	}
      } else {
	UPDATE_SUMS(sess_stats[f],results[i].feature[f]);
      }
    }
    old_session=session;
  }

  /* now handle last session */

  found_one=0; /* haven't found an outlier yet */
  p_found_one=s_found_one=0;

  if (enhanced) {
    compute_statistics(p_sess_stats);
    compute_statistics(s_sess_stats);
  } else
    compute_statistics(sess_stats);
  
  for (f=0; f < FEA_NUM; f++) {
    get_speaker(results[i-1].name,speak); 
    if (enhanced) {
      p_distance = p_sess_stats[f].mean - p_stats[f].mean;
      s_distance = s_sess_stats[f].mean - s_stats[f].mean;
    } else
      distance = sess_stats[f].mean - stats[f].mean;

    if (enhanced) {
      if (fabs(p_distance) > 2.0*p_stats[f].sd) {
	if (!p_found_one) {
	  get_speaker(results[i-1].name,speak);
	  printf("\n< Session %s/%c > (primary microphone) :\n\n",speak,old_session);
	  p_found_one=1;
	}
	printf("  * %s %s  ",(p_distance < 0) ? fea_low[f] : fea_high[f],fea_name[f]);
	printf("(%.2f %s)  mean val. = %.1f %s\n",p_sess_stats[f].mean,fea_unit[f],
	       p_stats[f].mean,fea_unit[f]);
      }
      
      if (fabs(s_distance) > 2.0*s_stats[f].sd) {
	if (!s_found_one) {
	  get_speaker(results[i-1].name,speak);
	  printf("\n< Session %s/%c > (secondary microphone) :\n\n",speak,old_session);
	  s_found_one=1;
	}
	printf("  * %s %s  ",(s_distance < 0) ? fea_low[f] : fea_high[f],fea_name[f]);
	printf("(%.2f %s)  mean val. = %.1f %s\n",s_sess_stats[f].mean,fea_unit[f],
	       s_stats[f].mean,fea_unit[f]);
      }
    } else {

      if (fabs(distance) > 2.0*stats[f].sd) {
	if (!found_one) {
	  get_speaker(results[i-1].name,speak);
	  printf("\n< Session %s/%c > :\n\n",speak,old_session);
	  found_one=1;
	}
	printf("  * %s %s  ",(distance < 0) ? fea_low[f] : fea_high[f],fea_name[f]);
	printf("(%.2f %s)  mean val. = %.1f %s\n",sess_stats[f].mean,fea_unit[f],
	       stats[f].mean,fea_unit[f]);
      }
    }
  }

  if (enhanced) {
    free (p_sess_stats); 
    free (s_sess_stats); 
  } else
    free (sess_stats); 
}


void summarize_outliers(void)
{
  int i,f,high,low,mike;


  for (i=0; i<file_count; i++) 
    for (f=0; f < FEA_NUM; f++) {
      results[i].outlier[f]=0;
      if (enhanced) {
	mike = get_mike(results[i].name);
	switch (mike) {
	case PRIMARY:
	  if ((results[i].feature[f] - p_stats[f].mean) > 2.0*p_stats[f].sd) {
	    results[i].outlier[f]=VERY_HIGH;
	  }
	  if ((results[i].feature[f] - p_stats[f].mean) < -2.0*p_stats[f].sd) {
	    results[i].outlier[f]=VERY_LOW;
	  }
	  break;
	case SECONDARY:
	  if ((results[i].feature[f] - s_stats[f].mean) > 2.0*s_stats[f].sd) {
	    results[i].outlier[f]=VERY_HIGH;
	  }
	  if ((results[i].feature[f] - s_stats[f].mean) < -2.0*s_stats[f].sd) {
	    results[i].outlier[f]=VERY_LOW;
	  }
	  break;
	}
      } else {
	if ((results[i].feature[f] - stats[f].mean) > 2.0*stats[f].sd) {
	  results[i].outlier[f]=VERY_HIGH;
	}
	if ((results[i].feature[f] - stats[f].mean) < -2.0*stats[f].sd) {
	  results[i].outlier[f]=VERY_LOW;
	}
      }

    }

  for (f=0; f<FEA_NUM; f++) {
    printf("\n\n---------%s----------\n",fea_name[f]);
    high=0; 
    low=0;
    for (i=0; i<file_count; i++) {
      if (results[i].outlier[f]==VERY_HIGH) high++;
      if (results[i].outlier[f]==VERY_LOW) low++;
    }
    printf("%d of %d files had an unusually %s %s.\n",
	   high,file_count,fea_high[f],fea_name[f]);
    printf("%d of %d files had an unusually %s %s.\n",
	   low,file_count,fea_low[f],fea_name[f]);
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
  char *site,speak[3],sess;

  noteworthy=0; /* asssume this result is not an outlier in any dimension */
  for (f=0; f<FEA_NUM; f++)
    if (results[i].outlier[f]) {
      noteworthy=1;
      break;
    }
    
  if (noteworthy) {
    printf("\n===========================================\n");
    printf("<%s>\n",results[i].name);
    printf("===========================================\n");
    site = site_name[get_site(results[i].name)];
    get_speaker(results[i].name,speak);
    sess = get_session(results[i].name);
    printf("site: %s   speaker: %s   session: %c\n\n",site,speak,sess);
    for (f=0; f<FEA_NUM; f++) {
      switch (results[i].outlier[f]) {
      case VERY_LOW:
	printf("  * %s %s",fea_low[f],fea_name[f]);
	break;
      case VERY_HIGH:
	printf("  * %s %s",fea_high[f],fea_name[f]);
	break;
      }
      if (results[i].outlier[f]) {
	if (enhanced) 
	  switch (get_mike(results[i].name)) {
	  case PRIMARY:
	    printf("  (%.2f %s) mean val. = %.1f %s\n",results[i].feature[f],fea_unit[f],
		   p_stats[f].mean,fea_unit[f]);
	    break;
	  case SECONDARY:
	    printf("  (%.2f %s) mean val. = %.1f %s\n",results[i].feature[f],fea_unit[f],
		   s_stats[f].mean,fea_unit[f]);
	    break;
	  }
	else 	  
	  printf("  (%.2f %s) mean val. = %.1f %s\n",results[i].feature[f],fea_unit[f],
		 stats[f].mean,fea_unit[f]);
      }
    }
  }
  return noteworthy;
}

void print_statistics (void)
{
  int i;

  printf("This ensemble consisted of %d files.\n\n",file_count);
  printf("\n");
  printf("NB: 0 dB refers to a signal with constant magnitude = 1\n\n");

  if (enhanced) {
    printf("Primary microphone files:\n\n");
    printf("number of files = %d\n\n",p_stats[0].number);
    for (i=0; i<FEA_NUM; i++)
    printf("mean %s = %5.1f %s     standard dev. = %5.1f %s%s",
	   fea_name[i],p_stats[i].mean,fea_unit[i],p_stats[i].sd,fea_unit[i],fea_lf[i]);
    printf("\n\nSecondary microphone files:\n\n");
    printf("number of files = %d\n\n",s_stats[0].number);
    for (i=0; i<FEA_NUM; i++)
    printf("mean %s = %5.1f %s     standard dev. = %5.1f %s%s",
	   fea_name[i],s_stats[i].mean,fea_unit[i],s_stats[i].sd,fea_unit[i],fea_lf[i]);
  } else {
    for (i=0; i<FEA_NUM; i++) 
      printf("mean %s = %5.1f %s     standard dev. = %5.1f %s%s",
	   fea_name[i],stats[i].mean,fea_unit[i],stats[i].sd,fea_unit[i],fea_lf[i]);
  }
}


void print_site_statistics (void)
{
  int i,j,old_site=-1,site;
  Statistic *temp,*p_temp,*s_temp;
  int mike;

  if (enhanced) {
    p_temp = (Statistic *) malloc (FEA_NUM * sizeof(Statistic));
    s_temp = (Statistic *) malloc (FEA_NUM * sizeof(Statistic));
  } else 
    temp = (Statistic *) malloc (FEA_NUM * sizeof(Statistic));

  if (enhanced) {
    INITIALIZE_STAT(p_temp);
    INITIALIZE_STAT(s_temp);
  } else {
    INITIALIZE_STAT(temp);
  }
    
  
  for (i=0; i<file_count; i++) {
    if (((site = get_site(results[i].name)) != old_site) &&
	(old_site != -1)) {
      printf("\n\n----- Data collected from %s:\n\n",site_name[old_site]);
      if (enhanced) {
	printf("Primary microphone files:\n\n");
	printf("number of files = %d\n\n",p_temp[0].number);
	compute_statistics(p_temp);
	for (j=0; j<FEA_NUM; j++)  {
	  printf("mean %s = %5.1f %s     standard dev. = %5.1f %s%s",
		 fea_name[j],p_temp[j].mean,fea_unit[j],p_temp[j].sd,fea_unit[j],fea_lf[j]);
	}
	printf("\nSecondary microphone files:\n\n");
	printf("number of files = %d\n\n",s_temp[0].number);
	compute_statistics(s_temp);
	for (j=0; j<FEA_NUM; j++)  {
	  printf("mean %s = %5.1f %s     standard dev. = %5.1f %s%s",
		 fea_name[j],s_temp[j].mean,fea_unit[j],s_temp[j].sd,fea_unit[j],fea_lf[j]);
	}
	INITIALIZE_STAT(p_temp);  
	INITIALIZE_STAT(s_temp);
      } else {
	compute_statistics(temp);
	for (j=0; j<FEA_NUM; j++) {
	  printf("mean %s = %5.1f %s     standard dev. = %5.1f %s%s",
		 fea_name[j],temp[j].mean,fea_unit[j],temp[j].sd,fea_unit[j],fea_lf[j]);
	}
	INITIALIZE_STAT(temp);
      }
    }
    if (enhanced) {
      mike = get_mike (results[i].name);
      for (j=0; j<FEA_NUM; j++) {
	if (mike==PRIMARY) {
	  UPDATE_SUMS(p_temp[j],results[i].feature[j]);
	} else {
	  UPDATE_SUMS(s_temp[j],results[i].feature[j]);
	}
      }
    } else {
      for (j=0; j<FEA_NUM; j++) {
	UPDATE_SUMS(temp[j],results[i].feature[j]);
      }
    }
    old_site=site;
  }

  /* handle case of last site */
  printf("\n\n----- Data collected from %s:\n\n",site_name[site]);
  if (enhanced) {
    printf("Primary microphone files:\n\n");
    printf("number of files = %d\n\n",p_temp[0].number);
    compute_statistics(p_temp);
    for (j=0; j<FEA_NUM; j++)  {
      printf("mean %s = %5.1f %s     standard dev. = %5.1f %s%s",
	     fea_name[j],p_temp[j].mean,fea_unit[j],p_temp[j].sd,fea_unit[j],fea_lf[j]);
    }
    printf("\nSecondary microphone files:\n\n");
    printf("number of files = %d\n\n",s_temp[0].number);
    compute_statistics(s_temp);
    for (j=0; j<FEA_NUM; j++)  {
      printf("mean %s = %5.1f %s     standard dev. = %5.1f %s%s",
	     fea_name[j],s_temp[j].mean,fea_unit[j],s_temp[j].sd,fea_unit[j],fea_lf[j]);
    }
    INITIALIZE_STAT(p_temp);  
    INITIALIZE_STAT(s_temp);
  } else {
    compute_statistics(temp);
    for (j=0; j<FEA_NUM; j++) {
      printf("mean %s = %5.1f %s     standard dev. = %5.1f %s%s",
	     fea_name[j],temp[j].mean,fea_unit[j],temp[j].sd,fea_unit[j],fea_lf[j]);
    }
    INITIALIZE_STAT(temp);
  }

  if (enhanced) {
    free(s_temp);
    free(p_temp);
  } else
    free(temp);
}
      

void compute_statistics (Statistic *stat)
{
  int i;
  double second_moment;
  double diff;

  /* uses trick that variance = E(x**2) - E(x)**2 */
  
  for (i=0; i<FEA_NUM; i++) {
    stat[i].mean = (stat[i].number==0) ? 0.0 : stat[i].sum / ((double) stat[i].number);
    second_moment = (stat[i].number==0) ? 0.0 : stat[i].sum_sqr / ((double) stat[i].number);
    diff = second_moment - stat[i].mean*stat[i].mean;
    diff = (diff < 0.0) ? 0.0 : diff;
    /* sometimes diff will be a very small negative number close to 0 */
    /* because of roundoff error. The above line is a hack around that. */
    stat[i].sd = (double) sqrt(diff);
  }
}
  

void compile_statistics (char *name, double signal, double noise, double snr, int sample_count, int speech_frame_count, double begin_silence, double end_silence)
{
  double speech_ratio,seconds;

  seconds = samples_to_seconds(sample_count);
  speech_ratio=((double) speech_frame_count / seconds);  /* percentage 0--100 */
  /* N.B. 1 frame == .01 sec */

  strcpy(results[file_count].name,name);

  results[file_count].feature[0] = signal;  /* speech power */                   
  results[file_count].feature[1] = noise;   /* noise power */                    
  results[file_count].feature[2] = snr;     /* snr */                            
  results[file_count].feature[3] = seconds;       /* duration */                       
  results[file_count].feature[4] = speech_ratio;  /* speech percentage */ 
  results[file_count].feature[5] = begin_silence;  /* heading silence */ 
  results[file_count].feature[6] = end_silence;  /* trailing silence */ 

  update(stats,signal,noise,snr,seconds,speech_ratio,begin_silence,end_silence);

  file_count++;
}


void update(Statistic *s, double signal, double noise, double snr, double seconds, double speech_ratio, double begin_silence, double end_silence)
{
  UPDATE_SUMS(s[0],signal);                                          
  UPDATE_SUMS(s[1],noise);                                           
  UPDATE_SUMS(s[2],snr);                                             
  UPDATE_SUMS(s[3],seconds);                                               
  UPDATE_SUMS(s[4],speech_ratio);
  UPDATE_SUMS(s[5],begin_silence);
  UPDATE_SUMS(s[6],end_silence);
}


void initialize_statistics(int how_many)
{
  int i;

  stats = (Statistic *) malloc (FEA_NUM * sizeof (Statistic));
  p_stats = (Statistic *) malloc (FEA_NUM * sizeof (Statistic));
  s_stats = (Statistic *) malloc (FEA_NUM * sizeof (Statistic));
  INITIALIZE_STAT(stats);
  INITIALIZE_STAT(p_stats);
  INITIALIZE_STAT(s_stats);

  results = (Datum *) malloc (how_many * sizeof (Datum));
  for (i=0; i<how_many; i++) {
    results[i].feature = (double *) malloc (FEA_NUM * sizeof(double));
    results[i].outlier = (int *) malloc (FEA_NUM * sizeof (int));
  }

}

int str_comp(const void *s1_in, const void *s2_in)
{
    char **s1=(char **)s1_in;
    char **s2=(char **)s2_in;
    return strcmp(*s1,*s2);
}



int initialize_file_grabber(void)
{
  char temp[L_tmpnam];
  char name[50];
  char command_line[100];
  char full_name[80];
  int i,site,count=0;
  int old_site;

  filename = (char **) malloc (MAX_FILES * sizeof (char *));
  for (i=0; i<MAX_FILES; i++) filename[i] = (char *) malloc (80*sizeof(char));

  old_site=-1;

  if (name_specified && !root) {
    /* read in the list of file names from a file */
    if ((group = fopen(group_name,"r")) == NULL) {
      printf("I can't find %s.\n",group_name);
      exit(-1);
    }
  } else {
    /* read in the directory */
    tmpnam(temp);

    if (primary)
      sprintf(command_line,"ls%s%s | grep s.wav > %s",root ? " -R " : "",
	      (root && name_specified) ? group_name : "",temp);
    else if (secondary)
      sprintf(command_line,"ls%s%s| grep c.wav > %s",root ? " -R " : "",
	      (root && name_specified) ? group_name : "",temp);
    else
      sprintf(command_line,"ls%s%s| grep .wav > %s",root ? " -R " : "",
	      (root && name_specified) ? group_name : "",temp);

    system(command_line);

    if ((group = fopen(temp,"r")) == NULL) {
      printf("atismd: error opening temp file.\n");
      exit(-1);
    }
  }

  /* store the filenames */
  while (fscanf(group,"%s",name) != EOF) {
    if ((site = get_site(name)) != old_site) {
      old_site = site;
      site_count++;
    }
    
    if (root) {
      if (!name_specified)
	sprintf(group_name,"%s",getenv("PWD"));
      make_full_name (name,full_name,group_name);
    }
    sprintf(filename[count++],"%s",(root) ? full_name : name);
  }
    
  
  if (name_specified && root) {
    sprintf(command_line,"rm %s",temp);
    system(command_line);
  }

  fclose(group);
  current_file = 0;

  /* sort the file names */
  qsort(filename,count,sizeof(char *),str_comp);

  return count;
}    

void make_full_name (char *name, char *full_name, char *root_dir)
{
  char session;
  char speaker[3];

  session = get_session(name);
  get_speaker(name,speaker);

  sprintf(full_name,"%s/%s/%c/%s",root_dir,speaker,session,name);
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

int speech_frames (int **list, int segments)
{
  int sum=0;

  while (segments--) 
    sum += (list[segments][TO]-list[segments][FROM]);

  return sum;
}
 


void investigate_outlier(int index)
{
  char command_line[160];
  char output_line[100],primary_name[80];
  int outlier=0;
  FILE *pipe;
  SP_FILE *junk;

  outlier = ((OUTLIER(index,0)==VERY_HIGH) ||
	     (OUTLIER(index,1)==VERY_HIGH) ||
	     (OUTLIER(index,2)==VERY_LOW));

  if (!outlier) return;


  /* check the header */

  /* if the noise or speech powers are high, or SNR is low */
  if ((OUTLIER(index,0)==VERY_HIGH) || (OUTLIER(index,1)==VERY_HIGH) ||
      (OUTLIER(index,2)==VERY_LOW)) {
    sprintf(command_line,"headcheck %s",results[index].name);
    printf("\n  running diagnostic to check header information:");
    printf("\n  +---------------------------------------------\n");
    RUN_PROCESS(command_line,pipe,output_line);
  }

  /* check for ground hum */

  /* if the noise is high or the SNR is low */
  if ((OUTLIER(index,0)==VERY_HIGH) || (OUTLIER (index,2)==VERY_LOW)) {
    if (enhanced && (get_mike(results[index].name)==SECONDARY)) {
	junk = get_primary(results[index].name,primary_name);
      if (junk != SPNULL) {
	sprintf(command_line,"speech %s | autohum %s -s",primary_name,results[index].name);
	sp_close(junk);
      }
    } else
      	sprintf(command_line,"autohum %s",results[index].name);
    printf("\n  running diagnostic to check for 60-Hz related line components:");
    printf("\n  +------------------------------------------------------------\n");
    RUN_PROCESS(command_line,pipe,output_line);
  }

  /* check for clipping */

  /* if the speech is hot*/
  if (OUTLIER(index,1)==VERY_HIGH) {
    sprintf(command_line,"clp %s",results[index].name);
    printf("\n  running diagnostic to check for clipping:");
    printf("\n  +---------------------------------------\n");
    RUN_PROCESS(command_line,pipe,output_line);
  }

  printf("\n\n");
}

double samples_to_seconds(int samples)
{
  return ((double) samples) / 16000.0;
}

char *strip_pathname(char *name)
{
  int end;

  end = strlen(name);
  return &(name[end-12]);
}

int get_site(char *name)
/* returns -1 if site is unknown */

           

{
  int i,site=SITE_UNKNOWN,pos;
  char *stripped_name;

  pos = strlen(name);

  if (name[pos-6]=='v') return SITE_ATT;

  stripped_name = strip_pathname(name);
  
  for (i=0; i< SITE_NUM; i++)
    if ((strncmp(stripped_name,site_low[i],2) >= 0) &&
	(strncmp(stripped_name,site_high[i],2) <= 0)) {
      site = i;
      break;
    }

  return site;
}

int get_mike(char *name)
/* returns the microphone type of the filename */

           

{
  int pos;

  pos = strlen(name);

  if (!strcmp(&name[pos-5],"s.wav")) return PRIMARY;
  if (!strcmp(&name[pos-5],"c.wav")) return SECONDARY;
  return MIKE_UNKNOWN;
}


void get_speaker(char *name, char *speaker)
{
  char *stripped_name;

  stripped_name = strip_pathname(name);

  speaker[0]=stripped_name[0];
  speaker[1]=stripped_name[1];
  speaker[2]='\0';
}

char get_session(char *name)
{
  char *stripped_name;

  stripped_name = strip_pathname(name);
  return (stripped_name[5]);
}

SP_FILE *get_primary(char *name, char *outname)
{
  char p_name[80];
  char *pos;

  strcpy(p_name,name);
  pos = strstr(p_name,"c.wav");
  strcpy(pos,"s.wav");

  strcpy(outname,p_name);

  return sp_open(p_name,"r");
}
  
  

void print_help_and_exit (void)
{
  printf("\n");
  printf("Usage: atismd [-ehpqrs] [input file] [-n [report name]]\n\n");
  printf("  -e : Perform an \"enhanced\" measurement consisting of both primary\n");
  printf("       and secondary microphone recordings, and using segmentation\n");
  printf("       data from the primary mike to enhance the secondary mike\n");
  printf("       SNR estimation.\n");
  printf("  -h : prints this message.\n");
  printf("  -n : Specify the report name on the command line. If a\n");
  printf("       name is not specified in this manner, the user will\n");
  printf("       be prompted to enter a title during execution.\n");
  printf("  -p : primary microphone recordings only.\n");
  printf("  -q : quick option. Skip the follow-up tests on outliers.\n");
  printf("  -r : move recursively through ATIS file structure.\n");
  printf("  -s : secondary microphone recordings only.\n");
  printf("\n");
  printf("atismd (the ATIS Doctor) checks an ensemble of .wav files, tallies\n");
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
  printf("of file names that should be checked. Otherwise, by default, atismd\n");
  printf("checks every file with a .wav extension in the current directory.\n");
  printf("\n");
  printf("The -r flag puts the program in recursive mode. This means that the\n");
  printf("program searches recursively through all of the directories below\n");
  printf("the root directory to find .wav files to be included in the report.\n");
  printf("For this option to work properly, the subdirectories must follow the\n");
  printf("ATIS convention, i.e., they must be of the form:\n");
  printf("       root_dir/speaker_code/session_number/,\n");
  printf("where root_dir may be specified by the user as [input-file] (otherwise\n");
  printf("it is taken to be the current directory).\n");
  printf("\n");
  printf("Primary recordings are defined to be those that end in \"s.wav\",\n");
  printf("that is to say, using Sennheiser micrphones. Secondary recordings\n");
  printf("defined to be those that end in \"c.wav\", i.e., those made with\n");
  printf("Crown microphones.\n");
  printf("\n");
  printf("The -e option specifies that both primary and secondary microphones\n");
  printf("should be considered. In this mode, the SNR estimations for the \n");
  printf("secondary microphones are enhanced by the segmentation information\n");
  printf("from the corresponding primary recording (if available). If a follow-\n");
  printf("up investigation of ground-loop contamination is called for, then\n");
  printf("this measurement is similarly enhanced.\n");
  printf("See the documentation for <segsnr> and <autohum> for details of\n");
  printf("these \"enhancements\".\n");
  exit(-1);
}
