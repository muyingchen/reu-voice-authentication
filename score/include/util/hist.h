/************************************************************************/
/*    file: hist.h							*/
/*    Desc: The structure definition for the histogram structure	*/
/*    Date: Nov 27, 1990						*/
/*    History:                                                          */
/*        Sept 7, 1994  Corrected a bug in 'body_insert_hist()' that    */
/*                      made the statements into a block.               */
/*									*/
/************************************************************************/
#define HIST_H_VERSION "V1.0"

#define PEAK_WIDTH 3
#define PEAK 0
#define TROUGH 1

typedef struct hist{
       int count;
       float from, to;
     } HIST;

#define head_insert_hist(_hist,_num_bins) { \
 register int _index; \
 register int _out_of_range=0; \
 register float _dist; \
 register float _from; \
   _from = _hist[0]->from; \
   _dist = _hist[_num_bins-1]->to - _hist[0]->from;

#define body_insert_hist(_hist,_num_bins,_value) { \
   _index = (int)((float)_num_bins * ((float)_value - (float)_from) / (float)_dist); \
   if ((_index>=0) && (_index<_num_bins)){ \
       _hist[_index]->count++; \
   } \
   else{ \
        _out_of_range++; \
   } \
} 

#define end_insert_hist(_hist,_num_bins) \
   if (_out_of_range > 0)   \
       printf("Hist Library: %d samples out of range (%4.2f,%4.2f)\n", \
                  _out_of_range,_from,_hist[_num_bins-1]->to); \
}

#if defined(__STDC__) || defined(__GNUC__) || defined(sgi)
#define PROTO(ARGS)	ARGS
#else
#define PROTO(ARGS)	()
#endif

/* hist.c */  void init_hist PROTO((HIST ***hist, int num_bins, float from, float to)) ;
/* hist.c */  void free_hist PROTO((HIST ***hist, int num_bins)) ;
/* hist.c */  void diff_hist PROTO((HIST **h1, HIST **h2, HIST **hd, int num_bins)) ;
/* hist.c */  void subtract_hist PROTO((HIST **h1, HIST **h2, HIST **hs, int num_bins)) ;
/* hist.c */  float percentile_hist PROTO((HIST **hist, int num_bins, float percentile)) ;
/* hist.c */  float percentage_left_hist PROTO((HIST **hist, int num_bins, float value)) ;
/* hist.c */  float do_least_squares PROTO((HIST **noise, HIST **normal, int num_bins)) ;
/* hist.c */  void hist_copy PROTO((HIST **from, HIST **to, int num_bins, int start, int end)) ;
/* hist.c */  void erase_hist PROTO((HIST **hist, int num_bins)) ;
/* hist.c */  void dump_hist PROTO((HIST **hist, int num_bins, FILE *fp)) ;
/* hist.c */  void dump_esps_hist PROTO((HIST **hist, int num_bins, char *fname)) ;
/* hist.c */  void read_esps_hist PROTO((HIST ***hist, int *num_bins, char *fname)) ;
/* hist.c */  void dump_gnuplot_hist PROTO((HIST **hist, int num_bins, char *fname)) ;
/* hist.c */  void dump_gnuplot_2hist PROTO((HIST **hist1, HIST **hist2, int num_bins, char *fname)) ;
/* hist.c */  void half_cosine_hist PROTO((HIST **hist, int num_bins, int begin_bin, int end_bin, int height)) ;
/* hist.c */  void full_cosine_hist PROTO((HIST **hist, int num_bins, int begin_bin, int end_bin, int height)) ;
/* hist.c */  int hist_area PROTO((HIST **hist, int num_bins)) ;
/* hist.c */  void hist_character PROTO((HIST **hist, int num_bins, float *mean, float *vari, int *area)) ;
/* hist.c */  int max_hist PROTO((HIST **hist, int num_bins)) ;
/* hist.c */  int average_hieght_hist PROTO((HIST **hist, int num_bins, int center, int window)) ;
/* hist.c */  void smooth_hist PROTO((HIST **from, HIST **to, int num_bins, int window)) ;
/* hist.c */  int comp PROTO((const void *a, const void *b)) ;
/* hist.c */  int locate_extremum PROTO((HIST **h, int from, int to, int type)) ;
/* hist.c */  void build_normal_hist PROTO((HIST **hist, int num_bins, float mean, float variance, float total_area)) ;
/* hist.c */  int hist_slope PROTO((HIST **hist, int num_bins, int center, int factor)) ;
/* hist.c */  void do_hist PROTO((HIST **hist, int num_bins, float *arr, int arr_cnt)) ;
/* hist.c */  void median_filter PROTO((HIST **h, HIST **out, int num_bins, int size)) ;



















