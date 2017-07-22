#include <util/utillib.h>

#ifdef __STDC__
# include <stdarg.h>
#else
# include <varargs.h>
#endif

#include <util/chars.h>
#include <util/memory.h>
#include <util/order.h>
#include <util/macros.h>
#include <util/rpg.h>

#define MAX_COL   40
#define MAX_ROW   500
#define MAX_JUST  40
#define MAX_SEP   30
#define MAX_VALUE_LEN   500

#define LOCKED		'l'
#define UNLOCKED	'u'

static int PAGE_WIDTH=80;
static int CENTER_PAGE=0;
static int static_rpt_dbg=0;
static int static_parse_dbg=0;

typedef struct column_justification_struct{
    char format_str[40];
    int min_just_width;
    int num_col;
    int col_just[MAX_COL];
    int col_lock[MAX_COL];
    int num_locked;
    char col_inter_space[MAX_COL];
} COLUMN_JUST;


typedef struct report_justification_struct{
    int width;
    int tot_num_col;
    int num_col_just;
    COLUMN_JUST col[MAX_COL];
    int tot_num_row;
    char before_row_separations[MAX_ROW+1][MAX_SEP];
    char after_row_separations[MAX_ROW+1][MAX_SEP];
    int row_just[MAX_ROW];
    char cell_values[MAX_COL][MAX_ROW][MAX_VALUE_LEN];
    /*   These variables are for column sizing information */
    int max_col_sizes[MAX_COL][MAX_COL];  /* this is dependent on the number of columns defined in the justification */
} REPORT_JUST;

static REPORT_JUST report_just;

int char_in_set(char chr, char *set)
{
    while (*set != '\0'){
	printf("char in set %c -> %c\n",chr,*set);
	if (chr == *set)
	    return(TRUE);
	set++;
    }
    return(FALSE);
}

void Desc_set_parse_verbose(int dbg){
    static_parse_dbg = dbg;
}

void Desc_set_report_verbose(int dbg){
    static_rpt_dbg = dbg;
}

void Desc_erase(void)
{
    int r, c;

    report_just.width = 80;
    report_just.tot_num_col = 0;
    report_just.num_col_just = 0;
    report_just.tot_num_row = 0;
    
    for (r=0 ; r<MAX_ROW; r++){
	report_just.before_row_separations[r][0] = '\0';	
	report_just.after_row_separations[r][0] = '\0';	
    }
    for (c=0; c<MAX_COL; c++)
	report_just.col[c].num_col = 0;
}

void Desc_set_page_center(int width)
{
    PAGE_WIDTH = width;
    CENTER_PAGE = 1;
}

int Desc_set_justification(char *just_str)
{ 
    char *p;
    int j, col=0;

    for (j=0; j<report_just.num_col_just; j++)
	if (strcmp(just_str,report_just.col[j].format_str) == 0)
	    return(j);

    if (j == MAX_JUST)
	return(-1);

    strcpy(report_just.col[j].format_str,just_str);
    for (p=just_str; *p!='\0' && *p!=':'; ){
	if (col >= MAX_COL){
	    fprintf(stderr,"Error: too many columns defined > %d\n",MAX_COL);
	    exit(-1);
	}
	if ((*p == 'c') || (*p == 'C') || (*p == 'l') || (*p == 'L') ||
	    (*p == 'r') || (*p == 'R') || (*p == 'n') || (*p == 'a')){
	    report_just.col[j].col_just[col] = *p;
	    report_just.col[j].col_lock[col] = UNLOCKED;
	    report_just.col[j].num_col++;
	    p++;
	}
	if (*p == '|'){	
	    report_just.col[j].col_inter_space[col] = '|';
	    p++;
	} else 
	    report_just.col[j].col_inter_space[col] = ' ';
	col++;
    }
    report_just.num_col_just++;
    if (col > report_just.tot_num_col)
	report_just.tot_num_col = col;
    /* now check for the locking column information */
    report_just.col[j].num_locked=0;
    if (*p == ':')
	p++;
    for (col=0; *p!='\0' ; col++, p++){
	if (col >= MAX_COL){
	    fprintf(stderr,"Error: too many columns defined > %d\n",MAX_COL);
	    exit(-1);
	}
	if ((*p == 'l')){
	    report_just.col[j].col_lock[col] = LOCKED;
	    report_just.col[j].num_locked++;
	} else 
	    report_just.col[j].col_lock[col] = UNLOCKED;
    }
    return(j);       
}

