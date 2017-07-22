/**********************************************************************/
/*                                                                    */
/*             FILENAME:  diskarr.c                                   */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This module will initialize, access and store     */
/*                  information for a 2 dimensional table on disk     */
/*                                                                    */
/*   dbg flags: <= 0    = Run silent                                  */
/*              >  0    = Status messages                             */
/*              >= 5    = analysis of all computations                */
/*              >= 9    = Dump of the disk array                      */
/*                                                                    */
/**********************************************************************/

#include <util/utillib.h>

#define SEEK_SET	0
#define SEEK_CUR	1

/**********************************************************************/
/*  Declare local initialization parameters, these are not external   */
/**********************************************************************/
static int array_x, array_y, array_elem_size;
static int array_cur_pos;
FILE *array_fp;

/**********************************************************************/
/*  In the file 'filename' create and erase a 2 dimensional table     */
/*  of size 'x' by 'y'                                                */
/**********************************************************************/
void init_disk_array(char *filename, int x, int y, int dbg)
{
   int i,j;

   array_elem_size = sizeof(DISK_T);
   array_x = x;
   array_y = y;
   if (dbg > 0) printf("Disk array file name '%s'\n",filename);
   if (dbg > 0) printf("Initializing disk array . . . %d bytes (%d,%d,%d) . . . ",
          x*y*array_elem_size,x,y,array_elem_size);
   fflush(stdout);
   if ((array_fp = fopen(filename,"w+")) == NULL){
       fprintf(stderr,"Could not open disk array file %s\n",filename);
       exit(-1);
   }
   array_cur_pos = 0;
   rewind(array_fp);
   for (i=0; i<y; i++)
       for (j=0; j<x; j++)
           set_disk_element(j,i,init_val);
   if (dbg > 0) printf("Done\n");
   fflush(stdout);
}

/**********************************************************************/
/*   clean up the disk array module                                   */
/**********************************************************************/
void close_disk_array(void)
{
   fclose(array_fp);
}


/**********************************************************************/
/*  to range checks on the x,y arguments                              */
/**********************************************************************/
void check_arg(int x, int y)
{
    int fail=FALSE;
    if (x<0) {fprintf(stderr,"Error: Disk array X index out of range, %d < 0\n",x); fail=TRUE;}
    if (y<0) {fprintf(stderr,"Error: Disk array Y index out of range, %d < 0\n",y); fail=TRUE;}
    if (x>array_x) {fprintf(stderr,"Error: Disk array X index out of range, %d > %d\n",x,array_x-1);
                    fail=TRUE;}
    if (y>array_y) {fprintf(stderr,"Error: Disk array Y index out of range, %d > %d\n",y,array_y-1);
                    fail=TRUE;}
    if (fail) 
        exit(-1);
}

/**********************************************************************/
/*   move the file pointer to the proper location and set the value   */
/**********************************************************************/
void set_disk_element(int x, int y, DISK_T value)
{
   int new_loc, move;

   check_arg(x,y);
   new_loc = (x + (array_x * y)) * array_elem_size;
   move = new_loc - array_cur_pos;
/*   printf("SET : cur_pos %d\n",array_cur_pos);*/
   array_cur_pos += move  + array_elem_size;
/*   printf("      moved to %d\n",array_cur_pos); */

   if (move != 0)
       if (fseek(array_fp,move,SEEK_CUR) != 0){
            fprintf(stderr,"FSEEK FAILED \n");
            exit (-1);
       }
   if (fwrite((char *)&value,1,array_elem_size,array_fp) != array_elem_size){
       fprintf(stderr,"FWRITE FAILED on (%d,%d) = %d\n",x,y,value);
       exit(-1);
   }
/*   printf("     value (%d,%d) %d\n",x,y,value);*/
}

/**********************************************************************/
/*   move the file pointer to the proper location and read the value  */
/*   the return it the the value variable                             */
/**********************************************************************/
void get_disk_element(int x, int y, DISK_T *value)
{
   int new_loc, move;

   check_arg(x,y);
   new_loc = (x + (array_x * y)) * array_elem_size;
   move = new_loc - array_cur_pos;
/*   printf("GET : cur_pos %d\n",array_cur_pos);*/
   array_cur_pos += move  + array_elem_size;
/*   printf("      moved to %d\n",array_cur_pos);*/

   if (move != 0)
       if (fseek(array_fp,move,SEEK_CUR) != 0){
            fprintf(stderr,"FSEEK FAILED \n");
            exit (-1);
       }
   if (fread((char *)value,1,array_elem_size,array_fp) != array_elem_size){
       fprintf(stderr,"FREAD FAILED on (%d,%d)\n",x,y);
       exit(-1);
   }
/*   printf("get value (%d,%d) %d\n",x,y,*value);*/
}

/**********************************************************************/
/*   Print the array elements to the file 'fp'                        */
/**********************************************************************/
void print_disk_array(FILE *fp)
{
    int i,j;
    DISK_T value;

    fprintf(fp,"Dump of the disk Array\n");
    fprintf(fp,"   X = %d, Y = %d\n\n",array_x,array_y);
    
    for (j=0; j<array_y; j++){
        for (i=0; i<array_x; i++){
            get_disk_element(i,j,&value);
            fprintf(fp,"%4d ",value);
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"\n");
}

/**********************************************************************/
/*   compute and return the entropy of the 2-dimensional table        */
/**********************************************************************/
void disk_compute_entropy(int dbg, float *entropy_out)
{
    /* ok, compute the entropy */
    double entropy=0.0;
    int pair_count=0;
    int num_pair=0;
    int i, j;
    DISK_T value;

    if (dbg>9) print_disk_array(stdout);

    if (dbg > 0) printf("Counting the pairs . . . ");
    fflush(stdout);
    if (dbg > 5) printf("\n");
    for (pair_count=0, j=0; j<array_y; j++)
        for (i=0; i<array_x; i++){
            get_disk_element(i,j,&value);
            if (value > 0){
                if (dbg>5) printf("Confusion pair: (%d,%d)\n",i,j);
                pair_count += value;
                num_pair++;
            }
        }
    if (dbg > 0) printf("There are %d pairs, %d unique\n",pair_count,num_pair);
    if (dbg > 0) printf("Computing entropy . . . ");
    if (dbg > 5) printf("\n");
    fflush(stdout);
    for (j=0; j<array_y; j++)
        for (i=0; i<array_x; i++){
            float contrib;
            get_disk_element(i,j,&value);
            if (value > 0){
/*                contrib =  (-((double)value/(double)pair_count) *
                            log2((double)value/(double)pair_count));*/
                contrib =  (-((double)value/(double)pair_count) *
                            log((double)value/(double)pair_count) / 0.69314718055994530942);
                entropy += contrib;
                if (dbg > 5) printf("Entropy for pair (%d,%d)[%d] = %f total = %f\n",
                                    i,j,value,contrib,entropy);
            }
        }
    if (dbg > 0) printf("Entropy is %f\n",entropy);
    *entropy_out = entropy;
}
