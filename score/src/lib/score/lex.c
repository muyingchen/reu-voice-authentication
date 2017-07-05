/**********************************************************************/
/*                                                                    */
/*             FILENAME:  lex.c                                       */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains utilities to load and          */
/*                  manipulate a lexicon file                         */
/*  Changes:                                                          */
/*       Aug. 1992.  Radical change, The package now uses Bill        */
/*       Fisher's Pcode Structure.  This enables uses of his phonetic */
/*       Alignment procedure.                                         */ 
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

void dump_LEXICON(TABLE_OF_CODESETS *pcs)
{;}

 /**********************************************************************/
 /*  Search the LEXICON structure, return the index into the LEXICON   */
 /**********************************************************************/
int is_LEXICON(TABLE_OF_CODESETS *pcs, char *str)
{
    PCIND_T foo;

    foo = pcindex5(adjust_type_case(str,pcs->cd[1].pc),pcs->cd[1].pc,1,1);
#if PCIND_SHORT == 1
    foo_s = foo;
    if (foo_s != foo){
	fprintf(stderr,"Error: Returned index for lexeme ");
	fprintf(stderr,"%s, %d is greater than %d\n",str,foo,0x7fff);
	exit(-1);
    }
#endif
     return(foo);
}


  /************************************************************/
  /*                                                          */
  /*  get_code_table_from_oldlex(ps1,fname,perr)              */
  /*                                                          */
  /*  Reads in a structure consisting of several pcodes,      */
  /* from an ASCII text file, and encode the lower codes.     */
  /*  Input: *fname contains the name of the file             */
  /*  Output: *ps1 is a table of pcodes                       */
  /*          *perr indicates success:                        */
  /*           0 iff a.o.k.                                   */
  /*           1 iff a pcode has in its defining file a       */
  /*             different lower pcode than in this table.    */
  /*           2 iff a pcode string had to be truncated.      */
  /*           4 a data (code) line was bungled.              */
  /*          14 iff pcode has more elements than the nmax    */
  /*                 that was declared in the table.          */
  /*          15 iff fatal error from get_pcode.              */
  /* (Uses global parameter LINE_LENGTH.)                     */
  /*                                                          */
  /************************************************************/
  void get_code_table_from_old_lex(TABLE_OF_CODESETS *ps1, char *fname, int *perr)
{
  /* data */
    char *proc = "get_code_table";
    char line[LINE_LENGTH], *pline = &line[0];

/* coding  */
  db_enter_msg(proc,0); /* debug only */
/* initialize  */
    *perr = 0;                   /* error return                        */
    ps1->ncodesets = 0;
    ps1->directory = EMPTY_STRING;


/* open pcode structure file */

/* This routine fools the other functions to work with by making a */
/* input line for gct_analyze_line() */

/* read and process lines */
    { char fake_line[1000];
      sprintf(fake_line,"%s word 5000 alphabetic",fname);

      if (db_level > 0) printf("*DB: fake input '%s'.\n",fake_line);
      gct_analyze_line2(fake_line,ps1,TRUE /*USE ADD FILE*/, perr);
      if (*perr > 0)
         {fprintf(stderr,"%s: bad line.\n",proc);
          fprintf(stderr,"%s: line='%s'\n",proc,pline);
         }
      if (*perr > 10) goto EXIT;
      if (db_level > 0) dump_code_table(ps1);
    }
/* and exit */
EXIT:
  db_leave_msg(proc,0); /* debug only */
   return;
  } /* end get_code_table */
