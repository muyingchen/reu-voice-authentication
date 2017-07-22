/**********************************************************************/
/*                                                                    */
/*             FILENAME:  dir.c                                       */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains general routines used          */
/*                  throughout the scoring package                    */
/*                                                                    */
/**********************************************************************/
#include <util/utillib.h>

#if defined(NARCH_NEXT)
#include <sys/dir.h>
#else
#include <dirent.h>
#endif

#include <stdio.h>
/***************************************************************/
/*  given a directory, make sure it exists and you have write  */
/*  access                                                     */
/***************************************************************/
int dir_exists(char *dir)
{
    DIR *dirp;

    if ((dirp = opendir(dir)) == NULL){
        return(0);
    }
    closedir(dirp);
    return(1);
}

/*********************************************************************/
/*   return true if the file with the name 'fname' can be opened     */
/*   for reading                                                     */
/*********************************************************************/
int file_readable(char *fname)
{
    FILE *fp;

    if ((*(fname) == NULL_CHAR) || ((fp=fopen(fname,"r")) == NULL))
        return(0);
    fclose(fp);
    return(1);
}

/**************************************************/
/*   Return the byte size of the file             */
#include <sys/stat.h>

int file_size(char *fname)
{
    struct stat fileinfo;
    if (stat(fname,&fileinfo) != 0)
	return(-1);
    return(fileinfo.st_size);
}
