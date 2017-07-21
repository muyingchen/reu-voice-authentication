# **********************************************************
# Project:		ARG General libraries
# SubTree:		/Users/muyingchen/desktop/voice-authentication/alize/nist/src/lib/ snr
# Filename:		Makefile
# Programmer:		J. Fiscus
# Organization:		NIST/NCSL/DIV 670/Auto Rec. Group
# Host System:		SUN 4 OS/4.1.1 (UNIX)
# Date Created:		05/28/92
# Project:		NIST LIBRARIES
# Revision history:
#
# Apr 04, 1994
#	- fixes the make 'depend' and 'makefile' commands to not fail
# 	  on the SGI
#
# **********************************************************
# Makefile for SRC Library Directory
# **********************************************************
SHELL = /bin/sh

# PROJDIR Passed On Command Line ONLY
PROJECT_ROOT	= /Users/muyingchen/desktop/voice-authentication/alize/nist
LIBDIR		= $(PROJECT_ROOT)/lib
DOCDIR		= $(PROJECT_ROOT)/doc

RANLIB		= ranlib
AR		= ar ru

# **************************************************
# INDIVIDUAL PROGRAM MODIFICATIONS SHOULD BEGIN HERE
# **************************************************

# Library to make (library name)
LIBRARY_ID = snr
LIBRARY	= lib$(LIBRARY_ID).a
# Source files for LIBRARY
SRC	= complex.c fft.c filter.c gspfchar.c power.c remez.c segment.c snr.c speech_det.c wav_io.c 

# Local additions for CFLAG options (eg: -g)
LOCAL_CFLAGS	= -g -DNARCH_LINUX

# *******************************************************
# THE REST OF THE MAKEFILE SHOULD NOT NEED TO BE MODIFIED
#       (EXCEPT UPON APPROVAL OF PROJECT MANAGER)
# *******************************************************

INCLUDE = $(PROJECT_ROOT)/include
OBJ	= complex.o fft.o filter.o gspfchar.o power.o remez.o segment.o snr.o speech_det.o wav_io.o 
# Enable converting the distribution to K&R C
#KRC     = $(SRC:%.c=%.kr.c)
LIBRY	= $(LIBRARY)

CFLAGS	= -I$(INCLUDE) $(LOCAL_CFLAGS)
CC	= gcc -m32 $(CFLAGS)

MAKEFILE	= Makefile
.PRECIOUS: $(MAKEFILE)

it: $(LIBRY)

install: $(MAKEFILE) $(LIBDIR)/$(LIBRARY)
$(LIBDIR)/$(LIBRARY): $(LIBRY)
	mv $(LIBRY) $(LIBDIR)
	chmod 664 $(LIBDIR)/$(LIBRARY)
	$(RANLIB) $(LIBDIR)/$(LIBRARY)

$(LIBRY): $(OBJ)
	$(AR) $(LIBRY) $?
	$(RANLIB) $(LIBRY)

#build_kr: $(KRC)
#$(KRC): $@
#	cp `echo $@ | sed 's/.kr//'` $@
#	unprotoize -c "-I$(INCLUDE)" $@
#	rm -f $@.save

%.o: %.c
	$(CC) -c -o $@ $<

clean :
	rm -f $(OBJ) $(LIBRARY) $(LIBRARY_ID).doc *.BAK dependlist;

bare: clean
	rm -f $(LIBRY)

depend $(MAKEFILE): $(SRC)
	$(CC) -M $(SRC) > dependlist
	@sed -e '1,/^# DO NOT DELETE/!d' $(MAKEFILE) > $(MAKEFILE).tmp.$$$$; \
	cat dependlist >> $(MAKEFILE).tmp.$$$$; \
	cp $(MAKEFILE) $(MAKEFILE).BAK; \
	mv $(MAKEFILE).tmp.$$$$ $(MAKEFILE); \
	rm -f dependlist \
	echo make depend complete

document:
	$(DOCUMENT) -h $(SRC) > $(LIBRARY_ID).doc
	echo "" >> $(LIBRARY_ID).doc
	echo "Procedure Definitions" >> $(LIBRARY_ID).doc
	echo "" >> $(LIBRARY_ID).doc
	$(DOCUMENT) -t $(SRC) >> $(LIBRARY_ID).doc




# DO NOT DELETE THIS LINE - make depend uses it
