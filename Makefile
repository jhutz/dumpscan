
AR         = ar
COMPILE_ET = compile_et
RANLIB     = ranlib

DEBUGG     = -g
INCLUDES   = -I/usr/local/include
CFLAGS     = $(DEBUG) $(INCLUDES)
LDFLAGS    = -L. -L/usr/local/lib -L/usr/local/lib/afs
#            -L/usr/ucblib -R/usr/ucblib

LIBS                 = -ldumpscan -lxfiles \
                       -lauth -laudit -lvolser -lvldb -lubik -lrxkad \
                       /usr/local/lib/afs/libsys.a -lrx -llwp \
                       /usr/local/lib/afs/util.a -lcom_err
#                      -lsocket -lnsl -lucb
OBJS_afsdump_scan    = afsdump_scan.o repair.o
OBJS_afsdump_xsed    = afsdump_xsed.o repair.o
OBJS_libxfiles.a     = xfiles.o xf_errs.o xf_files.o xf_rxcall.o int64.o
OBJS_libdumpscan.a   = primitive.o util.o dumpscan_errs.o parsetag.o \
                       parsedump.o parsevol.o parsevnode.o dump.o \
                       directory.o pathname.o backuphdr.o stagehdr.o

TARGETS = libxfiles.a libdumpscan.a \
          afsdump_scan afsdump_dirlist

all: $(TARGETS)

afsdump_scan: libxfiles.a libdumpscan.a $(OBJS_afsdump_scan)
	$(CC) $(CFLAGS) $(LDFLAGS) -o afsdump_scan $(OBJS_afsdump_scan) $(LIBS)

afsdump_xsed: libxfiles.a libdumpscan.a $(OBJS_afsdump_xsed)
	$(CC) $(CFLAGS) $(LDFLAGS) -o afsdump_xsed $(OBJS_afsdump_xsed) $(LIBS)

afsdump_dirlist: libxfiles.a libdumpscan.a afsdump_dirlist.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o afsdump_dirlist afsdump_dirlist.o $(LIBS)

null-search: libxfiles.a libdumpscan.a null-search.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o null-search null-search.c $(LIBS)

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

clean:
	-rm xf_errs.c xf_errs.h dumpscan_errs.c dumpscan_errs.h *.o $(TARGETS)