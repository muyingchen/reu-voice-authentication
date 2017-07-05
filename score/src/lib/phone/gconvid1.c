/* file gconvid1.c   */

#if !defined(COMPILE_ENVIRONMENT)
#include <phone/stdcenvf.h> /* std compile environment for functions */
#endif

 /**********************************************************************/
 /*                                                                    */
 /*    char *get_conv_id(conv_id,mark_file_name);                      */
 /*    char *conv_id, *mark_file_name;                                 */
 /*                                                                    */
 /*    Gets the conversation i.d. from the file name *mark_file_name   */
 /* and puts it into *conv_id, returning *conv_id.                     */
 /*    Modifying 10/22/92 by WMF to use all * characters in a          */
 /* mark_file_name analyzable as sw*.mrk as the conv_id; assumes that  */
 /* conv_id can hold LINE_LENGTH characters.                           */
 /*                                                                    */
 /* ANSI-ized by WMF on 11/23/94.                                      */
 /**********************************************************************/
 char *get_conv_id(char *conv_id,char *mark_file_name)
  {
/* data: */
   char *proc = "get_conv_id";
   char sxx[LINE_LENGTH], *sx = &sxx[0];
   char *p1;
   SUBSTRING ssx_data, *ssx = &ssx_data;
/* code: */
db_enter_msg(proc,9); /* debug only */
   p1 = strrchr(mark_file_name,'/');
   if (p1 == NULL) p1 = mark_file_name;
   else            p1 += 1;
   sx = strcpy(sx,p1);
/* file name minus path is in *sx */
   if ((*sx == 's')&&(*(sx+1) == 'w')) /* sw1234.mrk convention */
     {ssx->start = sx+2;
      ssx->end   = strstr(sx,".m");
      if (ssx->end == NULL)
        {fprintf(stderr,"*ERR:%s: invalid .mrk file name '%s'\n",
           proc,mark_file_name);
         ssx->end = ssx->start;
        }
       else ssx->end -= 1;
      conv_id = substr_to_str(ssx,conv_id,LINE_LENGTH);
     }
   else  /* 1234_9214_3847.marked convention */
     {p1 = strchr(sx,'_');
      if (p1 != NULL) *p1 = '\0';
      conv_id = strcpy(conv_id,sx);
     }
db_leave_msg(proc,9); /* debug only */
   return conv_id;
  } /* end of function "get_conv_id" */