char *get_next_string_value(char **str, int width)
{
    static char buf[150];
    char *p;
    int x=0;

    p = *str;

    for (x=0; (x < width) && (*p != '\0') && !(*p == '/' && *(p+1) == '/'); x++, p++)
	buf[x] = *p;
    if (*p == '/' && *(p+1) == '/')
	p+=2;
    else if (*p != '\0' && ((*(p+1) != ' ') || (*(p+1) != '/'))){
	/* backup to a space */
	while ((p != *str) && (*p != ' ')){
	    p--;
	    x--;
	}
	if (*p == ' ')
	    p++;
    }
    buf[x] = '\0';
    *str = p;
    return(buf);
}

void print_spaces(int n, FILE *fp)
{
    int x;
    for (x=0; x<n; x++)
	fprintf(fp," ");
}

#define SPACES_LEN   100
void Desc_dump_report(int space_pad, FILE *fp)
{
    int c, r, x, text_width, c2, j;
    int hit[MAX_COL], column_width;
    char *p;
    char fmt_str1[200]; 
    char *spaces="                                                                                                    ";
    if (static_rpt_dbg){
	int i,c,r,j,startc;
	printf("Dump of description:   %d Total Columns\n", report_just.tot_num_col);
	printf("Columns definitions:   \n");
	for (j=0; j<report_just.num_col_just; j++){
	    printf("    %2d: %2d Col:  ",j,report_just.col[j].num_col);
	    for (i=0; i<report_just.col[j].num_col; i++)
		printf("%c - '%c' ", report_just.col[j].col_just[i],
		       report_just.col[j].col_inter_space[i]);
	    printf("\n");
	    printf("          Lock:  ");
	    for (i=0; i<report_just.col[j].num_col; i++)
		printf("%c       ", report_just.col[j].col_lock[i]);
	    printf("\n");
	}
	printf("Report Width:          %d\n",report_just.width);
	printf("Before Row Separations:       ");
	for (r=0; r<report_just.tot_num_row; r++){
	    char *p;
	    for (p = report_just.before_row_separations[r]; (*p != '\0'); p++)
		printf("  %d-'%c'",r,*p);
	}
	printf("\n");
	printf("After Row Separations:       ");
	for (r=0; r<report_just.tot_num_row; r++){
	    char *p;
	    for (p = report_just.after_row_separations[r]; (*p != '\0'); p++)
		printf("  %d-'%c'",r,*p);
	}
	printf("\n");
	printf("Table Row Values:      %d Rows\n",report_just.tot_num_row);
	for (r=0; r<report_just.tot_num_row; r++){
	    printf("   %d:  (Just %d)  ",r,report_just.row_just[r]);
	    for (c=0; c<report_just.col[report_just.row_just[r]].num_col; c++){
		printf("  C%d",c);
		startc = c;
		if ((c < report_just.tot_num_col-1) && (report_just.col[report_just.row_just[r]].col_just[c+1] == 'a')){
		    printf("-");
		    c++;
		    while ((c < report_just.tot_num_col-1) && (report_just.col[report_just.row_just[r]].col_just[c] == 'a'))
			c++;
		    if (c !=  report_just.tot_num_col-1)
			c--;
		    printf("%d  ",c);
		} else
		    printf("  ");
		printf("%s  ",report_just.cell_values[startc][r]);
	    }
	    printf("\n");
	}
    }
    /* initialize the sizes to the minumum */
    for (c=0 ; c<MAX_COL; c++)
        for (c2=0 ; c2<MAX_COL; c2++)
	    report_just.max_col_sizes[c][c2]=0;
    /* first comput the max sizes for rows without spanning columns */
    for (r=0; r<report_just.tot_num_row; r++){
	for (c=0 ; c<report_just.col[report_just.row_just[r]].num_col; c++){
	    if (((c < report_just.col[report_just.row_just[r]].num_col-1) && 
		 (report_just.col[report_just.row_just[r]].col_just[c+1] != 'a')) ||
		(c == report_just.col[report_just.row_just[r]].num_col-1)){
		/* compute the max size of this column */
		p = report_just.cell_values[c][r];
		while (*p != '\0'){
		    if (isupper(report_just.col[report_just.row_just[r]].col_just[c]))
			for (x=0; (*p != '\0') && !(*p == ' ') && !(*p == '/' && *(p+1) == '/'); x++, p++)
			    ;
		    else
			for (x=0; (*p != '\0') && !(*p == '/' && *(p+1) == '/'); x++, p++)
			    ;
		    if (*p == '/' && *(p+1) == '/')
			p+=2;
		    if (*p == ' ')
			p++;
		    if (x >  report_just.max_col_sizes[report_just.col[report_just.row_just[r]].num_col][c])
			 report_just.max_col_sizes[report_just.col[report_just.row_just[r]].num_col][c] = x;
		}
	    }
	}
    }
    if (static_rpt_dbg) {
	for (j=0; j<MAX_COL; j++)
	    hit[j]=0;
	printf("   Maxlen Columns (1st Pass):  \n");
	for (j=0 ; j<report_just.num_col_just; j++){
	    if (!hit[report_just.col[j].num_col]){
		printf("      %d Col: ",report_just.col[j].num_col);	    	    
		for (c2=0; c2<report_just.col[j].num_col; c2++)
		    printf("  %2d", report_just.max_col_sizes[report_just.col[j].num_col][c2]);
		printf("\n");
 		hit[report_just.col[j].num_col]=1;
	    }
	}
	printf("\n");
    }

    /* SECOND compute the max sizes for rows WITH spanning columns */
    for (r=0; r<report_just.tot_num_row; r++){
	for (c=0 ; c<report_just.col[report_just.row_just[r]].num_col; c++){
	    /* if this isn't the last column and the next column is spanning */
	    if ((c < report_just.col[report_just.row_just[r]].num_col-1) &&
		(report_just.col[report_just.row_just[r]].col_just[c+1] == 'a')){
		int siz=0, span_siz=0, startc, c2, add;
		/* compute the max size of this column */
		startc=c;
		p = report_just.cell_values[c][r];
		while (*p != '\0'){
		    if (isupper(report_just.col[report_just.row_just[r]].col_just[c]))
			for (x=0; (*p != '\0') && !(*p == ' ') && !(*p == '/' && *(p+1) == '/'); x++, p++)
			    ;
		    else
			for (x=0; (*p != '\0') && !(*p == '/' && *(p+1) == '/'); x++, p++)
			    ;
		    if (*p == '/' && *(p+1) == '/')
			p+=2;
		    if (*p == ' ')
			p++;
		    if (x > siz)
			 siz = x;
		}
		/* compute the size of the columns spanned over */
		span_siz=0;
		while ((c < report_just.col[report_just.row_just[r]].num_col-1) && (report_just.col[report_just.row_just[r]].col_just[c+1] == 'a')){
		    span_siz +=  report_just.max_col_sizes[report_just.col[report_just.row_just[r]].num_col][c] + 
			         ((c < report_just.col[report_just.row_just[r]].num_col-1) ? 1 : 0) + space_pad*2;    
		    c++;
		}
		span_siz +=  report_just.max_col_sizes[report_just.col[report_just.row_just[r]].num_col][c];
		/* if the siz > span_size THEN redistribute the characters over the N columns */
		if (siz > span_siz) {
		    int num_unlocked=0;
		    for (c2=startc; c2<=c; c2++)
			if (report_just.col[report_just.row_just[r]].col_lock[c2] == UNLOCKED)
			    num_unlocked++;
		    if (static_rpt_dbg) printf("   Redistribute for Row %d, columns %d-%d  Unlocked Col %d ",r,startc,c,num_unlocked);
		    if (static_rpt_dbg) printf(" Span_Size %d  adjusting column size %d   Adding  ",span_siz,siz);
		    if (num_unlocked == 0){
			if (static_rpt_dbg) printf("   NONE\n");
		    } else {
			for (c2=startc; c2<=c; c2++){
			    if (report_just.col[report_just.row_just[r]].col_lock[c2] == UNLOCKED){
				if (c2 != c)
				    add = (int)((float)(siz - span_siz) / (float)(num_unlocked) + 0.5);
				else
				    add = siz - span_siz;
				if (static_rpt_dbg) printf(" %2d",add);
				report_just.max_col_sizes[report_just.col[report_just.row_just[r]].num_col][c2] += add;
				span_siz += add;
			    }
			}
			if (static_rpt_dbg) printf("\n");
		    }
		}
	    }
	}
    }
    if (static_rpt_dbg) {
	for (j=0; j<MAX_COL; j++)
	    hit[j]=0;
	printf("   Maxlen Columns (2nd Pass):  \n");
	for (j=0 ; j<report_just.num_col_just; j++){
	    if (!hit[report_just.col[j].num_col]){
		printf("      %d Col: ",report_just.col[j].num_col);	    	    
		for (c2=0; c2<report_just.col[j].num_col; c2++)
		    printf("  %2d", report_just.max_col_sizes[report_just.col[j].num_col][c2]);
		printf("\n");
 		hit[report_just.col[j].num_col]=1;
	    }
	}
	printf("\n");
    }
    report_just.width = 0;
    for (j=0; j<MAX_COL; j++)
	hit[j]=0;
    if (static_rpt_dbg) printf("   Computing Report width per justification:  \n");
    for (j=0 ; j<report_just.num_col_just; j++){
	hit[report_just.col[j].num_col] = 2;
	for (c2=0; c2<report_just.col[j].num_col; c2++)
	    hit[report_just.col[j].num_col] +=  report_just.max_col_sizes[report_just.col[j].num_col][c2] + space_pad*2;
	hit[report_just.col[j].num_col] +=  report_just.col[j].num_col - 1;
	report_just.col[j].min_just_width =  hit[report_just.col[j].num_col] ;
	if (report_just.width <  hit[report_just.col[j].num_col])
	    report_just.width = hit[report_just.col[j].num_col];
	if (static_rpt_dbg) printf("      Just %d: Col:%d   Width %d\n",j,report_just.col[j].num_col,hit[report_just.col[j].num_col]);
    }

    if (CENTER_PAGE && (report_just.width < PAGE_WIDTH))  print_spaces((PAGE_WIDTH - report_just.width)/2, fp);
    print_start_line(report_just.width,fp);
    /* produce the report */
    for (r=0; r<report_just.tot_num_row; r++){
	char *desc_column_ptr[MAX_COL], *p;
	int desc_column_size[MAX_COL];
	int desc_column_text_size[MAX_COL];
	int row_not_done, c2, c;	
	int current_just, current_num_col, current_underage, current_add, size_adjustment, current_num_unlocked;
	row_not_done = 1;

	current_just = report_just.row_just[r];
	current_num_col = report_just.col[current_just].num_col;
	current_num_unlocked = report_just.col[current_just].num_col - report_just.col[current_just].num_locked; 
	current_underage = report_just.width - report_just.col[current_just].min_just_width;

	/* first set pointers the the values and compute the column sizes and column text sizes */
	if (static_rpt_dbg) printf("   Row %d:  Just:%d  #C:%d  Adj:%d  Widths  ",r,current_just,current_num_col, current_underage);
	current_add = 0;
	for (c=0 ; c<current_num_col; c++){
	    if (report_just.col[current_just].col_just[c] != 'a'){
		if (report_just.col[current_just].col_lock[c] == LOCKED){
		    desc_column_text_size[c]=report_just.max_col_sizes[current_num_col][c];
		    desc_column_size[c]=(space_pad*2) + desc_column_text_size[c];
		} else {
		    desc_column_ptr[c] = report_just.cell_values[c][r];
		    size_adjustment = 0;
		    if (c != current_num_col-1){
			size_adjustment = (int)(((float)current_underage / (float)current_num_unlocked) *
						(float)(c + num_span_col(current_just,c) + 1)) - current_add;
			current_add += size_adjustment;
		    } else {
			size_adjustment += current_underage - current_add;
		    }
		    desc_column_text_size[c]=report_just.max_col_sizes[current_num_col][c];
		    desc_column_size[c]=(space_pad*2) + desc_column_text_size[c] + size_adjustment;
		    /* compute the size for this value */
		    for (c2=c+1; (c2 < current_num_col) && (report_just.col[current_just].col_just[c2] == 'a'); c2++){
			if (c != current_num_col-1){
			    size_adjustment = (int)(((float)current_underage / (float)current_num_col) * 
						    (float)(c + num_span_col(current_just,c) + 1)) - current_add;
			    current_add += size_adjustment;
			} else {
			    size_adjustment += current_underage - current_add;
			}
			desc_column_size[c] += report_just.max_col_sizes[current_num_col][c2] + space_pad*2 + 1 + size_adjustment;
			desc_column_text_size[c] +=  report_just.max_col_sizes[current_num_col][c2] + space_pad*2 + 1;
		    }
		}
	    } else {
		desc_column_ptr[c] = "";
		desc_column_text_size[c] = desc_column_size[c] = 0;
	    }
	    if (static_rpt_dbg) printf("   C%d:%d-%d(%d) ",c,desc_column_text_size[c], desc_column_size[c],current_add);
	}
	if (static_rpt_dbg) printf("\n");

	for (p=report_just.before_row_separations[r]; *p != '\0'; p++) { /* add a row separation */
	    if (CENTER_PAGE && (report_just.width < PAGE_WIDTH)) print_spaces((PAGE_WIDTH - report_just.width)/2, fp);
	    fprintf(fp,"|");
	    for (c=0 ; c<current_num_col; c++){
		if (report_just.col[current_just].col_just[c] != 'a'){
		    for (x=0; x< desc_column_size[c]; x++)
			fprintf(fp,"%c",*p);
		    if (! is_last_just_col(current_just,c)){
			int sp;
			sp =  num_span_col(current_just,c);
			if ( report_just.col[current_just].col_inter_space[sp + c] != ' '){
			    if (*p == ' ')
				fprintf(fp,"%c", report_just.col[current_just].col_inter_space[sp + c]);
			    else if ((report_just.col[current_just].col_inter_space[sp + c] == '|') && (*p == '-') &&
				     ((r > 0) && (report_just.col[report_just.row_just[r-1]].col_inter_space[sp + c] == '|')))
				fprintf(fp,"+");
			    else 
				fprintf(fp,"%c", *p);
			} else	
			    fprintf(fp,"%c", *p);
		    }
		}
	    }
	    fprintf(fp,"|\n");	    
	}

	while (row_not_done){
	    row_not_done = 0;
	    if (CENTER_PAGE && (report_just.width < PAGE_WIDTH)) print_spaces((PAGE_WIDTH - report_just.width)/2, fp);
	    fprintf(fp,"|");
	    for (c=0 ; c< current_num_col; c++){
		column_width=desc_column_size[c];
		text_width=desc_column_text_size[c];
		if (report_just.col[current_just].col_just[c] != 'a'){
		    switch (report_just.col[current_just].col_just[c]){
		      case 'c':
		      case 'C':
			*fmt_str1='\0';
			sprintf(fmt_str1,"%s%%%ds%s", spaces+SPACES_LEN-((column_width-text_width)/2),text_width,
				spaces+SPACES_LEN-(column_width-text_width - ((column_width-text_width)/2)));
			fprintf(fp,fmt_str1,
				center(get_next_string_value(&(desc_column_ptr[c]),text_width),text_width));
			if (*desc_column_ptr[c] != '\0')
			    row_not_done = 1;
			break;
		      case 'r':
		      case 'R':
			*fmt_str1='\0';
			sprintf(fmt_str1,"%s%%%ds%s",spaces+SPACES_LEN-((column_width-text_width)/2),text_width,
				spaces+SPACES_LEN-(column_width-text_width - ((column_width-text_width)/2)));
			fprintf(fp,fmt_str1,get_next_string_value(&(desc_column_ptr[c]),text_width));
			if (*desc_column_ptr[c] != '\0')
			    row_not_done = 1;
			break;
		      case 'l':
		      case 'L':
			*fmt_str1='\0';
			sprintf(fmt_str1,"%s%%-%ds%s",spaces+SPACES_LEN-((column_width-text_width)/2),text_width,
				spaces+SPACES_LEN-(column_width-text_width - ((column_width-text_width)/2)));
			fprintf(fp,fmt_str1,get_next_string_value(&(desc_column_ptr[c]),text_width));
			if (*desc_column_ptr[c] != '\0')
			    row_not_done = 1;
			break;
		      default:
			fprintf(fp,"undefined inter column space\n");
		    }
		    if (! is_last_just_col(current_just,c)){
			int sp;
			sp =  num_span_col(current_just,c);
			if ( report_just.col[current_just].col_inter_space[sp + c] != ' ')
			    fprintf(fp,"%c", report_just.col[current_just].col_inter_space[sp + c]);
			else
			    fprintf(fp," ");
		    }
		}
	    }
	    fprintf(fp,"|\n");
	}
	for (p=report_just.after_row_separations[r]; *p != '\0'; p++) { /* add a row separation */	
	    if (CENTER_PAGE && (report_just.width < PAGE_WIDTH)) print_spaces((PAGE_WIDTH - report_just.width)/2, fp);
	    fprintf(fp,"|");
	    for (c=0 ; c<current_num_col; c++){
		if (report_just.col[current_just].col_just[c] != 'a'){
		    for (x=0; x< desc_column_size[c]; x++)
			fprintf(fp,"%c",*p);
		    if (! is_last_just_col(current_just,c)){
			int sp;
			sp =  num_span_col(current_just,c);
			if ( report_just.col[current_just].col_inter_space[sp + c] != ' '){
			    if (*p == ' ')
				fprintf(fp,"%c", report_just.col[current_just].col_inter_space[sp + c]);
			    else if ((report_just.col[current_just].col_inter_space[c] == '|') && (*p == '-') &&
				     ((r > 0) && (report_just.col[report_just.row_just[r-1]].col_inter_space[sp + c] == '|')))
				fprintf(fp,"+");
			    else 
				fprintf(fp,"%c", *p);
			} else	
			    fprintf(fp,"%c", *p);
		    }
		}
	    }
	    fprintf(fp,"|\n");	    
	}
    }
    if (CENTER_PAGE && (report_just.width < PAGE_WIDTH)) print_spaces((PAGE_WIDTH - report_just.width)/2, fp);
    print_final_line(report_just.width,fp);
}

