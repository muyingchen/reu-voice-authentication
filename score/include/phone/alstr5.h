/* alstr5.h */
#ifndef ALSTR5_HEADER
#define ALSTR5_HEADER

 typedef struct al5 {  /* a basic alignment object using character strings */
    char *id; /* identification */
    boolean aligned; /* are the strings aligned, or not? */
    char *pastr;  /* "a" string */
    char *pbstr;  /* "b" string */
/* for future context-sensitive work: */
    char *pia;    /* pia and pjb point out a particular symbol-symbol match */
    char *pjb;    /* pia points into astr, pjb into bstr */
   }    ALIGNMENT_CHR;

#endif
/* end include alstr5.h */
