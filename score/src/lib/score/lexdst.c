/**********************************************************************/
/*                                                                    */
/*             FILENAME:  lexdst.c                                    */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  These are initialization and and general programs */
/*                  to use a lexicon distance file                    */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

#define SEEK_SET	0
#define SEEK_CUR	1
#define FILE_SHORT_SIZE	5

int seek_num=0, seek_sum=0;

/************************************************************************/
/*                                                                      */
/*   general functions for the LEX_DIST structure                       */
/*                                                                      */
/************************************************************************/
/*   open the file in the "file_name" field and set approriate flags    */
void open_LEX_DIST_file(LEX_DIST *ldist, char *format)
{
    if ((*(ldist->file_name) == NULL_CHAR) ||
        ((ldist->fp = fopen(ldist->file_name,format)) == NULL)){
        fprintf(stderr,"Could not open -%s- to -%s- the lex_dist file\n",
                ldist->file_name,format);
        exit(-1);
    }
    ldist->fp_is_open = TRUE;
}

/************************************************************************/
/*   Close the already opened file in the "fp" field, set flags         */
void close_LEX_DIST_file(LEX_DIST *ldist)
{
    fclose(ldist->fp);
    ldist->fp_is_open = FALSE;
}

/************************************************************************/
/*   General procedure to dump the current form of a distance matrix    */
void dump_LEX_DIST(LEX_DIST *ldist, FILE *fp)
{
    int r,c,size;

    size = ldist->num_words;

    /* print the struct info */
    fprintf(fp,"Dump of a Lexicon distance file\n\n");
    fprintf(fp,"   Filename     -> %s\n",ldist->file_name);
    fprintf(fp,"   is in mem    -> %d\n",ldist->is_in_memory);
    fprintf(fp,"   num words    -> %d\n",ldist->num_words);
    fprintf(fp,"   scale        -> %d\n",ldist->scale_factor / 1000);
    fprintf(fp,"   file is open -> %d\n",ldist->fp_is_open);
    fprintf(fp,"   file hdr size-> %d\n",ldist->hdr_size);
    fprintf(fp,"\n");
    
    /* print the matrix */
    fprintf(fp,"     ");
    for (c=0; c < size; c++)
        fprintf(fp," %3d",c);
    fprintf(fp,"\n-----");
    for (c=0; c < size; c++)
        fprintf(fp,"----");
    fprintf(fp,"\n");
    for (r=0; r < size; r++){
        fprintf(fp,"%3d |",r);
        for (c=0; c < size; c++)
            fprintf(fp,"%3d ",ldist->dist_funct(ldist,r,c));
        fprintf(fp,"\n");
    }
}

/************************************************************************/
/*  close the existing opened random access file                        */
void close_LEX_DIST_in_file(LEX_DIST *ldist)
{
    close_LEX_DIST_file(ldist);
}

/************************************************************************/
/*                                                                      */
/*   Initialize and open the file to read distance data from            */
/*                                                                      */
/************************************************************************/
void init_LEX_DIST_in_file_input(LEX_DIST **ldist, char *fname)
{
    LEX_DIST *tld;
    char  size_buff[FILE_SHORT_SIZE+1],scale_buff[FILE_SHORT_SIZE+1];

    alloc_singarr(tld,1,LEX_DIST);

    /* open the file for input */
    tld->file_name    = fname;
    open_LEX_DIST_file(tld,"r");

    /* load the fields of the LEX_DIST structure */
    tld->is_in_memory = FALSE;
    tld->dist_matr = NULL_CHAR;

    /* read the size and the scaling factor from the file and reset */
    fread(size_buff,sizeof(char),FILE_SHORT_SIZE,tld->fp);
    size_buff[FILE_SHORT_SIZE+1] = NULL_CHAR;
    fread(scale_buff,sizeof(char),FILE_SHORT_SIZE,tld->fp);
    scale_buff[FILE_SHORT_SIZE+1] = NULL_CHAR;

	
    tld->num_words = (short) atoi(size_buff);
    tld->scale_factor = (short) atoi(scale_buff);

    /* check the size and the scale to make sure the values seem reasonable*/
    if ((tld->num_words < 0) || (tld->num_words > 10000)){
        fprintf(stderr,"Error: the number of words in the distance table\n");
        fprintf(stderr,"       seems odd %d\n",tld->num_words);
        exit(-1);
    }
    if ((tld->scale_factor < 0) || (tld->scale_factor > 10000)){
        fprintf(stderr,"Error: the scaling factor in the distance table\n");
        fprintf(stderr,"       seems odd %d\n",tld->scale_factor);
        exit(-1);
    }

    rewind_LEX_DIST_file(tld);
    tld->hdr_size = FILE_SHORT_SIZE * 2;

    tld->dist_funct = file_cell;
    tld->stor_dist = set_file_cell;
    *ldist = tld;
}