int is_last_just_col(int just, int col)
{
    int c;
    if (col == report_just.col[just].num_col-1)
	return(1);
    c = col+1;
    while ((c < report_just.col[just].num_col) && ( report_just.col[just].col_just[c] == 'a'))
	c++; 
    if ((c == report_just.col[just].num_col))
	return(1);
    return(0);	   
}

int is_last_span_col(int just, int col)
{
    if ((col+1 < report_just.col[just].num_col) && ( report_just.col[just].col_just[col] == 'a'))
	return(0); 
    return(1);
}

void print_line(int wid, FILE *fp)
{
    int i;
    fprintf(fp,"|");
    for (i=0; i<wid-2; i++)
	fprintf(fp,"-");
    fprintf(fp,"|\n");
}

void print_blank_line(int wid, FILE *fp)
{
    int i;
    fprintf(fp,"|");
    for (i=0; i<wid-2; i++)
	fprintf(fp," ");
    fprintf(fp,"|\n");
}

void print_double_line(int wid, FILE *fp)
{
    int i;
    fprintf(fp,"|");
    for (i=0; i<wid-2; i++)
	fprintf(fp,"=");
    fprintf(fp,"|\n");
}

void print_start_line(int wid, FILE *fp)
{
    int i;
    fprintf(fp,",");
    for (i=0; i<wid-2; i++)
	fprintf(fp,"-");
    fprintf(fp,".\n");
}

