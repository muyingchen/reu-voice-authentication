/* file pcsort1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/***************************************************************/
/*  pcsort1(p1) pcodeset *p1;                                  */
/*  Sorts the elements of pcodeset *p1 into alphabetic order,  */
/*  using a quick-and-dirty algorithm.  This is the right sort */
/*  to use if the list is almost or completely sorted.         */
/***************************************************************/
void pcsort1(pcodeset *p1)
{int i,j;
 pcode_element temp_element;
 for (i = 1; i < p1->ncodes; i++)
   {for (j = i + 1; j <= p1->ncodes; j++)
     {if (str_less_than(p1->pc[j].ac,p1->pc[i].ac))
       {temp_element = p1->pc[i];
        p1->pc[i] = p1->pc[j];
        p1->pc[j] = temp_element;
   } } }
 return;
} /* end pcsort1(pcodeset) */

