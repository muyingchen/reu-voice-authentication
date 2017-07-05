/* File ppfstr2.h - phone-phone function structure #1 */

#ifndef PPFSTR2_HEADER
#define PPFSTR2_HEADER

 typedef struct ppfcn_table
   {pcodeset *pcode;
    char *external_format;
    boolean symmetric;
    boolean diagonal_zeros;
    int max_fval;
    int min_fval;
    int default_value;
    int fval[MAX_PHONES+1][MAX_PHONES+1];
   } ppfcn;

#endif
