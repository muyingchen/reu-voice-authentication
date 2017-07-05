/* file hashfcn.c                                            */
/* contains latest, best versions of hash() and hash_init(). */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/* HASH7P.C                                            */
/* parameters: C[i], HTAB_LEN                          */
/* Simple algorithm form CACM 33,6, June 1990, p. 678  */
/* adapted to multiply key[i] by C[i] before summing.  */
/* puts name in global data "hfcn_name"                */
/* This package also includes function "hash_init",    */
/* which installs a good choice of C[i] and the name   */
/* of this hash function in global data areas.         */
/* Logic has been addded to insure that an empty       */
/* string or "*" hashes to 0 and no other strings      */
/* hash to  0, so that 0 can be used for insertions    */
/* and deletions.                                      */

int hash(char *key, HASH_PARAMETERS *hpars, int table_length)
{int i, usable_table_length;  int j;
/* code */
 i = 0;
 if (!streq(key,"*"))
   {usable_table_length = table_length - 1;
    for (j=0; key[j] != '\0'; j++)
      {i += hpars->C[j]*(int)key[j];
       i = (i % usable_table_length) + 1;
      }
    if (j >= hpars->nparams)
      {fprintf(stderr,"*ERR: hash fcn used too many parameters.\n");
       fprintf(stderr,"  nparams=%d, but used %d\n",hpars->nparams,j);
   }  }
 return i;
}

void hash_init(HASH_PARAMETERS *hpars)
{/* installs good parameter values and function name */
 hpars->hfcn_name = strdup("hash7p");
 hpars->recommended_load_factor = 0.50;
 hpars->C[0] = 20;
 hpars->C[1] = 12989;
 hpars->C[2] = 23218;
 hpars->C[3] = 13827;
 hpars->C[4] = 26496;
 hpars->C[5] = 13753;
 hpars->C[6] = 5630;
 hpars->C[7] = 26463;
 hpars->C[8] = 4268;
 hpars->C[9] = 1141;
 hpars->C[10] = 2058;
 hpars->C[11] = 17787;
 hpars->C[12] = 11160;
 hpars->C[13] = 24305;
 hpars->C[14] = 13014;
 hpars->C[15] = 29271;
 hpars->C[16] = 31044;
 hpars->C[17] = 1581;
 hpars->C[18] = 17250;
 hpars->C[19] = 5363;
 hpars->C[20] = 31408;
 hpars->C[21] = 20265;
 hpars->C[22] = 7086;
 hpars->C[23] = 3919;
 hpars->nparams = 24;
 return;
}
