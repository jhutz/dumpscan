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

/* dumpscan.h - Public interface */

#ifndef _DUMPSCAN_H_
#define _DUMPSCAN_H_

#include "intNN.h"
#include "xfiles.h"

#include <lock.h>
#include <afs/afsint.h>
#include <afs/nfs.h>
#include <afs/vnode.h>

/* Random useful types */
typedef struct tagged_field tagged_field;
typedef struct tag_parse_info tag_parse_info;
typedef u_int32 (*tag_parser)(XFILE *, unsigned char *, tagged_field *,
                              u_int32, tag_parse_info *, void *, void *);

/* Error codes used within dumpscan.
 * Any of the routines declared below, or callbacks used by them,
 * may signal a system error by returning the error number, or
 * some other error by returning a com_err code.  Note that
 * ParseTaggedData does _not_ return DSERR_TAG; instead, it returns
 * 0, assuming the tag will be handled at a higher level.
 *
 * In addition, these errors may be reported to the caller of
 * ParseDumpFile using the error callback.  Such reports will be
 * issued whether or not error recovery is possible or attempted.
 *
 * NB: These errors are now in dumpscan_errs.h
 */


/* Backup system dump header */
/* Right now, this looks a lot like an old stage header.  Eventually, it
 * should contain enough fields to fully represent headers from old or
 * new stage, Transarc, or other backup systems, and the appropriate read
 * functions should extract as much data as possible from the actual file
 * to fill this in. */
typedef struct {
  u_int32 version;
  u_int32 from_date;
  u_int32 to_date;
  u_int32 dump_date;
  u_int32 filenum;
  unsigned char *server;
  unsigned char *part;
  unsigned char *volname;
  u_int32 volid;
  u_int32 dumplen;
  u_int32 level;
  u_int32 magic;
  u_int32 cksum;
  u_int32 flags;
} backup_system_header;


/** AFS dump header **/
#define F_DUMPHDR_VOLID       0x00000001
#define F_DUMPHDR_VOLNAME     0x00000002
#define F_DUMPHDR_FROM        0x00000004
#define F_DUMPHDR_TO          0x00000008
typedef struct {
  u_int64 offset;           /* Where in the file is it? */
  u_int32 field_mask;       /* What fields are present? */
  u_int32 magic;            /* Magic number */
  u_int32 version;          /* Dump format version */
  u_int32 volid;            /* VolID of volume in dump */
  unsigned char *volname;   /* Name of volume in dump */
  u_int32 from_date;        /* Reference date */
  u_int32 to_date;          /* Date of dump */
} afs_dump_header;


