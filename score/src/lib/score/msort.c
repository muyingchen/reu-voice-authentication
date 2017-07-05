/********************************************************/
/*    TITLE:          MULTI-INDEXED SORT                */
/*    SYSTEM:         SOURCE MODULE                     */
/*    SUBSYSTEM:                                        */
/*    SOURCE FILE:    MULTISORT.C                       */
/*    AUTHOR:         Michael Garris and Ted Zwiesler   */
/*                                                      */
/*    DATE CREATED:   12/16/86                          */
/*    LAST MODIFIED:                                    */
/********************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

#define  FULL -1
#define  DONE -1

/********************************************************/
/* globals:                                             */
/********************************************************/
int stack[STACKSIZE];
int *stkptr = stack;
int ambiflag = GOOD;

/********************************************************/
/* multisort_INDEX:                                     */
/*    Main module for executing multi-indexed sort.     */
/*    It inputs a pointer to an index_struct along      */
/*    with an order list which specifies the order      */
/*    each index array in the structure is to be        */
/*    sorted. This function returns an ambiflag         */
/*    which is set to AMBIGUOUS if the final index      */
/*    array sorted contains ambiguous entries. Other-   */
/*    wise the function returns GOOD.                   */
/********************************************************/
int multisort_INDEX(INDEX indexstruct, int o0, int o1, int o2, int o3, int o4)
{
   int left = 0;
   int right = indexstruct->cursize - 1;
   int curindex_i = 0;
   int nextgrp_i = 0;
   int orderlist[5];
   int grpstart, grpend;

   orderlist[0] = o0;
   orderlist[1] = o1;
   orderlist[2] = o2;
   orderlist[3] = o3;
   orderlist[4] = o4;
   curindex_i = getnextindex(indexstruct, orderlist, curindex_i);
   if(curindex_i == FULL){
      printf("No index array specified\n");
      exit(1);
   }
   if(orderlist[curindex_i] == INC){
      qsort_INDEX_inc(indexstruct, curindex_i, left, right);
   }
   else{
      qsort_INDEX_dec(indexstruct, curindex_i, left, right);
   }
   while((nextgrp_i = getnextgrp(indexstruct, curindex_i, nextgrp_i, right,
                                 &grpstart, &grpend)) != DONE){
      if(procgrp(grpstart, grpend, indexstruct, curindex_i + 1, orderlist)
         == AMBIGUOUS)
         ambiflag = AMBIGUOUS;
   }
   return(ambiflag);
}

/********************************************************/
/* getnextindex:                                        */
/*    This function inputs a pointer to an index_       */
/*    struct, an order list, and the current index      */
/*    array being processed. The function, looking      */
/*    at the order list, determines the next index      */
/*    array of highest precedence to be sorted and      */
/*    returns its subscript.                            */
/********************************************************/
int getnextindex(INDEX indexstruct, int *orderlist, int curindex_i)
{
   while(curindex_i < indexstruct->indexnum){
      if(orderlist[curindex_i++] != 0)
         return(--curindex_i);
   }
   return(FULL);
}

/********************************************************/
/* getnextgrp:                                          */
/*    This function inputs:                             */
/*       indexstruct => pointer to an index_struct      */
/*        curindex_i => subscript to current index      */
/*                      array                           */
/*              left => where to begin search in array  */
/*             right => where to end search in array    */
/*           grpsptr => subscript of where group starts */
/*           grpeptr => subscript of where group ends   */
/*                                                      */
/*    It begins a search in the current index array     */
/*    for groupings of like elements from position      */
/*    left in the array to right. This function re-     */
/*    turns the left and right subscripts of the        */
/*    first group found, and also returns the position  */
/*    in the current index array where the search       */
/*    should resume the next time getnextgrp is called. */
/*    If the process does not find any groups, then it  */
/*    returns DONE.                                     */
/********************************************************/
int getnextgrp(INDEX indexstruct, int curindex_i, int left, int right, int *grpsptr, int *grpeptr)
{
   int curgrp_i = left;
   int curval = (indexstruct->index[curindex_i])[curgrp_i];

   while(curgrp_i <= right){
      ++curgrp_i;
      if((curval == (indexstruct->index[curindex_i])[curgrp_i])
        &&(curgrp_i <= right)){
         *grpsptr = curgrp_i - 1;
         ++curgrp_i;
         while((curval == (indexstruct->index[curindex_i])[curgrp_i])
              &&(curgrp_i <= right))
            ++curgrp_i;
         *grpeptr = curgrp_i - 1;
         return(curgrp_i);
      }
      else 
         curval = (indexstruct->index[curindex_i])[curgrp_i];
   }
   return(DONE);
}

