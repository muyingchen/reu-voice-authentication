/*******************************************/
/* file stdcenvf.h                         */
/* standard compile environment for fcns   */
/*******************************************/

#define COMPILE_ENVIRONMENT 1


#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>

/* standard program parameter settings */
#include <phone/stdpars.h>

/* type definitions */
#include <phone/booldef.h>
#include <phone/wchardef.h>
#include <phone/num_rel.h>
#include <phone/interv1.h>
#include <phone/pcinddef.h>
#include <phone/hparstr1.h>
#include <phone/rulestr1.h>
#include <phone/pcstr3.h>
#include <phone/ppfstr2.h>
#include <phone/alstr4.h>
#include <phone/alstr5.h>
#include <phone/netstr1.h>
#include <phone/pctabstr.h>
#include <phone/smstr2.h>
#include <phone/talstr1.h>
#include <phone/ssstr1.h>
#include <phone/wtokstr1.h>

/* macros */
#include <phone/genmacs.h>
#include <phone/strmacs.h>

/* global data declarations (NOT definitions) */
#include <phone/gdatadec.h>

/* function declarations */
#include <phone/fcndcls.h>

/* prototypes of system functions */
#ifdef AT_NIST
#include "/usr/local/nist/general/include/util/proto.h"
#endif

