/**********************************************************************/
/*                                                                    */
/*             FILENAME:  db_sent_list.h                              */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  Declarations of a list of data base sentences     */
/**********************************************************************/

#define END_DB_ST_LIST		-1
typedef struct  db_sent_list_struct{
    int count;
    int *sentence_type;
    int *sentence_number;
} DB_ST_LIST;

#define MAX_DB_STS		1300

#define db_st_count(_db)	_db->count
#define db_st_ty_l(_db)		_db->sentence_type
#define db_st_ty(_db,_st)	_db->sentence_type[_st]
#define db_st_num_l(_db)	_db->sentence_number
#define db_st_num(_db,_st)	_db->sentence_number[_st]

#define alloc_DB_ST_LIST(_dbs) \
  { \
    alloc_singarr(_dbs,1,DB_ST_LIST); \
    alloc_int_singarr(db_st_ty_l(_dbs),MAX_DB_STS); \
    alloc_int_singarr(db_st_num_l(_dbs),MAX_DB_STS); \
  }

#define dump_DB_ST_LIST(_dbs) \
  { int _i; \
    printf("   Dump of the database sentence list\n\n"); \
    for (_i=0; _i< db_st_count(_dbs); _i++) \
        printf(" %5d  %4d\n",db_st_ty(_dbs,_i),db_st_num(_dbs,_i)); \
  }
