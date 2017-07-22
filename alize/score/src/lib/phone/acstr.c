/* file acstr.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif


/**********************************************************/
/*  char *ascii_str(sx,ichr,pcode,pal)                    */
/* Puts the ASCII string for code <ichr> of pcodeset      */
/* *pcode into sx, returning sx.  If ichr is outside the  */
/* bounds of *pcode, looks in pal->auxtab.                */
/**********************************************************/
char *ascii_str(char *sx, PCIND_T ichr, pcodeset *pcode, ALIGNMENT_INT *pal)
{char *proc = "ascii_astr";
  int iaux,j,kx;
  WCHAR_T *px;
/* code */
  if (ichr <= pcode->pctab_length)
    sx = strcpy(sx,pcode->pc[ichr].ac);
  else
    {iaux = ichr - pcode->pctab_length;
     sx = strcpy(sx,"");
     if (iaux > pal->naux)
       {fprintf(stderr,"*ERR:%s: iaux=%d,naux=%d\n",
          proc,iaux,pal->naux); 
        goto RETURN;
       }
     px = pal->auxtab[iaux];
     for (j=1; j <= px[0]; j++) 
       {kx = px[j];
        if (j > 1) sx = strcat(sx," ");
        sx = strcat(sx,pcode->pc[kx].ac);
    }  }
RETURN:
  return sx;
 }
/* end function ascii_astr() */
