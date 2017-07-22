 /* file aldist2.c */

/* cleaned-up version of aldist2() which breaks ties    */
/* by preferring substitutions first, then insertions   */


#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  enum edits {DEL,SUB,INS};

  static enum edits pick_min(short int dx[])
/*********************************************************/
/* Picks the minimum cost of dx[edits].                  */
/* If min-cost tie includes a SUB, then it is picked.    */
/* If min-cost tie is between INS and DEL, INS is picked.*/
/*********************************************************/
  {char proc[] = "pick_min";
   enum edits choice;
db_enter_msg(proc,1); /* debug only */
  /* pick min of dx[edits] using special logic to break ties */
   if (dx[DEL] < dx[SUB])
     {if (dx[DEL] < dx[INS]) choice = DEL;
      else
        {if (dx[INS] < dx[DEL]) choice = INS;
         else
           {/* dx[DEL],dx[INS] tie */
            if (db_level > 1) printf("%s DEL,INS tie for min\n",pdb);
            choice = INS; /* arb it */
     }  }  }
   else /* dx[DEL] >= dx[SUB] */
     {if (dx[DEL] == dx[SUB])
        {if (dx[INS] < dx[DEL]) choice = INS;
         else
           {if (dx[INS] > dx[DEL])
              {/* dx[DEL],dx[SUB] tie */
               if (db_level > 1) printf("%s DEL,SUB tie for min\n",pdb);
               /* pick SUB */
               choice = SUB;
              }
            else
              {/* dx[DEL], dx[SUB], dx[INS] tie */
               if (db_level > 1) printf("%s DEL,SUB,INS tie for min\n",pdb);
               /* pick SUB */
               choice = SUB;
        }  }  }
      else
        {/* dx[DEL] is out of contention */
         if (dx[SUB] < dx[INS]) choice = SUB;
          else
           {if (dx[INS] < dx[SUB]) choice = INS;
           else
              {/* dx[SUB], dx[INS] tie */
               if (db_level > 1) printf("%s SUB,INS tie for min\n",pdb);
               /* pick SUB */
               choice = SUB;
     }  }  }  }

db_leave_msg(proc,1); /* debug only */

   return choice;
  }


 /**********************************************************************/
 /*                                                                    */
 /*   int aldist2(pt1,w,return_alignment,pt2,perr)                     */
 /*                                                                    */
 /*   ALIGNMENT_INT *pt1, *pt2; int (*w)(int*,int*,pcodeset*);         */
 /*   boolean return_alignment; int *perr;                             */
 /*                                                                    */
 /*    Computes and returns the alignment distance between the         */
 /* symbols in the two integer strings in *pt1 (aint[] and bint[]).    */
 /* W(i,j) is a pointer to an integer cost (weight or distance)        */
 /* function of two integer variables and a pcodeset, which returns    */
 /* the distance from symbol i to j.                                   */
 /* w(i,j) is the cost of SUBstituting int j for int i.                */
 /* w(0,j) is the cost of INSerting int j;                             */
 /* w(i,0) is the cost of DELeting int j;                              */
 /*    If <return_alignment> is T, this function returns as a          */
 /* by-product the particular alignment found, in *pt2.  The two       */
 /* strings in *pt2 will have been put into alignment by inserting     */
 /* nulls (zero) into one or both of them.                             */
 /*                                                                    */
 /*   *perr = 0 means a.o.k.                                           */
 /*   error codes:                                                     */
 /*    1: (warning) 2 null strings to align                            */
 /*   11:invalid input parameters                                      */
 /*                                                                    */
 /**********************************************************************/

  int aldist2(ALIGNMENT_INT *pt1, int (*w) (int,int,pcodeset*,int*), \
              int return_alignment, ALIGNMENT_INT *pt2, int *perr)
  {char *proc = "aldist2";
/* data: */
   boolean print_warnings = F;
   pcodeset *pcode1;
   int mx,nx,i,j,k,tempa,tempb;
   short int d[MAX_SYMBS_IN_STR][MAX_SYMBS_IN_STR], distance, dx[4];
   short int back_pointer[MAX_SYMBS_IN_STR][MAX_SYMBS_IN_STR];
   enum edits choice;
/* back_pointer[i][j]== DEL iff the selected previous cell is [i-1][j]    */
/* back_pointer[i][j]== SUB iff the selected previous cell is [i-1][j-1]  */
/* back_pointer[i][j]== INS iff the selected previous cell is [i][j-1]    */

/* code: */
 /**** start debug ****/
db_enter_msg(proc,1); /* debug only */
if (db_level > 1 )
   {printf("%sreturn_alignment='%s'\n",pdb,bool_print(return_alignment));
    printf("%sthe pcode is '%s'\n",pdb,(pt1->pcset)->name);
    printf("%sna =%d, nb =%d\n",pdb,pt1->aint[0],pt1->bint[0]);
    printf("%sthe input strings to align are:\n\n",pdb);
    dump_int_alignment(pt1);
   }
 /**** debug ****/

 /* initialize output variables */
   *perr=0;
   distance=0;
   if (return_alignment) init_al_int(pt2,pt1->pcset);
 /* initialize local variables*/
   pcode1 = pt1->pcset;
   mx = pt1->aint[0]; /* (to make simpler, more understandable code) */
   nx = pt1->bint[0];
 /* check input parameters */
   if (mx > MAX_CODE_ELEMENTS)
     {fprintf(stderr,"%s: *ERR: na = %d,> MAX_CODE_ELEMENTS\n",proc,mx);
      *perr=11;
     }
   if (nx > MAX_CODE_ELEMENTS)
     {fprintf(stderr,"%s: *ERR: nb = %d,> MAX_CODE_ELEMENTS\n",proc,nx);
      *perr=11;
     }
   if (mx < 0)
     {fprintf(stderr,"%s: *ERR: na = %d (< 0)\n",proc,mx);
      *perr=11;
     }
   if (nx < 0)
     {fprintf(stderr,"%s: *ERR: nb = %d (< 0)\n",proc,nx);
      *perr=11;
     }
   if ((mx == 0) && (nx == 0))
     {if (print_warnings) fprintf(stderr,"%s: *WARNING: na = 0 and nb = 0\n",proc);
      *perr=1;
      if (return_alignment)
         {pt2->aint[0] = mx;
          pt2->bint[0] = nx;
          pt2->aligned = T;
         }
      goto RETURN;
     }
   if (*perr > 10) goto RETURN;
/* pick off a couple of degenerate cases */
if (db_level > 1) printf("%sjust before deejy-chk, mx=%d, nx=%d\n",pdb,mx,nx);
   if (mx == 0)
     {if (db_level > 1) printf("%sdeejy case of m=0.\n",pdb);
      for (j=1; j<=nx; j++)
        {distance += (*w)(0,pt1->bint[j],pcode1,perr);
         if (return_alignment)
           {pt2->aint[j] = 0;
            pt2->bint[j] = pt1->bint[j];
        }  }
      if (return_alignment)
         {pt2->aint[0] = nx;
          pt2->bint[0] = nx;
          pt2->aligned = T;
         }
      goto RETURN;
     }
   if (nx == 0)
     {if (db_level > 1) printf("%sdeejy case of n=0.\n",pdb);
      for (i=1; i<=mx; i++)
        {distance += (*w)(pt1->aint[i],0,pcode1,perr);
         if (return_alignment)
           {pt2->aint[i] = pt1->aint[i];
            pt2->bint[i] = 0;
        }  }
      if (return_alignment)
        {pt2->aint[0] = mx;
         pt2->bint[0] = mx;
         pt2->aligned = T;
        }
      goto RETURN;
     }
/* now do general case */
   d[0][0] = 0;
/* first calculate values for the margin cells */
if (db_level > 1) printf("%scalculating column margin cells.\n",pdb);
   for (i = 1; i <= mx; i++)
     {d[i][0] = d[i-1][0] + (*w)(pt1->aint[i],0,pcode1,perr);
      back_pointer[i][0] = DEL;
     }
if (db_level > 1) printf("%scalculating row margin cells.\n",pdb);
   for (j = 1; j <= nx; j++)
     {d[0][j] = d[0][j-1] + (*w)(0,pt1->bint[j],pcode1,perr);
      back_pointer[0][j] = INS;
     }
 /* now calculate the body cells */
if (db_level > 1) printf("%scalculating body cells.\n",pdb);
   for (j = 1; j <= nx; j++)
     {for (i = 1; i <= mx; i++)
        {
#define NEW_STYLE
#ifdef OLD_STYLE
	 dx[DEL] = d[i-1][j]   + (*w)(pt1->aint[i],0,pcode1,perr); /* DEL a[i] */
         dx[SUB] = d[i-1][j-1] + (*w)(pt1->aint[i],pt1->bint[j],pcode1,perr); /*SUB*/
         dx[INS] = d[i][j-1]   + (*w)(0,pt1->bint[j],pcode1,perr); /* INS b[j] */
#else
	 dx[DEL] = d[i-1][j]   + d[i][0] - d[i-1][0];
         dx[SUB] = d[i-1][j-1] + (*w)(pt1->aint[i],pt1->bint[j],pcode1,perr); /*sub*/
         dx[INS] = d[i][j-1]   + d[0][j] - d[0][j-1];
#endif
         choice = pick_min(dx);
         d[i][j] = dx[choice];
         back_pointer[i][j] = choice;
     }  }
   distance = d[mx][nx];
 /* return the particular alignment found ? */
   if (return_alignment)
     {k = 0;
      i = mx;
      j = nx;
if (db_level > 1) printf("%smaking return alignment.\n",pdb);
      while ((i > 0) || (j > 0))
        {k += 1;
         if (back_pointer[i][j] == DEL)
           {pt2->aint[k]=pt1->aint[i];
            pt2->bint[k]=0;
            --i;
           }
         else
           {if (back_pointer[i][j] == SUB)
	      {pt2->aint[k]=pt1->aint[i];
               pt2->bint[k]=pt1->bint[j];
               --i;
               --j;
              }
            else
              {if (back_pointer[i][j] == INS)
                 {pt2->aint[k]=0;
                  pt2->bint[k]=pt1->bint[j];
                  --j;
                 }
               else
	         {fprintf(stderr,"*ERR:%s: back pointer=%d\n",proc,
	             back_pointer[i][j]);
                  fprintf(stderr," i=%d, j=%d\n",i,j);
	          *perr=12; goto RETURN;
        }  }  }  }
      pt2->aint[0] = k;
      pt2->bint[0] = k;
/* now reverse the alignment list */
if (db_level > 1) printf("%sreversing alignment list.\n",pdb);
      j = k;
      for (i = 1; i <= (k / 2); i++)
        {tempa = pt2->aint[i];
         tempb = pt2->bint[i];
         pt2->aint[i] = pt2->aint[j];
         pt2->bint[i] = pt2->bint[j];
         pt2->aint[j] = tempa;
         pt2->bint[j] = tempb;
         j--;
        }
/* and put a little more data into the output alignment */
      pt2->aligned = T;
     } /* end of "if (return_alignment) ... */

RETURN:
/*   if (*perr > 0) exit(3);  */ /* if no error tolerance desired */
/**** de-bug ****/
 if (db_level > 2) printf("%sleaving aldist2, distance=%d\n",pdb,distance);
 db_leave_msg(proc,1); /* debug only */
/**** de-bug ****/

   return distance;
  } /* end of function "aldist2" */
 /* end of file "aldist2c.c" */
