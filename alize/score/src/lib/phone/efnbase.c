/* file efnbase.c */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

  /************************************************************/
  /*                                                          */
  /*  char *extract_fname_base(fname,fname_base)              */
  /*  Extracts the base of the file name *fname and leaves it */
  /* in *fname_base, returning fname_base.                    */
  /*                                                          */
  /************************************************************/
 char *extract_fname_base(char *fname, char *fname_base)
{char *p;
  p = strrchr(fname,'/');
  if (p == NULL) p = fname;
  else           p += 1;
  fname_base = strcpy(fname_base,p);
  p = strrchr(fname_base,'.');
  if (p != NULL) *p = '\0';
  return fname_base;
 }
