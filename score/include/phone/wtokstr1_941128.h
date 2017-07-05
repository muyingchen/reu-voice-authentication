/* file wtokstr1.h                           */
/* data structure for a list of word tokens, */
/* as in time-mark  (*.marked, *.mrk) files. */
/* parameterized by MAX_WORDS_IN_FILE        */
 typedef struct
   {char *turn;
    double t1;  /* beginning time of token */
    double dur; /* duration of token       */
    char *sp;   /* spelling of token       */
    boolean overlapped;
    boolean mispronounced;
    boolean unsure;
    boolean comment;
    boolean bad_marking;
    boolean crosstalk;
   } WTOKE1;

 typedef struct
   {int n;                           /* number of word tokens in table */
    WTOKE1 word[MAX_WORDS_IN_FILE]; /* table of word tokens           */
   } WTOKE_STR1;

