/* file stdpars.h                            */
/* standard symbolic source-code parameters: */

#ifndef STDPARS_HEADER
#define STDPARS_HEADER

#ifndef MAX_NODES
#define MAX_NODES 300
#endif

#ifndef MAX_ARCS
#define MAX_ARCS 3000
#endif

#ifndef MAX_ALTS
#define MAX_ALTS  12
#endif

#ifndef LINE_LENGTH
#define LINE_LENGTH  132
#endif

#ifndef LONG_LINE
#define LONG_LINE   2000
#endif

#ifndef RAND_MAX
#define RAND_MAX (int)pow(2.0,31.0) - 1.0 /* max rand() value */
#endif

#ifndef MAX_REC_LENGTH
#define MAX_REC_LENGTH 600
#endif

#ifndef MAX_SYMBS_IN_STR
#define MAX_SYMBS_IN_STR 150
#endif

#define MAX_WORDS_IN_FILE 3200
#define MAX_STR_NCHARS 600
#define MAX_CODE_SETS 4
#define MAX_CODE_ELEMENTS 6000
#define MAX_CODE_VALUE    6000

#define MAX_PHONES 160
#define MAX_FHIST_SAVED_VALUES 2000

#define MAX_AUX_SYMBOLS 20

#define HFCN_NPARAMS_MAX 24

#define MAX_SMTAB_ENTRIES 10000

#endif
