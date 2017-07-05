/* file genmacs.h         */
/* General-purpose macros */
#ifndef GENMACS_HEADER
#define GENMACS_HEADER

#ifndef fabs
#define fabs(_v) ((_v >= 0.0) ? (_v) : (_v)*(-1.000000)) /* if not in sys */
#endif

#define max(_a,_b) (((_a) > (_b)) ? (_a) : (_b))
#define min(_a,_b) (((_a) < (_b)) ? (_a) : (_b))

#endif