void init_LEX_DIST_in_file_output(LEX_DIST **ldist, int size, char *fname)
{
    LEX_DIST *tld;
    char size_buff[FILE_SHORT_SIZE+1], scale_buff[FILE_SHORT_SIZE+1];

    alloc_singarr(tld,1,LEX_DIST);

    /* open the file for input */
    tld->file_name    = fname;
    open_LEX_DIST_file(tld,"w+");

    /* load the fields of the LEX_DIST structure */
    tld->is_in_memory = FALSE;
    tld->dist_matr = NULL_CHAR;

    /* read the size and the scaling factor from the file and reset */
    tld->num_words = size;
    tld->scale_factor = 1000;
    tld->hdr_size = FILE_SHORT_SIZE * 2;
    tld->cur_fp_pos = 0;

    nist_itoa(tld->num_words, size_buff, FILE_SHORT_SIZE);
    nist_itoa(tld->scale_factor, scale_buff, FILE_SHORT_SIZE);

    /* write out the header information */
    fwrite(size_buff,sizeof(char),FILE_SHORT_SIZE,tld->fp);
    fwrite(scale_buff,sizeof(char),FILE_SHORT_SIZE,tld->fp);
    rewind_LEX_DIST_file(tld);

    tld->dist_funct = file_cell;
    tld->stor_dist = set_file_cell;
    *ldist = tld;
}

/************************************************************************/
/*  Return the value of a particular distance from the file             */
char file_cell(LEX_DIST *ldist, int r, int c)
{
    int dist,move;

    move = (cell(ldist->num_words,r,c)+ldist->hdr_size) - ldist->cur_fp_pos;
    seek_num++;
    seek_sum+=abs(move);

    if (move != 0)
        if(fseek(ldist->fp,move,SEEK_CUR) != 0){
            fprintf(stderr,"FSEEK FAILED \n");
            exit (-1);
        }
    
    ldist->cur_fp_pos = ldist->cur_fp_pos + move + 1;
    dist = fgetc(ldist->fp);
    return(dist);
}

/************************************************************************/
/*  set the value of a particular cell to the distance                  */
char set_file_cell(LEX_DIST *ldist, int r, int c, int val)
{
    int dist, move;

    if ((val > MAX_DIST_VAL) || (val < 0)){
        fprintf(stderr,"ERROR: Distance cell value out of bounds %d\n",val);
        exit(-1);
    }
    move = (cell(ldist->num_words,r,c)+ldist->hdr_size) - ldist->cur_fp_pos;
    seek_num++;
    seek_sum+=abs(move);

/*     printf("r%d c%d pos%d  move%d\n",r,c,ldist->cur_fp_pos,move);*/
    if (move != 0)
        if(fseek(ldist->fp,move,SEEK_CUR) != 0){
            fprintf(stderr,"FSEEK FAILED \n");
            exit (-1);
        }
    ldist->cur_fp_pos = ldist->cur_fp_pos + move + 1;
    fputc(val,ldist->fp);
    return(dist);
}

/***********************************************************************/
/*  rewind the opened data file                                        */
void rewind_LEX_DIST_file(LEX_DIST *ldist)
{
    rewind(ldist->fp);
    ldist->cur_fp_pos = 0;
}

