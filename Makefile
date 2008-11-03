# CMUCS AFStools
# dumpscan - routines for scanning and manipulating AFS volume dumps
#
# Copyright (c) 1998, 2001 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
#
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
#
# Carnegie Mellon requests users of this software to return to
#
#  Software Distribution Coordinator  or  Software_Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
#
# any improvements or extensions that they make and grant Carnegie Mellon
# the rights to redistribute these changes.

VERSION=1.2

# For building my binaries
#DESTDIR=/afs/cs.cmu.edu/project/systems-jhutz/dumpscan/
#bindir=.bin/$(VERSION)/$(SYS)
bindir=${afs}/bin

# Override this if your AFS is not in /usr/local
afs=/usr/local

AR         = ar
COMPILE_ET = compile_et
RANLIB     = ranlib
_lib = lib

# On Linux:
ifeq ($(shell uname),Linux)
R=-Wl,-rpath,
ifeq ($(shell uname -i),x86_64)
_lib = lib64
endif
endif

# On Solaris:
ifeq ($(shell uname),SunOS)
R        = -R
XLDFLAGS = -L/usr/ucblib -R/usr/ucblib
XLIBS    = -lsocket -lnsl -lucb -lresolv
endif

DEBUG      = -g
INCLUDES   = -I$(afs)/include
CFLAGS     = $(DEBUG) $(INCLUDES)
LDFLAGS    = -L. -L$(afs)/$(_lib) $(R)$(afs)/$(_lib) -L$(afs)/$(_lib)/afs $(XLDFLAGS)

LIBS                 = -ldumpscan -lxfiles \
                       -lauth -laudit -lvolser -lvldb -lubik -lrxkad \
                       $(afs)/$(_lib)/afs/libsys.a -lrx -llwp \
                       -lafsutil -lcom_err $(XLIBS) com_err_compat.o
OBJS_afsdump_scan    = afsdump_scan.o repair.o
OBJS_afsdump_xsed    = afsdump_xsed.o repair.o
OBJS_libxfiles.a     = xfiles.o xfopen.o xf_errs.o xf_printf.o int64.o \
                       xf_files.o xf_rxcall.o xf_voldump.o \
                       xf_profile.o xf_profile_name.o
OBJS_libdumpscan.a   = primitive.o util.o dumpscan_errs.o parsetag.o \
                       parsedump.o parsevol.o parsevnode.o dump.o \
                       directory.o pathname.o backuphdr.o stagehdr.o

BINS = afsdump_scan afsdump_dirlist afsdump_extract
TARGETS = libxfiles.a libdumpscan.a $(BINS)

DISTFILES := Makefile README xf_errs.et dumpscan_errs.et \
             $(filter-out %_errs.c %_errs.h,$(wildcard *.[ch]))

all: $(TARGETS)

afsdump_scan: libxfiles.a libdumpscan.a $(OBJS_afsdump_scan) com_err_compat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o afsdump_scan $(OBJS_afsdump_scan) $(LIBS)

afsdump_xsed: libxfiles.a libdumpscan.a $(OBJS_afsdump_xsed) com_err_compat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o afsdump_xsed $(OBJS_afsdump_xsed) $(LIBS)

afsdump_dirlist: libxfiles.a libdumpscan.a afsdump_dirlist.o com_err_compat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o afsdump_dirlist afsdump_dirlist.o $(LIBS)

afsdump_extract: libxfiles.a libdumpscan.a afsdump_extract.o com_err_compat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o afsdump_extract afsdump_extract.o $(LIBS)

genrootafs: libxfiles.a libdumpscan.a genroot.o com_err_compat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o genrootafs genroot.o $(LIBS)

null-search: libxfiles.a libdumpscan.a null-search.c com_err_compat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o null-search null-search.c $(LIBS)

# General-purpose target for custom programs
prog-% : libxfiles.a libdumpscan.a %.c com_err_compat.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $* $*.c $(LIBS)

libxfiles.a: $(OBJS_libxfiles.a)
	-rm -f libxfiles.a
	$(AR) r libxfiles.a $(OBJS_libxfiles.a)
	$(RANLIB) libxfiles.a

libdumpscan.a: $(OBJS_libdumpscan.a)
	-rm -f libdumpscan.a
	$(AR) r libdumpscan.a $(OBJS_libdumpscan.a)
	$(RANLIB) libdumpscan.a

xf_errs.c xf_errs.h: xf_errs.et
	$(COMPILE_ET) xf_errs.et

dumpscan_errs.c dumpscan_errs.h: dumpscan_errs.et
	$(COMPILE_ET) dumpscan_errs.et

util.o xfiles.o xf_files.o: xf_errs.h
backuphdr.o directory.o parsedump.o parsetag.o: dumpscan_errs.h
parsevnode.o parsevol.o pathname.o repair.o:    dumpscan_errs.h
stagehdr.o util.o:                              dumpscan_errs.h

CPRULE = test -d $(dir $@) || mkdir -p $(dir $@); cp $< $@
$(DESTDIR)$(bindir)/% : % ; $(CPRULE)
install: $(BINS:%=$(DESTDIR)$(bindir)/%)

clean:
	-rm -f xf_errs.c xf_errs.h dumpscan_errs.c dumpscan_errs.h *.o $(TARGETS)

dist:
	tar -czvf Dist.tar.gz $(DISTFILES)