/********************************************************/
/* procgrp:                                             */
/*    This function inputs:                             */
/*       listleft => left position in current index     */
/*                   array where the process is to start*/
/*      listright => right position in current index    */
/*                   array where the process is to end  */
/*    indexstruct => pointer to an index_struct         */
/*     curindex_i => subscript of current index array   */
/*      orderlist => list specifying how each index     */
/*                   array is to be sorted              */
/*                                                      */
/*    This recursive function walks through the current */
/*    index array from listleft to listright sorting    */
/*    the elements and then searching and processing    */
/*    recursively any subgroups found. This function    */
/*    returns an ambiflag which is set to AMBIGUOUS if  */
/*    the final index array sorted contains ambiguous   */
/*    entries. Otherwise the function returns GOOD.     */
/********************************************************/
int procgrp(int listleft, int listright, INDEX indexstruct, int curindex_i, int *orderlist)
{
   int nextgrp_i = listleft;
   int subgrpstart, subgrpend;

   curindex_i = getnextindex(indexstruct, orderlist, curindex_i);
   if(curindex_i == FULL){
      return(AMBIGUOUS);
   }
   if(orderlist[curindex_i] == INC)
      qsort_INDEX_inc(indexstruct, curindex_i, listleft, listright);
   else
      qsort_INDEX_dec(indexstruct, curindex_i, listleft, listright);
   while((nextgrp_i = getnextgrp(indexstruct, curindex_i, nextgrp_i,
                      listright, &subgrpstart, &subgrpend)) != DONE){
      if(procgrp(subgrpstart, subgrpend, indexstruct, curindex_i + 1,
         orderlist) == AMBIGUOUS)
         ambiflag = AMBIGUOUS;
   }
   return(ambiflag);
}

/********************************************************/
/* qsort_INDEX_inc:                                     */
/*    This procedure inputs a pointer to an index_      */
/*    struct, the subscript of an index array to be     */
/*    sorted, a left subscript pointing to where the    */
/*    sort is to begin in the index array, and a right  */
/*    subscript where to end. This module invokes an    */
/*    increasing quick-sort sorting the index array     */
/*    from l to r.                                      */
/********************************************************/
void qsort_INDEX_inc(INDEX indexstruct, int curindex_i, int l, int r)
{
   int left = l;
   int right = r;
   int pivot;
   int llen, rlen;
   int lleft, lright, rleft, rright;

   pushstack(left);
   pushstack(right);
   while(stkptr != stack){
      right = popstack();
      left = popstack();
      if((right - left + 1) > 1){
         pivot = select_pivot(indexstruct->index[curindex_i],left, right);
         partition_inc(indexstruct, curindex_i, &llen, &rlen, &lleft,
                   &lright, &rleft, &rright,pivot, left, right);
         if(llen > rlen){
            pushstack(lleft);
            pushstack(lright);
            pushstack(rleft);
            pushstack(rright);
	 }
         else{
            pushstack(rleft);
            pushstack(rright);
            pushstack(lleft);
            pushstack(lright);
	 }
      }
   }
}

/********************************************************/
/* partition_inc:                                       */
/*    Inputs a pivot element making comparisons and     */
/*    swaps with other elements in a list until pivot   */
/*    resides at its correct position in the list.      */
/********************************************************/
void partition_inc(INDEX indexstruct, int curindex_i, int *llen, int *rlen, int *ll, int *lr, int *rl, int *rr, int p, int l, int r)
{
   int i;
   int flag = TRUE;

   *ll = l;
   *rr = r;
   while (flag){
      if (l < p){
         if ((indexstruct->index[curindex_i])[l] >
                         (indexstruct->index[curindex_i])[p]){
            for(i = 0; i < indexstruct->indexnum; i++)
               swap (int, (indexstruct->index[i])[l],
                          (indexstruct->index[i])[p])

            if((indexstruct->itemtype) == INTTYPE)
               swap (int, indexstruct->item[l].i_item,
                          indexstruct->item[p].i_item)
            else
               swap (char *, indexstruct->item[l].p_item,
                             indexstruct->item[p].p_item)
 
            p = l;
	 }
         else
            l++;
      }
      else{
         if (r > p){
            if ((indexstruct->index[curindex_i])[r] <
                            (indexstruct->index[curindex_i])[p]){
               for(i = 0; i < indexstruct->indexnum; i++)
                  swap (int, (indexstruct->index[i])[r],
                             (indexstruct->index[i])[p])
               if((indexstruct->itemtype) == INTTYPE)
                  swap (int, indexstruct->item[r].i_item,
                             indexstruct->item[p].i_item)
               else
                  swap (char *, indexstruct->item[r].p_item,
                                indexstruct->item[p].p_item)
               p = r;
               l++;
	    }
            else
               r--;
	 }
         else{
            flag = FALSE;
            *lr = p - 1;
            *rl = p + 1;
            *llen = *lr - *ll + 1;
            *rlen = *rr - *rl + 1;
	 }
      }
   }
}