void print_final_line(int wid, FILE *fp)
{
    int i;
    fprintf(fp,"`");
    for (i=0; i<wid-2; i++)
	fprintf(fp,"-");
    fprintf(fp,"'\n");
}

void Desc_add_row_separation(char chr, int row_attach)
{
    char *p;

    if (row_attach == BEFORE_ROW){
	for (p = report_just.before_row_separations[report_just.tot_num_row];
	     (*p != '\0') && (p < report_just.before_row_separations[report_just.tot_num_row] + MAX_SEP); p++)
	    ;
	if (p == report_just.before_row_separations[report_just.tot_num_row] + MAX_SEP){
	    fprintf(stderr,"Error: Too many separations for row %d\n",report_just.tot_num_row);
	    exit(-1);
	}
	*p = chr; *(p+1) = '\0';
    } else {
	for (p = report_just.after_row_separations[report_just.tot_num_row];
	     (*p != '\0') && (p < report_just.after_row_separations[report_just.tot_num_row] + MAX_SEP); p++)
	    ;
	if (p == report_just.after_row_separations[report_just.tot_num_row] + MAX_SEP){
	    fprintf(stderr,"Error: Too many separations for row %d\n",report_just.tot_num_row);
	    exit(-1);
	}
	*p = chr; *(p+1) = '\0';
    }
}