/** AFS volume header **/
#define F_VOLHDR_VOLID        0x00000001
#define F_VOLHDR_VOLVERS      0x00000002
#define F_VOLHDR_VOLNAME      0x00000004
#define F_VOLHDR_INSERV       0x00000008
#define F_VOLHDR_BLESSED      0x00000010
#define F_VOLHDR_VOLUNIQ      0x00000020
#define F_VOLHDR_VOLTYPE      0x00000040
#define F_VOLHDR_PARENT       0x00000080
#define F_VOLHDR_CLONE        0x00000100
#define F_VOLHDR_MAXQ         0x00000200
#define F_VOLHDR_MINQ         0x00000400
#define F_VOLHDR_DISKUSED     0x00000800
#define F_VOLHDR_NFILES       0x00001000
#define F_VOLHDR_ACCOUNT      0x00002000
#define F_VOLHDR_OWNER        0x00004000
#define F_VOLHDR_CREATE_DATE  0x00008000
#define F_VOLHDR_ACCESS_DATE  0x00010000
#define F_VOLHDR_UPDATE_DATE  0x00020000
#define F_VOLHDR_EXPIRE_DATE  0x00040000
#define F_VOLHDR_BACKUP_DATE  0x00080000
#define F_VOLHDR_OFFLINE_MSG  0x00100000
#define F_VOLHDR_MOTD         0x00200000
#define F_VOLHDR_WEEKUSE      0x00400000
#define F_VOLHDR_DAYUSE       0x00800000
#define F_VOLHDR_DAYUSE_DATE  0x01000000
typedef struct {
  u_int64 offset;           /* Where in the file is it? */
  u_int32 field_mask;       /* What fields are present? */
  u_int32 volid;            /* Volume ID */
  u_int32 volvers;          /* ?? */
  unsigned char *volname;   /* Volume Name */
  int     flag_inservice;   /* Inservice flag (0 or not) */
  int     flag_blessed;     /* Blessed to come online (0 or not) */
  u_int32 voluniq;          /* Volume uniquifier */
  int     voltype;          /* Volume type */
  u_int32 parent_volid;     /* Parent volume ID */
  u_int32 clone_volid;      /* Clone volume ID */
  u_int32 maxquota;         /* Max quota */
  u_int32 minquota;         /* Min quota (obsolete) */
  u_int32 diskused;         /* Disk blocks used */
  u_int32 nfiles;           /* Number of files in volume */
  u_int32 account_no;       /* Account number (unused) */
  u_int32 owner;            /* Volume owner */
  u_int32 create_date;      /* Creation date of this copy */
  u_int32 access_date;      /* Last access */
  u_int32 update_date;      /* Last modification */
  u_int32 expire_date;      /* Expiration (unused) */
  u_int32 backup_date;      /* Last backup clone */
  unsigned char *offline_msg; /* Offline message */
  unsigned char *motd_msg;    /* Volume MOTD */
  u_int32 weekuse[7];       /* Weekuse data */
  u_int32 dayuse;           /* # accesses in last day */
  u_int32 dayuse_date;      /* Date for which dayuse is valid */
} afs_vol_header;


/** AFS vnode **/
#define F_VNODE_TYPE          0x00000001
#define F_VNODE_NLINKS        0x00000002
#define F_VNODE_PARENT        0x00000004
#define F_VNODE_DVERS         0x00000008
#define F_VNODE_AUTHOR        0x00000010
#define F_VNODE_OWNER         0x00000020
#define F_VNODE_GROUP         0x00000040
#define F_VNODE_MODE          0x00000080
#define F_VNODE_CDATE         0x00000100
#define F_VNODE_SDATE         0x00000200
#define F_VNODE_SIZE          0x00000800
#define F_VNODE_DATA          0x00001000
#define F_VNODE_ACL           0x00000400
typedef struct {
  u_int64 offset;           /* Where in the file is it? */
  u_int32 field_mask;       /* What fields are present? */
  u_int32 vnode;            /* Vnode number */
  u_int32 vuniq;            /* Uniquifier */
  int     type;             /* Vnode type */
  u_int16 nlinks;           /* Number of links (should be in 1 dir!) */
  u_int32 parent;           /* Parent vnode */
  u_int32 datavers;         /* Data version */
  u_int32 author;           /* Last writer */
  u_int32 owner;            /* Owner UID */
  u_int32 group;            /* Owning group */
  u_int16 mode;             /* UNIX mode bits */
  u_int32 client_date;      /* Last modified date from client */
  u_int32 server_date;      /* Last modified date on server */
  u_int32 size;             /* Size of data */
  u_int64 d_offset;         /* Where in the file is the data? */
  unsigned char acl[SIZEOF_LARGEDISKVNODE - SIZEOF_SMALLDISKVNODE];
} afs_vnode;


/** AFS directory entry **/
typedef struct {
  int  slot;                /* Directory slot # (info only) */
  char *name;               /* Name of entry */
  u_int32 vnode;            /* Vnode number */
  u_int32 uniq;             /* Uniquifier */
} afs_dir_entry;


