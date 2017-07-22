/**********************************************************************/
/*                                                                    */
/*             FILENAME:  db_st_list.c                                */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file has programs to search for the data     */
/*                  base sentence lists                               */
/*           CHANGES: 05-14-90 Fixed the bug in the is_in_DB_ST_LIST  */
/*                  that didn't allow to one case.                    */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/**********************************************************************/
/*    return true if the sentence of "type and num" is in the         */
/*    data base sentence list                                         */
/*  FIX:  high should be initialized to the number-1 of things to     */
/*        search Oct 23, 1990                                         */
/**********************************************************************/
int is_in_DB_ST_LIST(DB_ST_LIST *dbl, int type, int num)
{
    int low, high, mid;

    low = 0, high = db_st_count(dbl)-1;

    do { 
        mid = (low + high)/2;
        if ((type == db_st_ty(dbl,mid)) && (num == db_st_num(dbl,mid)))
            return(TRUE);

        if ((type < db_st_ty(dbl,mid)) ||
      	    ((type == db_st_ty(dbl,mid)) && (num < db_st_num(dbl,mid))))
            high = mid-1;
        else
            low = mid+1;
    } while (low <= high);
    return(FALSE);
}


/*************************************************************************/
/*   This procedure comb through the SYS_ALIGN structure and finds all   */
/*   the unique sentences that were recognized                           */
/*************************************************************************/
void make_DB_ST_LIST_fr_SYS_ALIGN(SYS_ALIGN *align_str, DB_ST_LIST **db_st_list)
{
    DB_ST_LIST *tdb;
    ITEMVAL *objs;

    alloc_DB_ST_LIST(tdb);
    db_st_count(tdb)=0;
#ifdef mmm
    for (spkr=0;spkr < num_spkr(align_str); spkr++){
        for (snt=0;snt < num_sents(align_str,spkr); snt++){
            for (stnum=0;stnum < db_st_count(tdb);stnum++){
                if ((db_st_ty(tdb,stnum) == sent_type(align_str,spkr,snt))&&
                    (db_st_num(tdb,stnum) == sent_num(align_str,spkr,snt)))
                    break;
	    }
            if (stnum == db_st_count(tdb))
                if (db_st_count(tdb) < MAX_DB_STS){
                    db_st_ty(tdb,stnum) = sent_type(align_str,spkr,snt);
                    db_st_num(tdb,stnum) = sent_num(align_str,spkr,snt);
                    db_st_count(tdb)++;
	        }
                else{
                    fprintf(stderr,"Too many sents to store in the Database");
                    fprintf(stderr,"sentence list %d = %d\n",db_st_count(tdb),
                                   MAX_DB_STS);
                    exit(-1);
		}
	}
    }
#endif
    /* sort the list of sentences */
    alloc_singarr(objs, db_st_count(tdb), ITEMVAL);
    with_2_INDEX_sort(ind_str, db_st_count(tdb), INTTYPE, 
                     db_st_ty_l(tdb), db_st_num_l(tdb),
                     INC, INC,
                     objs);

    *db_st_list = tdb;
}

