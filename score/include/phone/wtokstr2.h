/* file wtokstr2.h                              */
/* data structure #2 for a list of word tokens, */
/* as in time-mark  (*.marked, *.mrk) files.    */
/* parameterized by MAX_WORDS_IN_FILE           */
 typedef struct
   {char *turn;
    char chan;
    double t1;  /* beginning time of token      */
    double t2;  /* ending time of token         */
    double dur; /* duration of token = t1-t2    */
    char *sp;   /* spelling of token            */
    float confidence; /* confidence factor of token, OPTIONAL */
    boolean overlapped;
    boolean mispronounced;
    boolean unsure;
    boolean comment;
    boolean bad_marking;
    boolean crosstalk;
   } WTOKE2;

 typedef struct
   {int n;                           /* number of word tokens in table */
    int s;                           /* the current beginning of the table 
					during processing */
    char *id;                       /* the utterance id of the segment/file */
    WTOKE2 word[MAX_WORDS_IN_FILE]; /* table of word tokens           */
   } WTOKE_STR2;





/* Bill, These should be somewhere else */
void get_next_mark_utt (FILE *fp, WTOKE_STR1 *word_tokens, char *fname, boolean *end_of_file, int *perr);
void fill_mark_struct (FILE *fp, WTOKE_STR1 *word_tokens, char *fname, boolean *end_of_file, int *perr);