/** Tagged field definitions **/
#define DKIND_NOOP      0x00  /* No data */
#define DKIND_BYTE      0x10  /* 1 byte  - decimal */
#define DKIND_HEX8      0x11  /* 1 byte  - hex */
#define DKIND_CHAR      0x12  /* 1 byte  - character */
#define DKIND_FLAG      0x13  /* 1 byte  - true/false */
#define DKIND_INT16     0x20  /* 2 bytes - decimal */
#define DKIND_HEX16     0x21  /* 2 bytes - hex */
#define DKIND_INT32     0x30  /* 4 bytes - decimal */
#define DKIND_HEX32     0x31  /* 4 bytes - hex */
#define DKIND_TIME      0x32  /* 4 bytes - time */
#define DKIND_STRING    0x40  /* ASCIIZ string */
#define DKIND_SPECIAL   0x50  /* Custom parser */
#define DKIND_MASK     (~0x0f)
struct tag_parse_info {
  void *err_refcon;
  u_int32 (*cb_error)(u_int32, int, void *, char *, ...);
  u_int32 flags;
#define TPFLAG_SKIP   0x0001
#define TPFLAG_RSKIP  0x0002
  int shift_offset;
  u_int64 shift_start;
};
struct tagged_field {
  char tag;        /* Tag character */
  int  kind;       /* Kind of object */
  char *label;     /* Label to use (for debugging) */
  tag_parser func; /* Parser function (for DKIND_SPECIAL) */
  void *refptr;    /* Reference pointer (for parser's use) */
  int  refarg;     /* Reference argument (for parser's use) */
};


/** Control structure for parsing volume dumps **/
typedef struct {
  /* Callback functions:
   * Whenever a "complex" object is parsed, we call a callback function.
   * The callback gets a pointer to the complex object, the file pointer
   * for the dump we're parsing, and the value of refcon in this structure.
   * Callbacks should return 0 if all is well, non-0 to abort the dump.
   * By convention, positive numbers should be errno values, and negative
   * numbers can be used for other things.  It is OK to _try_ to seek anywhere
   * in the file.  Beware, though, that the input is not always seekable.
   * Also, note that the structures passed to these callbacks are going to
   * go away after the callback returns.  There is no way to prevent this;
   * make a copy if you want one.
   */
  void *refcon;
  u_int32 (*cb_bckhdr)(backup_system_header *, XFILE *, void *); /* Backup   */
  u_int32 (*cb_dumphdr)(afs_dump_header *, XFILE *, void *); /* Dump hdr     */
  u_int32 (*cb_volhdr)(afs_vol_header *, XFILE *, void *);   /* Volume hdr   */
  u_int32 (*cb_vnode_dir)(afs_vnode *, XFILE *, void *);     /* Directory    */
  u_int32 (*cb_vnode_file)(afs_vnode *, XFILE *, void *);    /* File         */
  u_int32 (*cb_vnode_link)(afs_vnode *, XFILE *, void *);    /* Symlink      */
  u_int32 (*cb_vnode_empty)(afs_vnode *, XFILE *, void *);   /* vnode+uniq   */
  u_int32 (*cb_vnode_wierd)(afs_vnode *, XFILE *, void *);   /* Unknown type */

  /* This function is called when there is an error in the dump. */
  /* (cb_error)(errno, fatal, refcon, msg_fmt, msg_args...) */
  void *err_refcon; /* If set, use instead of refcon for dir entries */
  u_int32 (*cb_error)(u_int32, int, void *, char *, ...);

  /* This function is called for each directory entry, if set */
  u_int32 (*cb_dirent)(afs_vnode *, afs_dir_entry *, XFILE *, void *);

  int flags;            /* Flags and options */
#define DSFLAG_SEEK     0x0001  /* Input file is seekable */

  int print_flags;      /* Flags to control what is printed */
#define DSPRINT_BCKHDR  0x0001  /* Print backup system header */
#define DSPRINT_DUMPHDR 0x0002  /* Print AFS dump header */
#define DSPRINT_VOLHDR  0x0004  /* Print AFS volume header */
#define DSPRINT_ITEM    0x0010  /* Print top-level tags */
#define DSPRINT_VNODE   0x0020  /* Print vnode attributes */
#define DSPRINT_ACL     0x0040  /* Print directory ACL's */
#define DSPRINT_DIR     0x0080  /* Print directory contents */
#define DSPRINT_DEBUG   0x0100  /* Print debugging info */
#define DSPRINT_PATH    0x0200  /* Print vnode paths */

  int repair_flags;     /* Flags to control what is repaired.
                         * Most of these _require_ DSFLAG_SEEK */
#define DSFIX_SKIP      0x0001  /* Try to skip null tags */
#define DSFIX_RSKIP     0x0002  /* Seek back to fing skipped tags */
#define DSFIX_VDSYNC    0x0004  /* Resync location after vnode data */
#define DSFIX_VFSYNC    0x0008  /* Try to resync after bad vnode */

  /** Things below this point for internal use only **/
  u_int32 vol_uniquifier;
} dump_parser;


