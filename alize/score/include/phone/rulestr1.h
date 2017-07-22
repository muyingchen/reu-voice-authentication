 /* File rulestr1.h --  rules data structures #1 */

#if !defined(RULESTR1_HEADER)
#define RULESTR1_HEADER

  typedef struct
   {char *sin; int sinl;   /* string in, length */
    char *sout; int soutl; /* string out, lenght */
   } RULE1;

 typedef struct
   {char *name;
    char *directory;
    char *desc;
    char *format;
    boolean copy_no_hit; /* if no hit, copy over instead of delete */
    int nrules;
    int max_nrules;
    int rtab_length;
    RULE1 *rule; /* RULE table, dynamically allocated */
   } RULESET1;

/* end of include rulestr1.h */

#endif