int num_span_col(int just, int col)
{
    int c, span=0;
    if (col == report_just.col[just].num_col-1)
	return(0);
    c = col+1;
    while ((c < report_just.col[just].num_col) && ( report_just.col[just].col_just[c] == 'a')){
	c++; 
	span++;
    }
    return(span);
}

static int iter_num_val=0;
static char iter_val[MAX_COL][MAX_VALUE_LEN];
static char iter_fmt[MAX_VALUE_LEN];

void Desc_set_iterated_format(char *format)
{
    strcpy(iter_fmt,format);
}

void Desc_set_iterated_value(char *str)
{
    strcpy(iter_val[iter_num_val++],str);
}

void Desc_flush_iterated_row(void)
{
    switch (iter_num_val){
      case 1: Desc_add_row_values(iter_fmt,iter_val[0]); 
	break;
      case 2: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1]); 
	break;
      case 3: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2]); 
	break;
      case 4: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3]); 
	break;
      case 5: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4]); 
	break;
      case 6: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5]); 
	break;
      case 7: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6]); 
	break;
      case 8: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7]); 
	break;
      case 9: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8]); 
	break;
      case 10: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9]); 
	break;
      case 11: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10]); 
	break;
      case 12: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10],iter_val[11]); 
	break;
      case 13: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10],iter_val[11],iter_val[12]); 
	break;
      case 14: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10],iter_val[11],iter_val[12],iter_val[13]); 
	break;
      case 15: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10],iter_val[11],iter_val[12],iter_val[13],iter_val[14]); 
	break;
      case 16: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10],iter_val[11],iter_val[12],iter_val[13],iter_val[14],iter_val[15]); 
	break;
      case 17: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10],iter_val[11],iter_val[12],iter_val[13],iter_val[14],iter_val[15],iter_val[16]); 
	break;
      case 18: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10],iter_val[11],iter_val[12],iter_val[13],iter_val[14],iter_val[15],iter_val[16],iter_val[17]); 
	break;
      case 19: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10],iter_val[11],iter_val[12],iter_val[13],iter_val[14],iter_val[15],iter_val[16],iter_val[17],iter_val[18]); 
	break;
      case 20: Desc_add_row_values(iter_fmt,iter_val[0],iter_val[1],iter_val[2],iter_val[3],iter_val[4],iter_val[5],iter_val[6],iter_val[7],iter_val[8],iter_val[9],iter_val[10],iter_val[11],iter_val[12],iter_val[13],iter_val[14],iter_val[15],iter_val[16],iter_val[17],iter_val[18],iter_val[19]); 
	break;
    }
    iter_num_val=0;
}



