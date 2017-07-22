/* alstr4.h */
#ifndef ALSTR4_HEADER
#define ALSTR4_HEADER

 typedef struct al1 {  /* a basic alignment object using integer strings */
    boolean aligned; /* are the symbols aligned, or not?  */
    int d; /* alignment distance, if they are aligned */
    WCHAR_T *aint; /* pointer to the "a" integer string */
    WCHAR_T *bint; /* pointer to the "b" integer string */
/* NB: with int strings, s[0] holds the number of elements in the string */
    pcodeset *pcset; /* the pcode defining the symbols */
/* for future context-sensitive work: */
    int ia;  /* ia and jb point out a particular symbol-symbol match */
    int jb;  /* ia indexes into aint[], jb into bint[].  */
/* definition of auxiliary symbols, composed of several pcset symbols: */
/* (auxtab[i] defines aux symbol i as an int string of pcset codes) */
    int naux;
    WCHAR_T *auxtab[MAX_AUX_SYMBOLS];
   }    ALIGNMENT_INT;

#endif
/* end include alstr4.h */