/***********************************************************************/
/*                                                                     */
/*   Structure intialization and use for an "in memory" model of the   */
/*   LEX_DIST structure                                                */
/***********************************************************************/
/*   Initialize the LEX_DIST structure to hold the information in      */
/*   memory.                                                           */
/*               from_file == TRUE if the distance matrix should be    */
/*                                 loaded from a file                  */
void init_LEX_DIST_in_mem(LEX_DIST **ldist, int size, char *fname,
			  int from_file)
{
    LEX_DIST *tld;

    alloc_singarr(tld,1,LEX_DIST);

    /* load the fields of the LEX_DIST structure */
    tld->file_name    = fname;
    tld->is_in_memory = TRUE;
    tld->scale_factor = 1000;
    tld->fp           = (FILE *)0;
    tld->fp_is_open   = FALSE;
    tld->hdr_size     = 0;

    /* set the function pointers for operations */
    tld->dist_funct = mem_cell;
    tld->stor_dist = set_mem_cell;

    if (from_file)  /* then read in the file from disk */
        read_in_LEX_DIST(tld);
    else{
        tld->num_words    = size;  /* only a ten word lexicon for now */
        alloc_singZ(tld->dist_matr,tld->num_words*tld->num_words,
              unsigned char,0);
    }

    *ldist = tld;
}
 

void write_LEX_DIST_from_mem(LEX_DIST *ldist)
{
    short size;
    char size_buff[FILE_SHORT_SIZE+1], scale_buff[FILE_SHORT_SIZE+1];

    size = ldist->num_words;

    open_LEX_DIST_file(ldist,"w");

    nist_itoa(ldist->num_words, size_buff, FILE_SHORT_SIZE);
    nist_itoa(ldist->scale_factor, scale_buff, FILE_SHORT_SIZE);

    fwrite(size_buff,sizeof(char),FILE_SHORT_SIZE,ldist->fp);
    fwrite(scale_buff,sizeof(char),FILE_SHORT_SIZE,ldist->fp);
    fwrite((char *)ldist->dist_matr,sizeof(char),size*size,ldist->fp);

    close_LEX_DIST_file(ldist);
}

void read_in_LEX_DIST(LEX_DIST *ldist)
{
    char  size_buff[FILE_SHORT_SIZE+1],scale_buff[FILE_SHORT_SIZE+1];

    open_LEX_DIST_file(ldist,"r");

    /* read the size and the scaling factor from the file and reset */
    fread(size_buff,sizeof(char),FILE_SHORT_SIZE,ldist->fp);
    size_buff[FILE_SHORT_SIZE+1] = NULL_CHAR;
    fread(scale_buff,sizeof(char),FILE_SHORT_SIZE,ldist->fp);
    scale_buff[FILE_SHORT_SIZE+1] = NULL_CHAR;

    ldist->num_words = (short) atoi(size_buff);
    ldist->scale_factor = (short) atoi(scale_buff);

    /* check the size and the scale to make sure the values seem reasonable*/
    if ((ldist->num_words < 0) || (ldist->num_words > 10000)){
        fprintf(stderr,"Error: the number of words in the distance table\n");
        fprintf(stderr,"       seems odd %d\n",ldist->num_words);
        exit(-1);
    }
    if ((ldist->scale_factor < 0) || (ldist->scale_factor > 10000)){
        fprintf(stderr,"Error: the scaling factor in the distance table\n");
        fprintf(stderr,"       seems odd %d\n",ldist->scale_factor);
        exit(-1);
    }

    ldist->hdr_size = FILE_SHORT_SIZE * 2;

    /* alloc the the memory to hold the distance matrix and read it in*/
    alloc_singZ(ldist->dist_matr,ldist->num_words*ldist->num_words,
              unsigned char,0);
    fread(ldist->dist_matr,sizeof(unsigned char),
              ldist->num_words*ldist->num_words,ldist->fp);

    close_LEX_DIST_file(ldist);
}

char mem_cell(LEX_DIST *ldist, int r, int c)
{
    return(ldist->dist_matr[cell(ldist->num_words,r,c)]);
}

char set_mem_cell(LEX_DIST *ldist, int r, int c, int val)
{
    return(ldist->dist_matr[cell(ldist->num_words,r,c)] = val);
}
