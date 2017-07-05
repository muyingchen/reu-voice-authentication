/********************************************************\
* 							 *
*  -*-p Mode: C; Package: (c_yaft C); Tab-width: 4; -*-	 *
* 							 *
\********************************************************/

/********************************************************/
/*    TITLE:          MULTI-INDEXED SORT                */
/*    SYSTEM:         HEADERS/MACROS                    */
/*    SUBSYSTEM:                                        */
/*    SOURCE FILE:    INDEXSTRUCT.H                     */
/*    AUTHOR:         Michael Garris and Ted Zwiesler   */
/*                                                      */
/*    DATE CREATED:   12/16/86                          */
/*    LAST MODIFIED:                                    */
/********************************************************/

/********************************************************/
/* defined variables                                    */
/********************************************************/
#define STACKSIZE    1000
#define AMBIGUOUS    -1
#define GOOD         1
#define INTTYPE  0
#define PTRTYPE  1
#define INC  1
#define DEC  2
/********************************************************/
/* structures and type def's                            */
/********************************************************/
typedef union {
   int i_item; /* int type */
   char *p_item; /* (char *) type */
} ITEMVAL;


struct index_struct {
    int cursize;  /* next location available in all arrays */
    int indexnum; /* number of index arrays in structure */
    int maxsize;  /* max size of all arrays */
    int **index;  /* pointer to an array of pointers to index arrays */
    int itemtype; /* type of item array */
    ITEMVAL *item;/* pointer to an item array */
} ;

