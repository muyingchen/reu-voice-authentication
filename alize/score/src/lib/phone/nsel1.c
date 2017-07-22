/* file nsel1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/******************************************************************/
/*  fcn nselected(iselect,jselect,pcode)                          */
/*  computes number of symbols in *pcode that are selected by     */
/*  the column and row selection vectors *iselect and *jsselect.  */
/******************************************************************/
int nselected(int *iselect, int *jselect, pcodeset *pcode)
{int i,n;
  n=0;
  for (i=0; i<=pcode->ncodes; i++)
    {if (iselect[i]) n+=1;
     if (jselect[i]) n+=1;
    }
  return n;
 } /* end function nselected() */
/* end of file nsel1.c */
