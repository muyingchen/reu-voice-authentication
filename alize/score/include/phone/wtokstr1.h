/* file wtokstr1.h                           */
/* data structure for a list of word tokens, */
/* as in time-mark  (*.marked, *.mrk) files. */
/* parameterized by MAX_WORDS_IN_FILE        */
/* Modified 2/28/95 by Jon Fiscus & Bill     */
/* Fisher to include a few more fields.      */

 typedef struct
   {char *turn; /* the Channel id */
    char *conv; /* the Conversation/File ID */
    double t1;  /* beginning time of token */
    double dur; /* duration of token       */
    char *sp;   /* spelling of token       */
    float confidence; /* confidence factor of toke, OPTIONAL */
    boolean overlapped;
    boolean mispronounced;
    boolean unsure;
    boolean comment;
    boolean bad_marking;
    boolean crosstalk;
    boolean alternate;
   } WTOKE1;

 typedef struct
   {int n;                           /* number of word tokens in table */
    int s;                           /* the current beginning of the table 
					during processing */
    char *id;                       /* the utterance id of the segement/file */
    WTOKE1 word[MAX_WORDS_IN_FILE]; /* table of word tokens           */
   } WTOKE_STR1;
