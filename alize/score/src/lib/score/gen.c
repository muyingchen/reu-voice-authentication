/**********************************************************************/
/*                                                                    */
/*             FILENAME:  gen.c                                       */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains general routines used          */
/*                  throughout the scoring package                    */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include <score/scorelib.h>

/***************************************************************/
/*  given a numXnum array of characters, make a strings of     */
/*  spaces with there length according row in the MATRIX       */
/***************************************************************/
void make_space(char **space, int num, char chr)
{
    int i,j;

    space[0][0] = NULL_CHAR;
    for (i=1;i<num;i++){
        for (j=0;j<i;j++)
            space[i][j] = chr;
        space[i][j] = NULL_CHAR;
    }
}

/*******************************************************************/
/*    given an array of float's, num items long, return the sum    */
/*******************************************************************/
float sum_float_singarr(float *arr, int num)
{
    float count = 0.00000;
    int i;

    for (i=0; i<num; i++)
        count += arr[i];
    return(count);
}


int convert_num(char chr)
{
    if ((chr >= '0') && (chr <= '9'))
        return(chr - '0');
    if ((chr >= 'a') && (chr <= 'z'))
        return(chr - 'a' + 10);
    if ((chr >= 'A') && (chr <= 'Z'))
        return(chr - 'A' + 10);
    fprintf(stderr,"Error: Illegal character '%d' in base-36 field\n",chr);
    return(0);
}

int convert_num2char(int num)
{
    if ((num >= 0) && (num <= 9))
        return(num + '0');
    return((num - 10) + 'A');
}

int atis2num(char *str)
{
    int count=0, base=36, cur_pos=1;
    char *ptr;

    /* find the end of the string */
    ptr = str;
    while (*ptr != NULL_CHAR) 
        ptr++;
    ptr--;
  
    /* calculate the number */
    while (ptr != (str-1)){
       count += convert_num(*ptr) * cur_pos;
       cur_pos *= base;
       ptr --;
    }
    return(count);
}

char *num2atis(int num)
{
    int base=36, i, pos;
    char *ptr;
    static char buff[10];

    /* find the end of the string */
    ptr = buff;
  
    pos = base*base*base;
    /* calculate the number */
    for (i=0; i<4; i++){
       ptr[i] = convert_num2char(num / pos);
       num %= pos;
       pos /= base;
    }
    ptr[4] = NULL_CHAR;
    return((char *)buff);
}

/**********************************************/
/* print the first N characters in the string */
/**********************************************/
void print_substr(FILE *fp, int num, char *str)
{
    int i;
    for (i=0; ((i<num) && (str[i]!='\0')); i++)
       fprintf(fp,"%c",str[i]);
    for (; (i<num); i++)
       fprintf(fp," ");
}

/*******************************************************************/
/*   check the character pointer to see if it points to the        */
/*   comment character                                             */
/*******************************************************************/
int is_comment(char *str)
{
   if ((*str == COMMENT_CHAR) && (*(str+1) != COMMENT_CHAR)){
      fprintf(stderr,"Warning: The comment designation is now %c%c, the line below\n",
                     COMMENT_CHAR,COMMENT_CHAR);
      fprintf(stderr,"         has only one comment character, this may");
     fprintf(stderr," be an error\n         %s\n",str);
   }

   if ((*str == COMMENT_CHAR) && (*(str+1) == COMMENT_CHAR))
       return(TRUE);
   else
       return(FALSE);
}

/*******************************************************************/
/*   check the character pointer to see if it points to the        */
/*   comment_info character                                        */
/*******************************************************************/
int is_comment_info(char *str)
{
   if ((*str == COMMENT_INFO_CHAR) && (*(str+1) != COMMENT_INFO_CHAR)){
      fprintf(stderr,"Warning: The comment designation is now %c%c, the line below\n",
                     COMMENT_INFO_CHAR,COMMENT_INFO_CHAR);
      fprintf(stderr,"         has only one comment info character, this may");
     fprintf(stderr," be an error\n         %s\n",str);
   }
   if ((*str == COMMENT_INFO_CHAR) && (*(str+1) == COMMENT_INFO_CHAR))
       return(TRUE);
   else
       return(FALSE);
}

