/*
 * CMUCS AFStools
 * dumpscan - routines for scanning and manipulating AFS volume dumps
 *
 * Copyright (c) 1998, 2001 Carnegie Mellon University
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

/* xfiles.c - General support routines for xfiles */
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "xfiles.h"
#include "xf_errs.h"

#define SKIP_SIZE 65536


afs_uint32 xfread(XFILE *X, void *buf, afs_uint32 count)
{
  afs_uint32 code;
  u_int64 tmp64;

  code = (X->do_read)(X, buf, count);
  if (code) return code;

  add64_32(tmp64, X->filepos, count);
  cp64(X->filepos, tmp64);
  if (X->passthru) return xfwrite(X->passthru, buf, count);
  return 0;
}


afs_uint32 xfwrite(XFILE *X, void *buf, afs_uint32 count)
{
  afs_uint32 code;
  u_int64 tmp64;

  if (!X->is_writable) return ERROR_XFILE_RDONLY;
  code = (X->do_write)(X, buf, count);
  if (code) return code;

  add64_32(tmp64, X->filepos, count);
  cp64(X->filepos, tmp64);
  return 0;
}


afs_uint32 xftell(XFILE *X, u_int64 *offset)
{
  if (X->do_tell) return (X->do_tell)(X, offset);
  cp64(*offset, X->filepos);
  return 0;
}


afs_uint32 xfseek(XFILE *X, u_int64 *offset)
{
  afs_uint32 code;

  if (!X->do_seek) return ERROR_XFILE_NOSEEK;
  code = (X->do_seek)(X, offset);
  if (code) return code;
  cp64(X->filepos, *offset);
  return 0;
}


afs_uint32 xfskip(XFILE *X, afs_uint32 count)
{
  afs_uint32 code;
  u_int64 tmp64;

  /* Use the skip method, if there is one */
  if (X->do_skip && !X->passthru) {
    mk64(tmp64, 0, count);
    code = (X->do_skip)(X, &tmp64);
    if (code) return code;
    add64_32(tmp64, X->filepos, count);
    cp64(X->filepos, tmp64);
    return 0;
  }

  /* Simulate using absolute seek, if available */
  if (X->do_seek && !X->passthru) {
    if (code = xftell(X, &tmp64)) return code;
    add64_32(X->filepos, tmp64, count);
    cp64(tmp64, X->filepos);
    return xfseek(X, &tmp64);
  }

  /* Do it the hard/slow way - read all the data to be skipped.
   * This is done if no other method is available, or if we are
   * supposed to be copying all the data to another XFILE
   */
  {
    char buf[SKIP_SIZE];
    afs_uint32 n;

    while (count) {
      n = (count > SKIP_SIZE) ? SKIP_SIZE : count;
      if (code = xfread(X, buf, n)) return code;
      count -= n;
    }
    return 0;
  }
}

afs_uint32 xfskip64(XFILE *X, u_int64 *count)
{
  afs_uint32 code;
  u_int64 tmp64;

  /* Use the skip method, if there is one */
  if (X->do_skip && !X->passthru) {
    code = (X->do_skip)(X, count);
    if (code) return code;
    add64_64(tmp64, X->filepos, *count);
    cp64(X->filepos, tmp64);
    return 0;
  }

  /* Simulate using absolute seek, if available */
  if (X->do_seek && !X->passthru) {
    if (code = xftell(X, &tmp64)) return code;
    add64_64(X->filepos, tmp64, *count);
    cp64(tmp64, X->filepos);
    return xfseek(X, &tmp64);
  }

  /* Do it the hard/slow way - read all the data to be skipped.
   * This is done if no other method is available, or if we are
   * supposed to be copying all the data to another XFILE
   */
  {
    char buf[SKIP_SIZE];
    afs_uint32 n;
    u_int64 remaining, zero;

    mk64(zero, 0, 0);
    cp64(remaining, *count);

    while (gt64(remaining, zero)) {
      mk64(tmp64, 0, SKIP_SIZE);
      if (gt64(remaining, tmp64)) n = SKIP_SIZE;
      else n = get64(remaining);
      if (code = xfread(X, buf, n)) return code;
      sub64_32(tmp64, remaining, n);
      cp64(remaining, tmp64);
    }
    return 0;
  }
}


afs_uint32 xfpass(XFILE *X, XFILE *Y)
{
  if (X->passthru) return ERROR_XFILE_ISPASS;
  if (!Y->is_writable) return ERROR_XFILE_RDONLY;
  X->passthru = Y;
  return 0;
}


afs_uint32 xfunpass(XFILE *X)
{
  if (!X->passthru) return ERROR_XFILE_NOPASS;
  X->passthru = 0;
  return 0;
}


afs_uint32 xfclose(XFILE *X)
{
  int code = 0;

  if (X->do_close) code = (X->do_close)(X);
  memset(X, 0, sizeof(*X));
  return code;
}