/** Hash table and control info for pathname manipulation **/
typedef struct vhash_ent {
  struct vhash_ent *next;    /* Pointer to next entry */
  u_int32 vnode;             /* VNode number */
  u_int32 parent;            /* Parent VNode number */
  u_int64 v_offset;          /* Offset to start of vnode */
  u_int64 d_offset;          /* Offset to data (0 if none) */
  u_int32 d_size;            /* Size of data */
} vhash_ent;
typedef struct {
  u_int32 n_vnodes;          /* Number of vnodes in volume */
  u_int32 n_dirs;            /* Number of file vnodes */
  u_int32 n_files;           /* Number of directory vnodes */
  int hash_size;             /* Hash table size (bits) */
  vhash_ent **hash_table;    /* Hash table */
  dump_parser *p;            /* Dump parser to use */
} path_hashinfo;


/** Function prototypes **/
/** Only the functions declared below are public interfaces **/
/** Maybe someday, I'll write man pages for these **/

/* primitive.c - I/O primitives */
extern u_int32 ReadByte(XFILE *, unsigned char *);
extern u_int32 ReadInt16(XFILE *, u_int16 *);
extern u_int32 ReadInt32(XFILE *, u_int32 *);
extern u_int32 ReadString(XFILE *, unsigned char **);
extern u_int32 WriteByte(XFILE *, unsigned char);
extern u_int32 WriteInt16(XFILE *, u_int16);
extern u_int32 WriteInt32(XFILE *, u_int32);
extern u_int32 WriteString(XFILE *, unsigned char *);

/* parsetag.c - Parse tagged data */
extern u_int32 ParseTaggedData(XFILE *, tagged_field *, unsigned char *,
                           tag_parse_info *, void *, void *);

/* stagehdr.c - Parse and dump Stage dump headers */
extern u_int32 ParseStageHdr(XFILE *, unsigned char *, backup_system_header *);
extern u_int32 DumpStagehdr(XFILE *, backup_system_header *);

/* backuphdr.c - Parse and print backup system headers */
extern void PrintBackupHdr(backup_system_header *);

/* parsedump.c - Parse all or part of a volume dump */
extern u_int32 ParseDumpFile(XFILE *, dump_parser *);
extern u_int32 ParseDumpHeader(XFILE *, dump_parser *);
extern u_int32 ParseVolumeHeader(XFILE *, dump_parser *);
extern u_int32 ParseVNode(XFILE *, dump_parser *);


/* directory.c - Directory parsing and lookup */
extern u_int32 ParseDirectory(XFILE *, dump_parser *, u_int32, int);
extern u_int32 DirectoryLookup(XFILE *, dump_parser *, u_int32,
                           char **, u_int32 *, u_int32 *);

/* dump.c - Dump parts of a volume dump */
extern u_int32 DumpDumpHeader(XFILE *, afs_dump_header *);
extern u_int32 DumpVolumeHeader(XFILE *, afs_vol_header *);
extern u_int32 DumpVNode(XFILE *, afs_vnode *);
extern u_int32 DumpVnodeData(XFILE *, char *, u_int32);
extern u_int32 CopyVnodeData(XFILE *, XFILE *, u_int32);

/* pathname.c - Follow and construct pathnames */
extern u_int32 Path_PreScan(XFILE *, path_hashinfo *, int);
extern void Path_FreeHashTable(path_hashinfo *);
extern u_int32 Path_Follow(XFILE *, path_hashinfo *, char *, vhash_ent *);
extern u_int32 Path_Build(XFILE *, path_hashinfo *, u_int32, char **, int);

#endif
