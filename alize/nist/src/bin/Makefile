# **********************************************************
# Project:		ARG General libraries
# SubTree:		/Users/muyingchen/desktop/voice-authentication/alize/nist/src/progs
# Filename:		Makefile
# Programmer:		J. Fiscus
# Organization:		NIST/NCSL/DIV 670/Auto Rec. Group
# Host System:		SUN 4 OS/4.1.1 (UNIX)
# Date Created:		05/28/92
# Revision history:
#
# Apr 04, 1994
#	- fixes the make 'depend' and 'makefile' commands to not fail
# 	  on the SGI
#	- fixed the problem with an executable not being remade if it's
#	  source file had changed.
#
#
#
# **********************************************************
# Makefile for SRC Binary Directory
# **********************************************************
SHELL = /bin/sh

PROJECT_ROOT	= /Users/muyingchen/desktop/voice-authentication/alize/nist

# **************************************************
# INDIVIDUAL PROGRAM MODIFICATIONS SHOULD BEGIN HERE
# **************************************************

# Program to make (binary name)
EXECUTABLE	= atismd autohum bias clp h_add h_delete h_edit h_read h_strip h_test headcheck segsnr speech stnr tsphere w_decode w_diff w_edit w_encode wavmd 
# Supporting functions to make if any (They will be included in every compile)
OBJ		= 
# List all .c source code files
SRC		= atismd.c autohum.c bias.c clp.c h_add.c h_delete.c h_edit.c h_read.c h_strip.c h_test.c headcheck.c segsnr.c speech.c stnr.c tsphere.c w_decode.c w_diff.c w_edit.c w_encode.c wavmd.c 
# Enable converting the distribution to K&R C
# KRC             = $(SRC:%.c=%.kr.c)

# Subdirectorys to make 
SUBS		= atismd.dSYM atismd.dSYM/Contents atismd.dSYM/Contents/Resources atismd.dSYM/Contents/Resources/DWARF autohum.dSYM autohum.dSYM/Contents autohum.dSYM/Contents/Resources autohum.dSYM/Contents/Resources/DWARF bias.dSYM bias.dSYM/Contents bias.dSYM/Contents/Resources bias.dSYM/Contents/Resources/DWARF clp.dSYM clp.dSYM/Contents clp.dSYM/Contents/Resources clp.dSYM/Contents/Resources/DWARF h_add.dSYM h_add.dSYM/Contents h_add.dSYM/Contents/Resources h_add.dSYM/Contents/Resources/DWARF h_delete.dSYM h_delete.dSYM/Contents h_delete.dSYM/Contents/Resources h_delete.dSYM/Contents/Resources/DWARF h_edit.dSYM h_edit.dSYM/Contents h_edit.dSYM/Contents/Resources h_edit.dSYM/Contents/Resources/DWARF h_read.dSYM h_read.dSYM/Contents h_read.dSYM/Contents/Resources h_read.dSYM/Contents/Resources/DWARF h_strip.dSYM h_strip.dSYM/Contents h_strip.dSYM/Contents/Resources h_strip.dSYM/Contents/Resources/DWARF h_test.dSYM h_test.dSYM/Contents h_test.dSYM/Contents/Resources h_test.dSYM/Contents/Resources/DWARF headcheck.dSYM headcheck.dSYM/Contents headcheck.dSYM/Contents/Resources headcheck.dSYM/Contents/Resources/DWARF segsnr.dSYM segsnr.dSYM/Contents segsnr.dSYM/Contents/Resources segsnr.dSYM/Contents/Resources/DWARF speech.dSYM speech.dSYM/Contents speech.dSYM/Contents/Resources speech.dSYM/Contents/Resources/DWARF stnr.dSYM stnr.dSYM/Contents stnr.dSYM/Contents/Resources stnr.dSYM/Contents/Resources/DWARF tsphere.dSYM tsphere.dSYM/Contents tsphere.dSYM/Contents/Resources tsphere.dSYM/Contents/Resources/DWARF w_decode.dSYM w_decode.dSYM/Contents w_decode.dSYM/Contents/Resources w_decode.dSYM/Contents/Resources/DWARF w_diff.dSYM w_diff.dSYM/Contents w_diff.dSYM/Contents/Resources w_diff.dSYM/Contents/Resources/DWARF w_edit.dSYM w_edit.dSYM/Contents w_edit.dSYM/Contents/Resources w_edit.dSYM/Contents/Resources/DWARF w_encode.dSYM w_encode.dSYM/Contents w_encode.dSYM/Contents/Resources w_encode.dSYM/Contents/Resources/DWARF wavmd.dSYM wavmd.dSYM/Contents wavmd.dSYM/Contents/Resources wavmd.dSYM/Contents/Resources/DWARF 
# LIBS needed to compile the EXECUTABLE
# (Full paths eg: /usr/local/image/lib/libimage.a)
# use $(LIBDIR) defined above for path extensions if appropriate
LIBS	= $(PROJECT_ROOT)/lib
# How the libraries look when invoked on the compile line (eg: -limage)
LLIBS	= sphereCombinedLibs -lm
# Local additions for CFLAG options (eg: -g)
LOCAL_CFLAGS	= -g -g -DNARCH_LINUX