#ifdef __STDC__
void Desc_add_row_values(char *format , ...)
#else
void Desc_add_row_values(va_alist)
va_dcl
#endif
{
    va_list args;
    char *str;
    int col=0;
#ifndef __STDC__    
    char *format;
#endif

#ifdef __STDC__    
    va_start(args,format);
#else
    va_start(args);
    format = va_arg(args,char *);
#endif

    if (report_just.tot_num_row >= MAX_ROW){
	fprintf(stderr,"Error: Too many rows > %d\n",MAX_ROW);
	exit(-1);
    }

    if (static_parse_dbg) printf("Desc_add_row_values\n");
    report_just.row_just[report_just.tot_num_row] = 
	Desc_set_justification(format);
    if (static_parse_dbg) 
	printf("    Format %s   id %d\n",format,
	       report_just.row_just[report_just.tot_num_row]);
    for (col=0; col<report_just.col[report_just.row_just[report_just.tot_num_row]].num_col; col++){
	if (report_just.col[report_just.row_just[report_just.tot_num_row]].col_just[col] != 'a'){
	    str = va_arg(args, char *);
	    strcpy(report_just.cell_values[col][report_just.tot_num_row],str);
	    if (static_parse_dbg) printf("    value %d %s\n",col,str);
	}
    }
    va_end(args);
    report_just.tot_num_row ++;
}


int Desc_dump_ascii_report(char *file)
{
    int c, r;
    char *p;
    FILE *fp;

    if (strcmp(file,"") == 0)
	return(1);
    if ((fp=fopen(file,"w")) == (FILE *)NULL){
	fprintf(stderr,"Error: unable to open ascii report file '%s'\n",file);
	return(1);
    }
    
    if (CENTER_PAGE)
	fprintf(fp,"center_page %d\n",report_just.width);
    for (r=0; r<report_just.tot_num_row; r++){
	for (p = report_just.before_row_separations[r]; (*p != '\0'); p++)
	    fprintf(fp,"separate \"%c\" BEFORE_ROW\n",*p);
	for (p = report_just.after_row_separations[r]; (*p != '\0'); p++)
	    fprintf(fp,"separate \"%c\" AFTER_ROW\n",*p);
	fprintf(fp,"row \"%s\"",report_just.col[report_just.row_just[r]].format_str);
	for (c=0; c<report_just.col[report_just.row_just[r]].num_col; c++)
	    if (report_just.col[report_just.row_just[r]].col_just[c] != 'a')
		fprintf(fp," \"%s\"",report_just.cell_values[c][r]);
        fprintf(fp,"\n");
    }
    fclose(fp);
    return(0);
}
