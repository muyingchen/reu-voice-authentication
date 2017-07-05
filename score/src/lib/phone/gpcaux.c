/* file gpcaux.c                                               */
/* Contains auxiliary sub-functions used by the higher level   */
/* functions get_pcode2() and get_pcode3():                    */
/*   process_pcode_aux_line()                                  */
/*   get_pcode_element()                                       */
/*   make_pc_table()                                           */
/*   init_pc_table()                                           */
/*   process_pcode_data_line()                                 */
/*   sort_pcodeset_a()                                         */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /************************************************************/
  /*   process_pcode_aux_line(p1,pline,perr)                  */
  /*                                                          */
  /*   error codes:  0: a.o.k.                                */
  /*                 3: bungled aux line                      */
  /*   (Fatal error if memory allocation fails.)              */
  /************************************************************/
  void process_pcode_aux_line(pcodeset *p1, char *pline, int *perr)
   {
    char *proc = "process_pcode_aux_line";
    char svx[LINE_LENGTH], *sval = &svx[0];
    char upx[LINE_LENGTH], *upr_pline = &upx[0];
    SUBSTRING sspx, *ssp = &sspx;
/* coding */
 db_enter_msg(proc,0); /* debug only */
    *perr = 0;
    upr_pline = make_upper(strcpy(upr_pline,pline));
/* first handle aux lines that don't need field values */
    if (strstr(upr_pline,"COMPOSITIONAL"))
      {if (strstr(upr_pline,"NOT"))
            p1->compositional = F;
       else p1->compositional = T;
       goto RETURN;
      }
    if ((strstr(upr_pline,"HASH FCN")) || (strstr(upr_pline,"HASH FUNCTION")))
      {p1->hashed = T;
       goto RETURN;
     }
    if (strstr(upr_pline,"SEPARATED"))
      {p1->separated = T;
       if (strstr(upr_pline,"NOT"))
         {p1->separated = F;
          goto RETURN;
      }  }
/* NOTE: if there is a field value, take care of it below */
/* get field value in quotes, if any, into *sval */
    *ssp = sstok2(pline,"\"'");
    if (substr_length(ssp) < 0)
      {fprintf(stderr,"%s: aux line contains no quoted field.\n",proc);
       fprintf(stderr," line:'%s'\n",pline);
       *perr=3;
      }
    else
      {sval = substr_to_str(ssp,sval,LINE_LENGTH);
/* identify field and store its value */
       if (strstr(upr_pline,"NAME"))
         {free_str(p1->name);
          p1->name = strdup_safe(sval,proc);
         } 
       if (strstr(upr_pline,"DESC"))
         {free_str(p1->desc);
          p1->desc = strdup_safe(sval,proc);
         }
       if (strstr(upr_pline,"FORMAT"))
         {free_str(p1->format);
          p1->format = strdup_safe(sval,proc);
	 }
       if (strstr(upr_pline,"ORDER"))
         {free_str(p1->order);
          p1->order = strdup_safe(sval,proc);
	 }
       if (strstr(upr_pline,"MAX_NCODES")) p1->max_ncodes = atol(sval);
       if (strstr(upr_pline,"TYPE_CASE"))
         {free_str(p1->type_case);
          p1->type_case = strdup_safe(sval,proc);
	 }
       if (strstr(upr_pline,"ELEMENT TYPE"))
         {free_str(p1->element_type);
          p1->element_type = strdup_safe(sval,proc);
	 }
       if (strstr(upr_pline,"SEPARATED"))
         {if (*sval == 'F')
             p1->separated = F;
          else
            {p1->separated = T;
             free_str(p1->sep_codes);
             p1->sep_codes = strdup_safe(sval,proc);
         }  }
       if (strstr(upr_pline,"LOWER CODE"))
         {free_str(p1->lower_code_fname);
          p1->lower_code_fname = strdup_safe(sval,proc);
/* (actual pointer to lower code, p1->lower_code, must be resolved later.)*/
	 }
       if (strstr(upr_pline,"PPF"))
         {free_str(p1->ppf_fname);
          p1->ppf_fname = strdup_safe(sval,proc);
/* (actual pointer to ppf fcn, p1->ppf, must be resolved later.) */
	 }
if (db_level > 0) printf("%schecking for RULES keyword\n",pdb);
       if (strstr(upr_pline,"RULES") != NULL)
         {
if (db_level > 0) printf("%sRULES aux line being processed.\n",pdb);
          if (strstr(upr_pline,"STANDARD") != NULL)
            {
if (db_level > 0) printf("%sRULES STANDARD aux line being processed.\n",pdb);
             if (strstr(upr_pline,"FROM") != NULL)
               {free_str(p1->from_stn_rules_fname);
                p1->from_stn_rules_fname = strdup_safe(sval,proc);
/* (actual pointer to p1->from_stn_rules, must be resolved later */
	       }
             if (strstr(upr_pline,"TO") != NULL)
               {free_str(p1->from_stn_rules_fname);
                p1->to_stn_rules_fname = strdup_safe(sval,proc);
/* (actual pointer to p1->to_stn_rules, must be resolved later */
      }  }  }  }
RETURN:
 db_leave_msg(proc,0); /* debug only */
  } /* end process_pcode_aux_line */


  /************************************************************/
  /*   get_pcode_element(pline,compositional,px,perr)         */
  /*   gets the pcode element *px from the line *pline.       */
  /*   error codes:                                           */
  /*       0: a.o.k.                                          */
  /*       3: bungled data line                               */
  /*   (Fatal error if memory allocation fails.)              */
  /************************************************************/
  void get_pcode_element(char *pline, int compositional, pcode_element *px, int *perr)
   {
    char *proc = "get_pcode_element";
    char sx_data[LINE_LENGTH], *sx = &sx_data[0];
    char *p_toke1_end;
    SUBSTRING ssline_data, *ss_line = &ssline_data;
    SUBSTRING ss1, *ss_ac = &ss1;
    SUBSTRING ss2, *ss_lc = &ss2;
  /* initialize */
 db_enter_msg(proc,3); /* debug only */
    *perr = 0;
    px->ac = EMPTY_STRING; /* each element must have an ac */
    px->lc_ext = NULL;     /* but, at least initially, may not have an lc */
    px->lc_int = NULL;
    ss_line->start = pltrimf(pline);
    ss_line->end   = prtrim(pline);
  /* get the external ASCII code - the first token */
    *ss_ac = sstok(ss_line," "); 
    if (ss_ac->start <= ss_line->end)
  /* (if it's bounded like /aa/, [aa], "it"  delete the boundaries.)  */
      {p_toke1_end = ss_ac->end;
       if (((*ss_ac->start == '/')&&(*ss_ac->end == '/'))
        || ((*ss_ac->start == '[')&&(*ss_ac->end == ']'))
        || ((*ss_ac->start == '"')&&(*ss_ac->end == '"')))
         {ss_ac->start +=1;
          ss_ac->end   -=1;
      }  }
    else
      {fprintf(stderr,"%s code line bungled\n",proc);
       fprintf(stderr,"line='%s'\n",pline);
       *perr=3; return;
      }
    px->ac = strdup_safe(substr_to_str(ss_ac,sx,LINE_LENGTH),proc);
    if (compositional)
      {ss_lc->start = pltrimf(p_toke1_end + 1);
       ss_lc->end   = ss_line->end;
  /* (if it's bounded like /b aa/, [b aa]  delete the boundaries.)  */
       if (((*ss_lc->start == '/')&&(*ss_lc->end == '/'))
        || ((*ss_lc->start == '[')&&(*ss_lc->end == ']')))
         {ss_lc->start +=1;
          ss_lc->end   -=1;
	 }
       px->lc_ext = strdup_safe(substr_to_str(ss_lc,sx,LINE_LENGTH),proc);
      }
 db_leave_msg(proc,3); /* debug only */
    return;
  } /* end get_pcode_element */


  /************************************************************/
  /*   make_pc_table(p1)                                      */
  /* Dynamically allocates a table for pc code elements.      */
  /* Returns p1->pc = NULL if table can't be made.            */
  /************************************************************/
  void make_pc_table(pcodeset *p1)
   {char *proc = "make_pc_table";
    int nelements;
    double load_factor;
  /* code */
 db_enter_msg(proc,0); /* debug only */
    if (!p1->hashed) load_factor = 1.0;
    else load_factor = p1->hash_pars->recommended_load_factor;
    nelements = p1->max_ncodes / load_factor;
/* NOTE: calloc inits to all zero bytes, so strlen(*)=0 */
    p1->pc = (pcode_element*)
      calloc_safe((size_t)nelements,sizeof(pcode_element),proc);
    if (p1->pc == NULL) p1->pctab_length = 0;
    else                p1->pctab_length = nelements;
 db_leave_msg(proc,0); /* debug only */
    return;
  } /* end make_pc_table */

  /************************************************************/
  /*   init_pc_table(p1)                                      */
  /* Initializes p1->pc[*]                                    */
  /************************************************************/
  void init_pc_table(pcodeset *p1)
   {char *proc = "init_pc_table";
    int i;
  /* code */
 db_enter_msg(proc,0); /* debug only */
    for (i = 0; i < p1->pctab_length; i++) p1->pc[i].ac = EMPTY_STRING;
 db_leave_msg(proc,0); /* debug only */
    return;
  } /* end init_pc_table */

  /************************************************************/
  /*   process_pcode_data_line(p1,pline,perr)                 */
  /*    error codes:   1: no valid information in pline       */
  /*                  11: memory ovf                          */
  /*                  12: bungled external code               */
  /*                  13: too many phones                     */
  /*   (Fatal error if memory allocation fails.)              */
  /************************************************************/
  void process_pcode_data_line(pcodeset *p1, char *pline, int *perr)
   {char *proc = "process_pcode_data_line";
    char *prest, *toke;
    int i;
    pcode_element x;
    pcode_element *px = &x;
 db_enter_msg(proc,3); /* debug only */
    *perr = 0;
  /* check - blank line? */
    if (textlen(pline) < 1)
      {fprintf(stderr,"%s invalid input:blank line.\n",proc);
       *perr=1;
       return;};
  /* check for which format  */
  /* TI1: 1 code/line, may specify numeric code, no composition; */
  /* NIST1: multi codes/line, no numeric codes, no composition;  */
  /* NIST2: 1 code/line, no numeric codes, composition;          */
  /* 1 pcode per line formats: */
    if ((streq(p1->format,"TI1"))||(streq(p1->format,"NIST2")))
       {get_pcode_element(pline,p1->compositional,px,perr);
        if (*perr == 0) i = add_pcode_element(p1,px,perr);
       }
  /* multi pcode per line format: */
    if (streq(p1->format,"NIST1")) /* multiple pcodes per line */
      {prest=pline;
       px->lc_ext=NULL;
       px->lc_int=NULL;
       while ( (*perr==0) && (toke=strtok(prest," \n")) != NULL )
         {px->ac = strdup_safe(toke,proc);
          if (streq(px->ac,"\"space\"")) px->ac=" "; /*special case */
	  i = add_pcode_element(p1,px,perr);
          prest=NULL; /* requirement of strtok */
      }  }
    if (*perr > 0)
      {fprintf(stderr,"Err exit from %s\n",proc);
       fprintf(stderr,"no. of code elements added so far=%d\n",
            p1->ncodes);
       fprintf(stderr,"Get_pcode or add_pcode failed\n");
      }
 db_leave_msg(proc,3); /* debug only */
    return;
  } /* end process_pcode_data_line */


  /***************************************************************/
  /*  sort_pcodeset_a(p1) pcodeset *p1;                          */
  /*  Sorts the elements of pcodeset *p1 into alphabetic order.  */
  /***************************************************************/
  void sort_pcodeset_a(pcodeset *p1)
  {int nbad,i;
/* first see how disordered it is */
   nbad = 0;
   for (i = 2; i <= p1->ncodes; i++)
    if ( str_less_than(p1->pc[i].ac,p1->pc[i-1].ac) )
      nbad += 1;
/* pick sort method based on no. of inversions and no. of items */
   if (nbad == 0) return;
/*****@@## add full sort later : ##@@*****/
/*   if ( (nbad > 30) || (pt1->ncodes > 300) ) {full_sort(p1); return} */
/* default is a simple sort */
   pcsort1(p1);
   return;
  } /* end sort_pcodeset_a(pcstr) */

/* end file gpcaux.c */
