/* bias.c */
/* This program removes the dc bias of a .wav file.  */
/* Change: Nov. 12, 1992 By Jon Fiscus               */
/*        If an output file is specified, the        */
/*        header fields for sample_min and sample_max*/
/*        are updated.                               */
/*     Note: Since there hasn't been a standards     */
/*           for multichannel files, results may be  */
/*           unpredictable.                          */

#include <snr/snrlib.h>

#define BLOCKSIZE 2048
#define CLIP(x) ((x) < -32767) ? -32767 : (((x) > 32767) ? 32767 : (x))
#define TAPER_LENGTH 3200

/* bias.c */	void print_help_and_exit  PROTO((void));

int main (int argc, char **argv)
{
    SP_FILE *in_file,*out_file;
    char *proc = "bias";
    short buffer[BLOCKSIZE];
    int newval;
    SPFILE_CHARS *sp_char;
    double *sum,*num_samples,*offset; /* arrays */
    int i,channel,read,k;
    char c,*in_file_name,*out_file_name;
    int taper=0,samples_read;
    
    if ((argc < 2) || (argc > 4))
	print_help_and_exit();
    
    in_file_name = argv[1];
    if (argc>=3) out_file_name = argv[2];

    for (k=1; k<argc; k++) 
	if (argv[k][0] == '-')
	    while ((c = *++argv[k])) {
		switch (c) {
		  case 'h':
		    print_help_and_exit();
		  case 't':
		    taper=1;
		    break;
		  default:
		    printf("I don't know option -%c. I only know -ht. Try -h for help.\n",c);
		    exit(-1);
		}
	    }
    
    if ((in_file = sp_open(in_file_name,"r")) == SPNULL) {
	printf("I can't find %s.\n\n",in_file_name);
	return -1;
    }
    sp_char = (SPFILE_CHARS *) malloc (sizeof(SPFILE_CHARS));

    load_sp_char(in_file,sp_char);
    if (strstr(sp_char->sample_coding,"pcm") == (char *)0 &&
	strstr(sp_char->sample_coding,"linear") == (char *)0){
	fprintf(stderr,"%s: Unable to De-bias a file encoded as '%s'\n",
		proc,sp_char->sample_coding);
	exit(-1);
    }
    
    sum = (double *) malloc (sp_char->channel_count * sizeof(double));
    num_samples = (double *) malloc (sp_char->channel_count * sizeof(double));
    offset = (double *) malloc (sp_char->channel_count * sizeof(double));

    for (channel=0; channel < sp_char->channel_count; channel++) {
	sum[channel] = num_samples[channel] = 0.0;
	if (sp_rewind(in_file) > 0)
	    fprintf(spfp,"Warning: sp_rewind failed\n");
	if (sp_set_data_mode(in_file,rsprintf("CH-%d",channel+1)) != 0){
	    fprintf(spfp,"bias: Error, sp_set_data_mode failed\n");
	    sp_print_return_status(spfp);
	    exit(-1);
	}
	
	while ((read = sp_read_data((char*)buffer,BLOCKSIZE,in_file)) != 0) {
	    num_samples[channel] += (double) read;
	    for (i=0; i<read; i++)
		sum[channel] += (double) buffer[i];
	}
	offset[channel] = sum[channel]/num_samples[channel];
	printf("DC offset = %4.2f.\n",offset[channel]);
    }
    
    if (argc >= 3) { /* if output file is specified then correct bias */
	SP_INTEGER value;
	char *fld, str[100];

	if (sp_rewind(in_file) > 0)
	    fprintf(spfp,"Warning: sp_rewind failed\n");

	if ((out_file = sp_open(out_file_name,"w")) == SPNULL){
	    fprintf(stderr,"%s: Unable to open output file <%s>\n",
		    proc,out_file_name);
	    exit(-1);
	}
	/* Reset the channel count fields */
	strcpy(str,"CH-1");
	for (channel=1; channel < sp_char->channel_count; channel++)
	    strcat(str,rsprintf(",%d",channel+1));
	if (sp_set_data_mode(in_file,str) != 0){
	    fprintf(spfp,"bias: Error, sp_set_data_mode failed\n");
	    sp_print_return_status(spfp);
	    goto ERROR;
	}

	if (sp_copy_header(in_file,out_file) != 0){
	    fprintf(stderr,"%s: Unable to copy header from original\n",proc);
	    goto ERROR;
	}	

	if (sp_h_delete_field(out_file,"sample_checksum") > 100){
	    fprintf(stderr,
		    "%s: Can't delete sample_checksum\n",proc);
	    goto ERROR;
	}

	/* Reset the sample min and max if they exist */
	fld = "sample_max";
	if (sp_h_get_field(in_file,fld,T_INTEGER,(void *)&value)==0){
	    value -= (SP_INTEGER)offset[0];
	    if (sp_h_set_field(out_file,fld,T_INTEGER,(void *)&value) != 0)
		fprintf(stderr,"%s: Warning: Re-set of %s failed\n",proc,fld);
	}
	fld = "sample_min";
	if (sp_h_get_field(in_file,fld,T_INTEGER,(void *)&value)==0){
	    value -= (SP_INTEGER)offset[0];
	    if (sp_h_set_field(out_file,fld,T_INTEGER,(void *)&value) != 0)
		fprintf(stderr,"%s: Warning: Re-set of %s failed\n",proc,fld);
	}
	
	samples_read=0;
	while ((read = sp_read_data((char *)buffer,
				    BLOCKSIZE/sp_char->channel_count,
				    in_file))!=0){
	    for (i=0;i<read;i++) {
		samples_read++;
		/* lop off DC offset */
		for (channel=0; channel < sp_char->channel_count; channel++) {
		    int ind = (sp_char->channel_count * i) + channel;
		    newval = buffer[ind] - (short)offset[channel]; 

		    if (taper && (samples_read < TAPER_LENGTH))
			newval = (short) ((double) newval *
					  (double) samples_read /
					  (double) (TAPER_LENGTH-1));
		    buffer[ind] = CLIP(newval); 
		}
	    }
	    if (sp_write_data((void *)buffer,read,out_file) != read){
		fprintf(stderr,"%s: Error, sp_write_data failed\n",proc);
		goto ERROR;
	    }

	}
	sp_close(out_file);
	return (0);
    }
    return(0);
  ERROR:
    sp_close(out_file);
    unlink(out_file_name);
    return(-1);
}



void print_help_and_exit (void)
{
  printf("Usage: bias [-h] <input file> [output file] [-t]\n\n");
  printf("  -h : prints this message.\n");
  printf("  -t : tapers the beginning of the file.\n");
  printf("\n");
  printf("Measures and corrects dc bias in a .wav file.\n");
  printf("If <output file> is not specified, then the DC bias is\n");
  printf("measured but not corrected. The -t flag specifies that the\n");
  printf("beginning of the output file should be faded in over the\n");
  printf("first 80ms in order to avoid the popping artifact that\n");
  printf("results from step discontinuities at the beginning of\n");
  printf("files.\n");
  exit(-1);
}



    
  



  
	
      
