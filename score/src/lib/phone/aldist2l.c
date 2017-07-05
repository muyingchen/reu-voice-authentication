 /* file aldist2l.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*   int aldist2l(astr,bstr)                                          */
 /*   char *astr, *bstr;                                               */
 /*                                                                    */
 /*    Computes and returns the alignment distance between the         */
 /* two character strings *astr and *bstr.                             */
 /*    All initialization steps are included and hidden from the user, */
 /* in order to simplify the interface with a Prolog calling program.  */
 /* The name of the table of pcodesets to use is hard-coded into this  */
 /* function.                                                          */
 /*    Return of a negative number indicates an error.                 */
 /*                                                                    */
 /*                                                                    */
 /**********************************************************************/

  int aldist2l(char *astr, char *bstr)
{char *proc = "aldist2l";
/* data: */
   char *pfname = "pcdt_lib.txt";
   static TABLE_OF_CODESETS codesets, *pcs = &(codesets);
   static ALIGNMENT_INT *pn1;
   static pcodeset *pcode1;
   ALIGNMENT_INT *pn2;
   static ALIGNMENT_CHR *ps1;
   static boolean first_time = T;
   boolean return_alignment;
   int distance, err, *perr = &err;

/* code: */
db_enter_msg(proc,0); /* debug only */
if (db_level > 0)
   {printf("%sastr='%s'\n",pdb,astr);
    printf("%sbstr='%s'\n",pdb,bstr);
    printf("%sfirst_time=%s\n",pdb,bool_print(first_time));
   }

 /* if first time, initialize structures */
  if (first_time)
    {first_time = F;
     fprintf(stdout,"Please pardon me while I read in some code sets...\n");
     get_code_table(pcs, pfname, perr);
     if (*perr > 0)
        {fprintf(stdout,"*ERR:%s:get_code_table returned err code =%d\n",
           proc,*perr);
         distance = -1;
         goto RETURN;
        }
     pcode1 = pcs->cd[1].pc;
     pn1 = make_al_int(pcode1);
     ps1 = make_al_chr();
     ps1->aligned = F;
    }
/* quick check */
  if (streq(astr,bstr)) {distance = 0; goto RETURN;}    
/* fill alignment structure */
  ps1->pastr = astr;
  ps1->pbstr = bstr;
  adjust_type_case(ps1->pastr,pcode1);
  adjust_type_case(ps1->pbstr,pcode1);
  alignment_s_to_i(pn1,ps1,pcode1,perr);
  if (*perr > 0)
    {fprintf(stdout,"*ERR:%s: alignment_s_to_i returns perr=%d\n",
      proc,*perr);
     distance = -2;
     goto RETURN;
    }

if (db_level > 0)
 {printf("%s: before aldist2, *pn1 is:\n",pdb);
  dump_int_alignment(pn1);
 }

/* compute distance between strings by finding alignment */
  distance = aldist2(pn1,wod2,(return_alignment=F),pn2,perr);

RETURN:
if (db_level > 0) printf("%sleaving aldist2, distance=%d\n",pdb,distance);
db_leave_msg(proc,0); /* debug only */

  return distance;
 } /* end of function "aldist2l" */
