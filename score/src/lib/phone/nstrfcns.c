/* file nstrfcns.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /***************************************/
 /* integer string functions            */
 /* source code:                        */
 /* nstrcpy(pn2,pn1)                    */
 /* nstrcpyn(pn2,pn1,n)                 */
 /* nstrcat(pn2,pn1)                    */
 /* nstrdup(pn)                         */
 /* nstrdup_safe(pn,proc)               */
 /* nstreq(pn1,pn2)                     */
 /* nstrcmp(pn1,pn2)                    */
 /***************************************/
 
 /*************************************************************/
 /*  nstrcpy(pn_to,pn_from)                                   */
 /*  Copies integer string *p_from to *p_to.                  */
 /*************************************************************/
 int *nstrcpy(int *pn_to, int *pn_from)
{/* char *proc = "nstrcpy"; */
  int *pni, *pnj; int i, n;
  n = *pn_from;
  pnj = pn_to;
  pni = pn_from;
  for (i=0; i <=n; i++) {*pnj = *pni; pni++; pnj++;}
  return pn_to;
 }
 
 /*************************************************************/
 /*  nstrcpyn(pn_to,pn_from,nmax)                             */
 /*  Copies integer string *p_from to *p_to, copying at most  */
 /* nmax characters.                                          */
 /*************************************************************/
 int *nstrcpyn(int *pn_to, int *pn_from, int nmax)
{/* char *proc = "nstrcpyn"; */
  int *pni, *pnj; int i, n;
  n = *pn_from;
  if ( n > nmax) n = nmax;
  pnj = pn_to + 1;
  pni = pn_from + 1;
  *pn_to = n;
  for (i=1; i <=n; i++) {*pnj = *pni; pni++; pnj++;}
  return pn_to;
 }
 
 /*************************************************************/
 /*  nstrcat(pn_to,pn_from)                                   */
 /*  Concatenates string *pn_from to end of string *pn_to.    */
 /*  Returns *pn_to.                                          */
 /*************************************************************/
 int *nstrcat(int *pn_to, int *pn_from)
{/* char *proc = "nstrcat"; */
  int *pni, *pnj; int i;
  pnj = pn_to+(pn_to[0]+1);
  pni = pn_from + 1;
  for (i=1; i <= pn_from[0]; i++) {*pnj = *pni; pni++; pnj++;}
  pn_to[0] += pn_from[0];
  return pn_to;
 }
 
 /*************************************************************/
 /*  nstrdup(pn)                                              */
 /*  Copies an integer string into a newly-created location.  */
 /*************************************************************/
 int *nstrdup(int *pn)
{char *proc = "nstrdup";
  int *pnx; int n;
  n = pn[0];
  pnx = (int *) malloc((unsigned)(sizeof(int)*(n + 1)));
  if (memory_trace) printf("*DB: MALLOC %x\n",(int)pnx);
  if (pnx == NULL) fprintf(stderr,"%s: memory ovf\n",proc);
  else   pnx = nstrcpy(pnx,pn);
  return pnx;
 }
 
 /*************************************************************/
 /*  nstrdup_safe(pn,proc)                                    */
 /*  Copies an integer string into a newly-created location.  */
 /*  Like nstrdup(ps) except fatal err if malloc fails.       */
 /*************************************************************/
 int *nstrdup_safe(int *pn, char *calling_proc)
{char *proc = "nstrdup_safe";
  int *pnx; int n;
  n = pn[0];
  pnx = (int *) malloc_safe(sizeof(int)*(n + 1),proc);
  if (memory_trace) printf("%s MALLOC %x\n",pdb,(int)pnx);
  pnx = nstrcpy(pnx,pn);
  return pnx;
 }
 
 /*************************************************************/
 /*  nstreq(pn1,pn2)                                          */
 /*  Tests equality of integer strings *pn1 and *pn2.         */
 /*************************************************************/
 boolean nstreq(int *pn1, int *pn2)
{/*char *proc = "nstreq";*/
  boolean eq;
  int i;
  eq = T;
  if (pn1[0] != pn2[0]) eq = F;
  for (i=1; (eq)&&(i <= pn1[0]); i++)
    if (pn1[i] != pn2[i]) eq = F;
  return eq;
 }

 /*************************************************************/
 /*  nstrcmp(pn1,pn2)                                         */
 /*  Compares integer string *pn1 to integer string *pn2.     */
 /*  returns:                                                 */
 /*    < 0 if *pn1 < *pn2                                     */
 /*   == 0 if *pn1 == *pn2                                    */
 /*    > 0 if *pn1 > *pn2                                     */
 /*************************************************************/
 int nstrcmp(int *pn1, int *pn2)
{/*char *proc = "nstrcmp";*/
  int i, ans;
  ans = 0;
  for (i=1; (ans == 0) && (i <= pn1[0]); i++)
    {if (i > pn2[0]) ans = 1; /* *pn2 is shorter */
     else
       {if (pn1[i] < pn2[i]) ans = -1;
        if (pn1[i] > pn2[i]) ans = 1;
       }
    }
  if ((ans == 0) && (pn1[0] < pn2[0])) ans = -1; /* *pn1 shorter */
  return ans;
 }

/* end include nstrfcns.c */
