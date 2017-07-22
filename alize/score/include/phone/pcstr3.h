 /* File pcstr3.h --  pcode data structures #3 */

 /*
         Structure for a gp-ized code book:


 Can hold either separated or non-separated (1 char) codes;
 non-separated multi-character codes will be added later.
    Each code consists of some data descriptive of the code
 as a whole, followed by a list of elements or entries in
 the code book.  Each code element consists conceptually
 of an external ASCII representation, an integer to serve as the
 internal representation of the code element, and an ordered
 list of code elements in the next lower code.  For example,
 a code element in a word code might contain these entries:
  
  integer rep   ASCII rep  <-- lower code rep -->
       1.        every     eh v r iy  
       2.        the       dh ax

  and some elements in a phonetic code might be:
  
       1.        /iy/      voc voiced high front 
       2.        /p/       cons stop unvoiced bilabial

     For now, the integer representation will just be the code
  element's index into this table.  The integer value 0 ("zero")
  is reserved to stand for the null code element. It is always
  initialized to "*", with no lower code representation. 
     Information on the code as a whole, with default values
  in brackets, is:

     name : the name of the code [NO DEFAULT].
     file_name : the name of the ascii file defining this code [NO DEFAULT].
     directory: the directory that the ascii file is found in.
     desc : a simple description of the code [EMPTY_STRING].
     format: the format of the ascii file "name" ["NIST2"].
     order: the order that the codes are; "freq", "alpha",
            "arbitrary", etc. ["arbitrary"].
     hashed: indicates if the pcode table is hashed [F].
     hash_pars: parameters for the hash function used.
     case: the case of the ASCII representation: "upper", "lower",
            "mixed", or "immaterial" ["mixed"].
     comment_flag: token beyond which material on a line is just comment.
     separated: whether or not codes in a string are separated [T].
     sep_codes: the ASCII character(s) that can separate codes [" "].
     compositional: whether or not each element is composed of a string
            of codes at the next lower level [F].
     element_type: such as "word" if code elements are words, etc. ["phone"]
     lower_code_fname: name of file defining lower code (used for
            compositional codes only) [EMPTY_STRING].
     lower_code: pointer to the next lower code [NULL].
     ppf_fname: name of file defining a symbol-to-symbol weight or
        distance function for the symbols in this file.  The name is
        derived from "phone-to-phone function"[EMPTY_STRING].
     ppf: pointer to a phone-to-phone weight or distance function structure[NULL].
     from_stn_rules_fname: name of file containing rules to transform
        the "standard" code into this code.
     from_stn_rules: pointer to ruleset transforming the "standard"
        code into this code (NULL if there are none).
     to_stn_rules_fname: name of file containing rules to transform
        this code into the "standard" code.
     to_stn_rules: pointer to ruleset transforming this code into
        the "standard" code (NULL if there are none).
     highest_code_value: highest numeric code in this code book [0].
        This element is not now being used.
     ncodes: number of code elements [0].
     max_ncodes: maximum number of code elements, used to determine
      the size of table to be used for PCODE_ELEMENTs [1000].  This
      value is an estimate from the header or elsewhere, and if a
      hash table is used, more elements than this will be made room
      for in the table.
     pctab_length: no. of pcode elements actually allocated space for.
 Note: EMPTY_STRING is a pointer to the null or empty string.

*/     

#ifndef PCSTR3_HEADER
#define PCSTR3_HEADER
 
 typedef struct
   {char *ac; /* external ASCII representation */
    char *lc_ext; /* string of external codes in the next lower code */
    int  *lc_int; /* str of int. codes, next lower code */
   } pcode_element;

 typedef struct pcs
   {char *name;
    char *file_name;
    char *directory;
    char *desc;
    char *format;
    char *order;
    boolean hashed;
    HASH_PARAMETERS *hash_pars;
    char *type_case;  /* upper, lower, mixed, immaterial */
    char *comment_flag;
    boolean separated;
    char *sep_codes;
    boolean compositional;
    char *element_type;
    char *lower_code_fname;
    struct pcs *lower_code;
    char *ppf_fname;
    struct ppfcn_table *ppf;
    char *from_stn_rules_fname;
    RULESET1 *from_stn_rules;
    char *to_stn_rules_fname;
    RULESET1 *to_stn_rules;
    PCIND_T highest_code_value;
    PCIND_T ncodes;
    PCIND_T max_ncodes;
    PCIND_T pctab_length;
    pcode_element *pc; /* pc table, dynamically allocated */
   } pcodeset;

#endif
/* end of include pcstr3.h */
