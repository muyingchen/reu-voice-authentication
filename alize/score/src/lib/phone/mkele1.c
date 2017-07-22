/* file mkele1.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /*************************************************************************/
  /*  pcode_element *make_element(px,symb,pc1)                             */
  /*  Makes a pcode element *px, using spelling <symb> and pcodeset *pc1.  */
  /*  Returns px; or NULL to signify fatal error.                          */
  /*************************************************************************/
  pcode_element *make_element(pcode_element *px, char *symb, pcodeset *pc1)
{char *proc = "make_element";
    char sxx[LINE_LENGTH], *sx = &sxx[0];
    char sxy[LINE_LENGTH], *sy = &sxy[0];
    char dxx[LINE_LENGTH], *descrip = &dxx[0];
    int sinx[MAX_SYMBS_IN_STR+1], err;
    static boolean already_warned = F;
    RULESET1 *rset;
/* code */
 db_enter_msg(proc,0); /* debug only */
    err = 0;
    px->ac = strdup(symb);
    px->lc_ext = EMPTY_STRING;
    px->lc_int = EMPTY_INT_STRING;
if (db_level > 0) printf("%ssymb to process is'%s'\n",pdb,symb);
if (db_level > 0) printf("%spc1->desc:[%s]\n",pdb,pc1->desc);
    if (pc1->compositional)
       {if (streq(pc1->element_type,"word"))
          {/* make lower code external rep in sx using text-to-phone rules: */
if (db_level > 0) printf("%sprocessing a compositional word code\n",pdb);
           descrip = make_upper(strcpy(descrip,pc1->desc));
           sx = strcpy(sx,"");
           if (strstr(descrip,"ENG") != NULL) sx = ttp1(symb,sx,&err);
           if (strstr(descrip,"JAP") != NULL) sx = ttpj1(symb,sx,&err);
   /* N.B.: Spanish ttp fcn will use rules in directory pc1->directory: */
           if (strstr(descrip,"SPAN") != NULL)
                sx = ttpspan2(symb,sx,pc1->directory,&err);
        /* default to English */
           if (streq(sx,""))
             {if (!already_warned)
                {fprintf(stderr,"*WARNING: using default English TTP function.\n");
                 already_warned = T;
	        }
              sx = ttp1(symb,sx,&err);
             }
if (db_level > 0) printf("%sreturned from TTP*, output sx='%s'\n",pdb,sx); 
if (db_level > 0)
  {if (pc1->lower_code == NULL) printf("%spc1->lower_code == NULL\n",pdb);
   else                         printf("%spc1->lower_code != NULL\n",pdb);
  }
if (db_level > 0) printf("%spc1->lower_code->name='%s'\n",pdb,pc1->lower_code->name);

          if (textlen(sx) < 1)
            {fprintf(stderr,"*WARNING: ttp('%s') returns nothing.\n",symb);
            }
          if ((sx != NULL)&&(textlen(sx) > 0))
             {/* fix up & encode lower code rep */
              if (pc1->lower_code == NULL)
                {fprintf(stderr,"*ERR: %s: a lower-code",proc);
                 fprintf(stderr," representation cannot be encoded.\n");
                 fprintf(stderr," No lower pcode is defined.\n");
                 fprintf(stderr," pcode name='%s', entry='%s' = /%s/\n",
                   pc1->name, px->ac,px->lc_ext);
                 err = 13; goto EXIT;
		}
        /* first apply rules to change code from the stn made by ttp* */
              rset = pc1->lower_code->from_stn_rules;
if (db_level > 0)
 {if (rset == NULL) printf("%slower code from_stn_rules rset == NULL\n",pdb);
  else  printf("%slower code from_stn_rules rset='%s'\n",pdb,rset->name);
 }
              if (rset != NULL)
                {sy = strcpy(sy,"");
                 sy = apply_rules1(sy,sx,rset,&err);
if (db_level > 0) printf("%srule output=sy='%s'\n",pdb,sy);
                 if (err > 0)
                   {fprintf(stderr,"*ERR:%s: apply_rules err %d\n",proc,err);
                    fprintf(stderr," applying ruleset '%s' to:\n",rset->name);
                    fprintf(stderr,"'%s'\n",sx);
                    goto EXIT;
		}  }
              else sy = sx;
         /* and remove initial & final word boundaries */
              px->lc_ext = strdup(wbtrim(sy));
if (db_level > 0) printf("%strying to encode '%s'\n",pdb,px->lc_ext);
              encode2(px->lc_ext,pc1->lower_code,sinx,sinx,&err);
              if (err > 0)
                {fprintf(stderr,"*ERR: %s: a lower-code",proc);
                 fprintf(stderr," representation cannot be encoded.\n");
                 fprintf(stderr," encode2 returns err=%d\n",err);
                 fprintf(stderr," lower pcode name='%s', entry='%s' = /%s/\n",
                   pc1->lower_code->name, px->ac,px->lc_ext);
                 err = 11; goto EXIT;
                }
if (db_level > 1) printf("*DB: returned from encode2, adding pc element.\n");
              px->lc_int = nstrdup(sinx);
       }  }  }
  EXIT:
 db_leave_msg(proc,0); /* debug only */
     if (((px->ac == NULL)||(px->lc_ext == NULL))||(err > 0)) return NULL;
    else return px;
   }
/* end include mkele.c */
