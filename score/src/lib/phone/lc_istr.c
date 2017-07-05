/* file lc_istr.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


/************************************************************/
/*  int *lc_istr(sx,pcode,ichr,auxtab,naux)                 */
/* Puts the lower code integer string for code <ichr> of    */
/* pcodeset *pcode into sx, returning sx.  If ichr is       */
/* outside the bounds of *pcode, looks in pal->auxtab.      */
/************************************************************/
int *lc_istr(int *sx, int ichr, pcodeset *pcode, ALIGNMENT_INT *pal)
{char *proc = "lc_istr";
  int iaux,j,kx;
  WCHAR_T *px;
/* code */
  sx[0] = 0;
  if (ichr > 0)
    {if (ichr <= pcode->pctab_length)
       sx = nstrcpy(sx,pcode->pc[ichr].lc_int);
     else
       {iaux = ichr - pcode->pctab_length;
        if (iaux > pal->naux)
          {fprintf(stderr,"*ERR:%s: iaux=%d,naux=%d\n",
             proc,iaux,pal->naux); 
           goto RETURN;
          }
        px = pal->auxtab[iaux];
        for (j=1; j <= px[0]; j++) 
          {kx = px[j];
           sx = nstrcat(sx,pcode->pc[kx].lc_int);
    }  }  }
RETURN:
  return sx;
 }
/* end function lc_istr() */
