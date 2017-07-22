/* file pctabstr.h       */
/* multi-code structure  */

#ifndef PCTABSTR_HEADER
#define PCTABSTR_HEADER

typedef struct {pcodeset *pc;
                char *pfname;
                } code_entry;
typedef struct {
	int ncodesets;
        char *directory;
	code_entry cd[MAX_CODE_SETS];
          } TABLE_OF_CODESETS;

#endif
/* end of file pctabstr.h */
