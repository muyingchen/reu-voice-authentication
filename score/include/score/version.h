/************************************************/
/*						*/
/*  file: version.h				*/
/*  desc: header file to define the release	*/
/*        version number			*/
/*  created: Jan 14, 1990			*/
/************************************************/

#define VERSION_ID "3.5"

#define print_version(_exe_name) \
   printf("Program %s, Version %s\n",_exe_name,VERSION_ID);
