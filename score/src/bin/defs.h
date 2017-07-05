#include <score/setup.h>


#define MAX_FILENAME_LEN	80

#define DEFAULT_LEXICON		"drivers/rm/lexicon.snr"
#define DEFAULT_HOMO		"drivers/rm/homophn.txt"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h> 

#include <score/chars.h>
#include <score/version.h>
#include <score/sysalign.h>
#include <score/lex.h>
#include <score/gen.h>
#include <score/mem.h>

