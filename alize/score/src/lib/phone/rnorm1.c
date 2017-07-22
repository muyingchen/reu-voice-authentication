/* file rnorm1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


  /******************************************************************/
  /*                                                                */
  /* double rnormal(m,sd)                                           */
  /*                                                                */
  /* Returns a random normal deviate with mean <m> and standard     */
  /* deviation <sd>.  Uses polar method (Knuth 2, p. 103).          */
  /*                                                                */
  /******************************************************************/

 double rnormal(double m, double sd)
{char *proc = "rnormal";
  double u1,u2,v1,v2,s,f,x1,x2,y1;
/* code: */
  db_enter_msg(proc,0); /* debug only */
  do
    {
     u1 = rand() / (double)RAND_MAX;
     u2 = rand() / (double)RAND_MAX;
/*     u1 = random() / (double)RAND_MAX; */ /* better BSD generator */
/*     u2 = random() / (double)RAND_MAX; */ /* better BSD generator */
     v1 = 2.0*u1 - 1.0;
     v2 = 2.0*u2 - 1.0;
     s = (v1*v1) + (v2*v2);
    } while (s >= 1.0);
  f = sqrt( (-2.0*log(s))/s );
  x1 = v1*f;
  x2 = v2*f;
  y1 = m+(sd*x1);
if (db_level > 2) printf("*DB: x1=%f, x2=%f, y1=%f\n",x1,x2,y1);
  db_leave_msg(proc,0); /* debug only */
  return y1;
 } /* end rnormal */
