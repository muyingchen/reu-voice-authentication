/* 

    file: pmmlcg.h
    Desc: Include file for the random number generator

*/
#include <math.h>
#if defined(__STDC__) || defined(__GNUC__) || defined(sgi)
#define PROTO(ARGS)	ARGS
#else
#define PROTO(ARGS)	()
#endif

/* pmmlcg.c */  void init_seed PROTO((int new_seed, int verbose)) ;
/* pmmlcg.c */  float pmmlcg PROTO((void)) ;
/* pmmlcg.c */  int pmmlcg_newseed PROTO((void)) ;
/* pmmlcg.c */  int pmmlcg_seed PROTO((void)) ;
/* pmmlcg.c */  float exp_dist PROTO((float beta)) ;
/* pmmlcg.c */  float M_erlang PROTO((float beta, int M)) ;
/* pmmlcg.c */  int arbitrary_discrete_rv PROTO((float *prob_list, int num_prob)) ;
