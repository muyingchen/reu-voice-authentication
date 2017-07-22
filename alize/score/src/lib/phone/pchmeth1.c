/* file pchmeth1.c                         */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

/* Pcode hash method #1 functions:         */
/* pc_htab_store(), pcindex_htab_lookup()  */

int pc_htab_store(pcodeset *pc1, pcode_element *element, int *duplicate_key, int *table_full)
                                      
                                    
/*****************************************************************/
/* Stores *element in hash table pc1->pc[*].                     */
/* Returns the index of the slot that it puts the entry into,    */
/* also *duplicate_key=T or *table_full=T  to indicate error.    */
/* Handles collisions by linear searching down in table to find  */
/* next available slot.                                          */
/*****************************************************************/
/* Linear probing:  */
 {char *proc = "pc_htab_store";
  int i, count=0;
  *duplicate_key = F;
  *table_full = F;
/* code */
 db_enter_msg(proc,3); /* debug only */
  i = hash(element->ac,pc1->hash_pars,pc1->pctab_length);
/* find an empty location */
  while (strlen(pc1->pc[i].ac) > 0)     /* Location i is occupied?  */
    {if (streq(pc1->pc[i].ac,element->ac)) /* Duplicate key */
       {*duplicate_key = T;
        goto RETURN;
       }
     if (++count == pc1->pctab_length)   /* Table full */
         {*table_full = T;
          goto RETURN;
         }
     if (++i == pc1->pctab_length) i = 0; /* bump i mod table_length */
    }
/* put in key and entry */
  pc1->pc[i] = *element;
RETURN:
if (detail_level > 3) printf("  no. of collisions=%ld\n",count);
 db_leave_msg(proc,3); /* debug only */
  return i;
}


int pcindex_htab_lookup(char *symb, pcodeset *pc1, int *found)
                                          
/********************************************************/
/* Returns index of *symb in pc1->pc[*] hash table.     */
/* On return, found = T iff key was found in table.     */
/* Handles collisions by linear searching down in table */
/* to find a match (success), an empty slot(failure),   */
/* or looking thru all locations in table (failure).    */
/********************************************************/
 {char *proc = "pcindex_htab_lookup";
  int i, count=0;
/* code */
  db_enter_msg(proc,3); /* debug only */
  *found = F;
  if (strlen(symb) < 1) goto RET;
  if (pc1->pctab_length == 0) goto RET;
  i = hash(symb,pc1->hash_pars,pc1->pctab_length);
  while (!streq(pc1->pc[i].ac, symb)) /* Strings unequal?    */
    {
     if (strlen(pc1->pc[i].ac) < 1)    goto RET; /* Yes - empty location? */
     if (++count == pc1->pctab_length) goto RET; /* or table exhausted?   */
     i = ++i % pc1->pctab_length;
    }
  *found = T;          /* Found! */
RET:
 db_leave_msg(proc,3); /* debug only */
  return i;
}

/* end pchmeth1.c */
