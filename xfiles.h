/*
 * CMUCS AFStools
 * dumpscan - routines for scanning and manipulating AFS volume dumps
 *
 * Copyright (c) 1998 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software_Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

/* xfiles.h - Type, constant, and function declarations for
 * extensible file-like things */

#ifndef _XFILES_H_
#define _XFILES_H_

#include <stdio.h>
#include "intNN.h"

struct rx_call;
struct rx_connection;

/* The XFILE structure */
typedef struct XFILE XFILE;
struct XFILE {
  u_int32 (*do_read)(XFILE *, void *, u_int32);   /* read data */
  u_int32 (*do_write)(XFILE *, void *, u_int32);  /* write data */
  u_int32 (*do_tell)(XFILE *, u_int64 *);         /* find position */
  u_int32 (*do_seek)(XFILE *, u_int64 *);         /* set position */
  u_int32 (*do_skip)(XFILE *, u_int32);           /* skip forward */
  u_int32 (*do_close)(XFILE *);                   /* close */
  u_int64 filepos;                                /* position (counted) */
  int is_seekable;                                /* 1 if seek works */
  int is_writable;                                /* 1 if write works */
  XFILE *passthru;                                /* XFILE to pass thru to */
  void *refcon;                                   /* type-specific data */
};


/* Functions for opening XFILEs.  For these, the first two arguments are
 * always a pointer to an XFILE to fill in, and the mode in which to
 * open the file.  O_RDONLY and O_RDWR are permitted; O_WRONLY is not.
 * Other open modes may or may not be used, depending on the object type.
 * Remaining arguments are a function of the object type
 */
extern u_int32 xfopen     (XFILE *, int, char *);      /* open by TYPE:name */
extern u_int32 xfopen_path(XFILE *, int, char *, int); /* open by path   */
extern u_int32 xfopen_FILE(XFILE *, int, FILE *);      /* open by FILE * */
extern u_int32 xfopen_fd  (XFILE *, int, int);         /* open by fd     */
extern u_int32 xfopen_rxcall (XFILE *, int, struct rx_call *);
extern u_int32 xfopen_voldump(XFILE *, struct rx_connection *,
                              int32, int32, int32);

extern u_int32 xfregister(char *, u_int32 (*)(XFILE *, int, char *));

/* Standard operations on XFILEs */
extern u_int32 xfread(XFILE *, void *, u_int32);        /* read data */
extern u_int32 xfwrite(XFILE *, void *, u_int32);       /* write data */
extern u_int32 xftell(XFILE *, u_int64 *);              /* get position */
extern u_int32 xfseek(XFILE *, u_int64 *);              /* set position */
extern u_int32 xfskip(XFILE *, u_int32);                /* skip forward */
extern u_int32 xfpass(XFILE *, XFILE *);                /* set passthru */
extern u_int32 xfunpass(XFILE *);                       /* unset passthru */
extern u_int32 xfclose(XFILE *);                        /* close */

#endif /* _XFILES_H_ */