# *******************************************************
# THE REST OF THE MAKEFILE SHOULD NOT NEED TO BE MODIFIED
#       (EXCEPT UPON APPROVAL OF PROJECT MANAGER)
# *******************************************************

BIN	= $(PROJECT_ROOT)/bin
INCLUDE = $(PROJECT_ROOT)/include
LIBDIR	= $(LIBS)

CFLAGS	= -I$(INCLUDE) -L$(LIBDIR) $(LOCAL_CFLAGS)
CC	= gcc -m32
INSTALL	= install -s -m 755

MAKEFILE	= Makefile
.PRECIOUS: $(MAKEFILE)

#.c: $(BIN)/$@ stub.o
#	$(CC) $@.c $(FUNCT_O) $(LLIBS) -o $@
#
#.c.o:
#	cc -c $@.c

it: $(EXECUTABLE) $(SUBS)
	@-X=`pwd`; \
        for i in $(SUBS) ; do \
		echo '<<' $$i '>>'; \
		cd $$X/$$i; make $@ PROJDIR=$(PROJDIR); \
	done

install: $(MAKEFILE) $(EXECUTABLE)
	$(INSTALL) $(EXECUTABLE) $(BIN)
	@-X=`pwd`; \
        for i in $(SUBS) ; \
		do echo '<<' $$i '>>'; \
		cd $$X/$$i; make $@ PROJDIR=$(PROJDIR); \
	done

#$(EXECUTABLE): $$@.c
.c:
	$(CC) $(CFLAGS) $@.c $(OBJ) $(LLIBS) -o $@

#build_kr: $(KRC)
#	@-X=`pwd`; \
#        for i in $(SUBS) ; \
#		do echo '<<' $$i '>>'; \
#		cd $$X/$$i; make $@ PROJDIR=$(PROJDIR); \
#	done
#
#$(KRC): $@
#	cp `echo $@ | sed 's/.kr//'` $@
#	unprotoize -c "-I$(INCLUDE)" $@
#	rm -f $@.save

# if there are other separate programs to compile, add the name to
#	SRC, OBJ, ... and the redo the last 3 lines EXPLICITLY
#	for each program.
#	Install will also have to be changed.

clean :
	rm -f *.o $(EXECUTABLE) core a.out *.BAK
	@-X=`pwd`; \
	for i in `echo $(SUBS)` ; do \
		echo '<<' $$i '>>'; cd $$X/$$i; make $@ PROJDIR=$(PROJDIR); \
	done;

bare: clean
	rm -f $(EXECUTABLE)
	@-X=`pwd`; \
	for i in `echo $(SUBS)` ; do \
		echo '<<' $$i '>>'; cd $$X/$$i; make $@ PROJDIR=$(PROJDIR); \
	done;

depend $(MAKEFILE): $(SRC)
	$(CC) $(CFLAGS) -M $(SRC) > dependlist
	@sed -e '1,/^# DO NOT DELETE/!d' $(MAKEFILE) > $(MAKEFILE).tmp.$$$$; \
	cat dependlist >> $(MAKEFILE).tmp.$$$$; \
	cp $(MAKEFILE) $(MAKEFILE).BAK; \
	mv $(MAKEFILE).tmp.$$$$ $(MAKEFILE); \
	rm -f dependlist; \
	X=`pwd`; \
        if test -n "$(SUBS)" ; then for i in $(SUBS) ; do echo '<<' $$i '>>';\
	cd $$X/$$i; make $@ PROJDIR=$(PROJDIR); done ; fi;

# DO NOT DELETE THIS LINE - make depend uses it