/********************************************************/
/* qsort_INDEX_dec:                                     */
/*    This procedure inputs a pointer to an index_      */
/*    struct, the subscript of an index array to be     */
/*    sorted, a left subscript pointing to where the    */
/*    sort is to begin in the index array, and a right  */
/*    subscript where to end. This module invokes a     */
/*    decreasing quick-sort sorting the index array     */
/*    from l to r.                                      */
/********************************************************/
void qsort_INDEX_dec(INDEX indexstruct, int curindex_i, int l, int r)
{
   int left = l;
   int right = r;
   int pivot;
   int llen, rlen;
   int lleft, lright, rleft, rright;

   pushstack(left);
   pushstack(right);
   while(stkptr != stack){
      right = popstack();
      left = popstack();
      if((right - left + 1) > 1){
         pivot = select_pivot(indexstruct->index[curindex_i],left, right);
         partition_dec(indexstruct, curindex_i, &llen, &rlen, &lleft,
                   &lright, &rleft, &rright,pivot, left, right);
         if(llen > rlen){
            pushstack(lleft);
            pushstack(lright);
            pushstack(rleft);
            pushstack(rright);
	 }
         else{
            pushstack(rleft);
            pushstack(rright);
            pushstack(lleft);
            pushstack(lright);
	 }
      }
   }
}

/********************************************************/
/* partition_dec:                                       */
/*    Inputs a pivot element making comparisons and     */
/*    swaps with other elements in a list until pivot   */
/*    resides at its correct position in the list.      */
/********************************************************/
void partition_dec(INDEX indexstruct, int curindex_i, int *llen, int *rlen, int *ll, int *lr, int *rl, int *rr, int p, int l, int r)
{
   int i;
   int flag = TRUE;

   *ll = l;
   *rr = r;
   while (flag){
      if (l < p){
         if ((indexstruct->index[curindex_i])[l] <
                         (indexstruct->index[curindex_i])[p]){
            for(i = 0; i < indexstruct->indexnum; i++)
               swap (int, (indexstruct->index[i])[l],
                          (indexstruct->index[i])[p])

            if((indexstruct->itemtype) == INTTYPE)
               swap (int, indexstruct->item[l].i_item,
                          indexstruct->item[p].i_item)
            else
               swap (char *, indexstruct->item[l].p_item,
                             indexstruct->item[p].p_item)
 
            p = l;
	 }
         else
            l++;
      }
      else{
         if (r > p){
            if ((indexstruct->index[curindex_i])[r] >
                            (indexstruct->index[curindex_i])[p]){
               for(i = 0; i < indexstruct->indexnum; i++)
                  swap (int, (indexstruct->index[i])[r],
                             (indexstruct->index[i])[p])
               if((indexstruct->itemtype) == INTTYPE)
                  swap (int, indexstruct->item[r].i_item,
                             indexstruct->item[p].i_item)
               else
                  swap (char *, indexstruct->item[r].p_item,
                                indexstruct->item[p].p_item)
               p = r;
               l++;
	    }
            else
               r--;
	 }
         else{
            flag = FALSE;
            *lr = p - 1;
            *rl = p + 1;
            *llen = *lr - *ll + 1;
            *rlen = *rr - *rl + 1;
	 }
      }
   }
}


/********************************************************/
/* select_pivot:                                        */
/*    This module selects a pivot from a list being     */
/*    sorted using the Singleton Method.                */
/********************************************************/
int select_pivot(int *rnk, int l, int r)
{
   int m = (l + r) / 2;

   if (rnk[l] <= rnk[m])
      if (rnk[m] <= rnk[r])
         return(m);
      else
         if (rnk[r] > rnk[l])
            return(r);
         else
            return(l);
   else
      if (rnk[l] < rnk[r])
         return(l);
      else
         if (rnk[r] < rnk[m])
            return(m);
         else
            return(r);
}

/********************************************************/
/* popstack:                                            */
/*    This function decrements the pointer to a         */
/*    globally defined stack array and returns the      */
/*    last element on the stack.                        */
/********************************************************/
int popstack(void)
{
   if(--stkptr < stack){
      printf("stack underflow error\n");
      exit(1);
   }
   else{
      return(*stkptr);
   }
}

/********************************************************/
/* pushstack:                                           */
/*    This procedure adds an inputted value to a        */
/*    globally defined stack array and increments       */
/*    its pointer to point to the next available        */
/*    position in the stack.                            */
/********************************************************/
void pushstack(int position)
{
   *stkptr++ = position;
   if(stkptr > (stack + STACKSIZE)){
      printf("stack overflow error\n");
      exit(1);
   }
}