typedef struct index_struct *INDEX;
/********************************************************/
/* with INDEX macros:                                   */
/*   _name => name of pointer to index_struct           */
/*       s => max size of all arrays in _name           */
/*   itype => type of item array (int, char *)          */
/*                                                      */
/*   ** use the macro corresponding to the number of    */
/*      index arrays in the structure _name.            */
/********************************************************/
#define with_5_INDEX(_name,s,itype) \
{ \
    struct index_struct __r_a__; \
	struct index_struct *_name = &__r_a__; \
	int *_index_[5]; \
        int __index0__[s]; \
        int __index1__[s]; \
        int __index2__[s]; \
        int __index3__[s]; \
        int __index4__[s]; \
	ITEMVAL _item_[s]; \
        _index_[0] = __index0__; \
        _index_[1] = __index1__; \
        _index_[2] = __index2__; \
        _index_[3] = __index3__; \
        _index_[4] = __index4__; \
	_name->item = _item_; \
	_name->index = _index_; \
	_name->cursize = 0; \
        _name->indexnum = 5; \
        _name->maxsize = s; \
        _name->itemtype = itype; \
	{

#define with_4_INDEX(_name,s,itype) \
{ \
    struct index_struct __r_a__; \
	struct index_struct *_name = &__r_a__; \
	int *_index_[4]; \
        int __index0__[s]; \
        int __index1__[s]; \
        int __index2__[s]; \
        int __index3__[s]; \
	ITEMVAL _item_[s]; \
        _index_[0] = __index0__; \
        _index_[1] = __index1__; \
        _index_[2] = __index2__; \
        _index_[3] = __index3__; \
	_name->item = _item_; \
	_name->index = _index_; \
	_name->cursize = 0; \
        _name->indexnum = 4; \
        _name->maxsize = s; \
        _name->itemtype = itype; \
	{


#define with_3_INDEX(_name,s,itype) \
{ \
    struct index_struct __r_a__; \
	struct index_struct *_name = &__r_a__; \
	int *_index_[3]; \
        int __index0__[s]; \
        int __index1__[s]; \
        int __index2__[s]; \
	ITEMVAL _item_[s]; \
        _index_[0] = __index0__; \
        _index_[1] = __index1__; \
        _index_[2] = __index2__; \
	_name->item = _item_; \
	_name->index = _index_; \
	_name->cursize = 0; \
        _name->indexnum = 3; \
        _name->maxsize = s; \
        _name->itemtype = itype; \
	{


#define with_2_INDEX(_name,s,itype) \
{ \
    struct index_struct __r_a__; \
	struct index_struct *_name = &__r_a__; \
	int *_index_[2]; \
        int __index0__[s]; \
        int __index1__[s]; \
	ITEMVAL _item_[s]; \
        _index_[0] = __index0__; \
        _index_[1] = __index1__; \
	_name->item = _item_; \
	_name->index = _index_; \
	_name->cursize = 0; \
        _name->indexnum = 2; \
        _name->maxsize = s; \
        _name->itemtype = itype; \
	{


#define with_1_INDEX(_name,s,itype) \
{ \
    struct index_struct __r_a__; \
	struct index_struct *_name = &__r_a__; \
	int *_index_[1]; \
        int __index0__[s]; \
        ITEMVAL _item_[s]; \
        _index_[0] = __index0__; \
	_name->item = _item_; \
	_name->index = _index_; \
	_name->cursize = 0; \
        _name->indexnum = 1; \
        _name->maxsize = s; \
        _name->itemtype = itype; \
	{

#define end_with_INDEX } }
/********************************************************/
/* add to "number" INDEX macros:                        */
/*    __istruct => pointer to index_struct              */
/*         _itm => value to be added to item array      */
/*        _ind? => value to be added to index array '?' */
/*                                                      */
/*   ** use the macro corresponding to the number of    */
/*      index arrays in the structure __istruct.        */
/********************************************************/
#define add_to_1_INDEX(__istruct, _itm, _ind0) \
{   int _indlist[1]; \
    _indlist[0] = _ind0; \
    add_to_INDEX(__istruct, _itm, _indlist) \
}

#define add_to_2_INDEX(__istruct, _itm, _ind0, _ind1) \
{   int _indlist[2]; \
     _indlist[0] = _ind0; \
     _indlist[1] = _ind1; \
     add_to_INDEX(__istruct, _itm, _indlist) \
}

#define add_to_3_INDEX(__istruct, _itm, _ind0, _ind1, _ind2) \
{   int _indlist[3]; \
    _indlist[0] = _ind0; \
    _indlist[1] = _ind1; \
    _indlist[2] = _ind2; \
    add_to_INDEX(__istruct, _itm, _indlist) \
}

#define add_to_4_INDEX(__istruct, _itm, _ind0, _ind1, _ind2, _ind3) \
{   int _indlist[4]; \
    _indlist[0] = _ind0; \
    _indlist[1] = _ind1; \
    _indlist[2] = _ind2; \
    _indlist[3] = _ind3; \
    add_to_INDEX(__istruct, _itm, _indlist) \
}

#define add_to_5_INDEX(__istruct, _itm, _ind0, _ind1, _ind2, _ind3, _ind4) \
{   int _indlist[5]; \
    _indlist[0] = _ind0; \
    _indlist[1] = _ind1; \
    _indlist[2] = _ind2; \
    _indlist[3] = _ind3; \
    _indlist[4] = _ind4; \
    add_to_INDEX(__istruct, _itm, _indlist) \
}

/********************************************************/
/* add to INDEX macro:                                  */
/********************************************************/
#define add_to_INDEX(__r,_itm,_indlist) \
{ \
	int _add_to_INDEX_i = __r->cursize; \
        int _j_; \
        int *indexarray; \
        if(_add_to_INDEX_i >= __r->maxsize){ \
           printf("ARRAY OVERFLOW IN INDEX STRUCTURE\n"); \
           exit(1); \
	} \
        for(_j_ = 0; _j_ < __r->indexnum; _j_++){ \
            indexarray = __r->index[_j_]; \
            indexarray[_add_to_INDEX_i] = _indlist[_j_]; \
	} \
        if(__r->itemtype == INTTYPE) \
           __r->item[_add_to_INDEX_i].i_item = (int)_itm; \
        else \
           __r->item[_add_to_INDEX_i].p_item = (char *)_itm; \
	__r->cursize += 1; \
}
/********************************************************/
/* multisort "number" INDEX macros:                     */
/*           _index => pointer to an index_struct       */
/*           order? => sorting order for index array '?'*/
/*                                                      */
/*   ** use the macro corresponding to the number of    */
/*      index arrays in the structure __istruct.        */
/*   ** order? may be:                                  */
/*            INC => sort on index array '?' increasing */
/*            DEC => sort on index array '?' decreasing */
/*              0 => no sort on index array '?'         */
/********************************************************/
#define multisort_1_INDEX(_index,order0) \
           multisort_INDEX(_index,order0,0,0,0,0)

#define multisort_2_INDEX(_index,order0,order1) \
           multisort_INDEX(_index,order0,order1,0,0,0)

#define multisort_3_INDEX(_index,order0,order1,order2) \
           multisort_INDEX(_index,order0,order1,order2,0,0)

#define multisort_4_INDEX(_index,order0,order1,order2,order3) \
           multisort_INDEX(_index,order0,order1,order2,order3,0)

#define multisort_5_INDEX(_index,order0,order1,order2,order3,order4) \
           multisort_INDEX(_index,order0,order1,order2,order3,order4)
/********************************************************/
/* ith elements in arrays                               */
/********************************************************/
#define ith_item_int(_name,i) (_name->item[i].i_item)
#define ith_item_ptr(_name,i) (_name->item[i].p_item)
#define ith_index(_name,j,i) (_name->((index[j])[i]))
/********************************************************/
/* for loop indexing from 0 to cursize in _indx         */
/********************************************************/
#define foreach_in_INDEX(_indx) \
{ int _i_; \
  struct index_struct *_indx_ = _indx; \
    for(_i_=0; _i_<_indx_->cursize; _i_++) {

#define this_item_int (_indx_->item[_i_].i_item)
#define this_item_ptr (_indx_->item[_i_].p_item)
#define this_index(j) ((_indx_->index[j])[_i_])

#define endfor  } }
/********************************************************/
/* general purpose swap:                                */
/********************************************************/
#define swap(datatype,a,b) \
		{ \
			datatype _tmp_ = a; \
			a = b; \
			b = _tmp_; \
		}
/********************************************************/
/* sort by item macros:                                 */ 
/********************************************************/
#define sortINDEX_increasing_by_item(_indx) \
		{ \
		swap(ITEMVAL *,_indx->item,(ITEMVAL *)_indx->index[0]); \
		multisort_1_INDEX(_indx, INC); \
		swap(ITEMVAL *,_indx->item,(ITEMVAL *)_indx->index[0]); \
		}

#define sortINDEX_decreasing_by_item(_indx) \
		{ \
		swap(ITEMVAL *,_indx->item,(ITEMVAL *)_indx->index[0]); \
		multisort_1_INDEX(_indx, DEC); \
		swap(ITEMVAL *,_indx->item,(ITEMVAL *)_indx->index[0]); \
		}


/***********************************************************************/
/*   Macros to use in the scoring package                              */
/***********************************************************************/
#define dump_INDEX(_name) \
{  int _i,_s; \
   printf("dump of the index structure \n"); \
   printf("cursize %d \n",_name->cursize); \
   printf("indexnum %d\n",_name->indexnum); \
   printf("max size %d\n",_name->maxsize); \
   printf("item type %d\n",_name->itemtype); \
   for (_s=0; _s < _name->cursize; _s++) { \
       for (_i=0; _i < _name->indexnum; _i++) \
           printf("%-6d ",(_name->index[_i])[_s]); \
       if (_name->itemtype == INTTYPE) \
           printf("%-6d\n",_name->item[_s].i_item); \
       else \
           printf("   %c\n",_name->item[_s].i_item); \
   } \
}

#define with_2_INDEX_sort(_name,s,itype,ind_l0,ind_l1,o0,o1,_item_) \
{ \
    struct index_struct __r_a__; \
	struct index_struct *_name = &__r_a__; \
	int *_index_[2]; \
        _index_[0] = ind_l0; \
        _index_[1] = ind_l1; \
	_name->item = _item_; \
	_name->index = _index_; \
	_name->cursize = s; \
        _name->indexnum = 2; \
        _name->maxsize = s; \
        _name->itemtype = itype; \
        multisort_2_INDEX(_name,o0,o1); \
}

