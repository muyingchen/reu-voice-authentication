/**********************************************************************/
/*                                                                    */
/*           FILE: 2_samp_z.c                                         */
/*           WRITTEN BY: Brett                                        */
/*           DATE: April 14 1989                                      */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/**********************************************************************/

#include <phone/stdcenvf.h>
#include "defs.h"

#define MAX_NUM_SPKRS 200
#define MAX_NUM_SYS 20
#define NEG_Z_NUM -1.96
#define Z_NUM 1.96

int get_full_sys_name(char *name, int sys_num, CONFIG *cfg)
{
  int i=0,len;
  char **chr, *c;
  while ((*cfg->rec_list[i].intern_id != NULL_CHAR) &&
	 (strcmp(cfg->rec_list[i].intern_id,"ALIGN") != 0))
    ++i;
  if (strcmp(cfg->rec_list[i].intern_id,"ALIGN") == 0)
    {
      chr = (char **)cfg->rec_list[i].value;
      strcpy(name,chr[sys_num]);
      len = strlen(name);
      c = &name[len-3];
      if (strcmp(c,"ali") == 0)
	{
	  *c++ = 's';
	  *c++ = 'c';
	  *c++ = 'r';
	}
      else
	{
	  fprintf(stderr,"Error: Unexpected filename extension: %s\n",c);
	  return(-1);
	}
      return(0);
    }
  else
    {
      fprintf(stderr,"Error: Could not find \"ALIGN\" field in the CONFIG struct\n");
      return(-1);
    }
}

void do_speaker_stat_analysis(SYS_ALIGN_LIST *sa_list, CONFIG *cfg, int **w_arr, int **s_arr, int *ns)
{
  int i,k,num_spkrs;
  float tmp1, tmp2, l1[MAX_NUM_SPKRS], l2[MAX_NUM_SPKRS];
  float mean,std,var,Z_stat;
  char scorefile[80];
  FILE *fp;
  for (i=0; i < num_sys(sa_list); i++)
    {
      get_full_sys_name(scorefile,i,cfg);
      fp=fopen(scorefile,"r");
      if (fp == NULL)
	fprintf(stderr, "Error: Could not open scorefile \'%s\'\n",scorefile);
      num_spkrs=0;
      while (fscanf(fp,"%f %f\n",&tmp1,&tmp2) != EOF)
	{
	  l1[num_spkrs]=tmp1;
	  l2[num_spkrs]=tmp2;
	  if (num_spkrs > MAX_NUM_SPKRS)
	    fprintf(stderr, "Error: Too many speakers\n");
	  else
	    ++num_spkrs;
	}
      fclose(fp);
      *ns=num_spkrs;
      calc_mean_var_std_dev_Zstat_float(l1,num_spkrs,&mean,&var,&std,&Z_stat);
      for (k=0; k < num_spkrs; k++)
	{
	  if (l1[k] < (mean-(2*std)))
	    w_arr[k][i]=(TEST_DIFF * (-1));
	  else
	    if (l1[k] > (mean+(2*std)))
	      w_arr[k][i]=TEST_DIFF;
	    else
	      w_arr[k][i]=NO_DIFF;
	}
      calc_mean_var_std_dev_Zstat_float(l2,num_spkrs,&mean,&var,&std,&Z_stat);
      for (k=0; k < num_spkrs; k++)
	{
	  if (l2[k] < (mean-(2*std)))
	    s_arr[k][i]=(TEST_DIFF * (-1));
	  else
	    if (l2[k] > (mean+(2*std)))
	      s_arr[k][i]=TEST_DIFF;
	    else
	      s_arr[k][i]=NO_DIFF;
	}
    }
}

void do_2_samp_z(SYS_ALIGN_LIST *sa_list, CONFIG *cfg, int **w_winner, int **s_winner)
{
  int i,j,num_spkr1,num_spkr2;
  float l1[MAX_NUM_SPKRS],l2[MAX_NUM_SPKRS],l3[MAX_NUM_SPKRS],l4[MAX_NUM_SPKRS];
  float tmp1,tmp2,w_Z,s_Z;
  char scorefile[80];
  FILE *fp;
  for (i=0; i < num_sys(sa_list); i++)
    {
      get_full_sys_name(scorefile,i,cfg);
      fp=fopen(scorefile,"r");
      if (fp == NULL)
	fprintf(stderr, "Error: Could not open scorefile \'%s\'\n",scorefile);
      num_spkr1=0;
      while (fscanf(fp,"%f %f\n",&tmp1,&tmp2) != EOF)
	{
	  l1[num_spkr1]=tmp1;
	  l2[num_spkr1]=tmp2;
	  if (num_spkr1 > MAX_NUM_SPKRS)
	    fprintf(stderr, "Error: Too many speakers\n");
	  else
	    ++num_spkr1;
	}
      fclose(fp);
      for (j=i+1; j < num_sys(sa_list); j++)
	{
	  get_full_sys_name(scorefile,j,cfg);
	  fp=fopen(scorefile,"r");
	  if (fp == NULL)
	    fprintf(stderr, "Error: Could not open scorefile \'%s\'\n",scorefile);
	  num_spkr2=0;
	  while (fscanf(fp,"%f %f\n",&tmp1,&tmp2) != EOF)
	    {
	      l3[num_spkr2]=tmp1;
	      l4[num_spkr2]=tmp2;
	      if (num_spkr2 > MAX_NUM_SPKRS)
		fprintf(stderr, "Error: Too many speakers\n");
	      else
		++num_spkr2;
	    }
	  fclose(fp);
	  calc_two_sample_z_test_float(l1,l3,num_spkr1,num_spkr2,&w_Z);
	  calc_two_sample_z_test_float(l2,l4,num_spkr2,num_spkr2,&s_Z);
	  if (w_Z < NEG_Z_NUM)
	    w_winner[i][j]=(TEST_DIFF * (-1));
	  else
	    if (w_Z > Z_NUM)
	      w_winner[i][j]=TEST_DIFF;
	    else
	      w_winner[i][j]=NO_DIFF;
	  if (s_Z < NEG_Z_NUM)
	    s_winner[i][j]=(TEST_DIFF * (-1));
	  else
	    if (s_Z > Z_NUM)
	      s_winner[i][j]=TEST_DIFF;
	    else
	      s_winner[i][j]=NO_DIFF;
	}
    }
}



