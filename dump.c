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

/* dump.c - Write out parts of a volume dump */

#include "dumpscan.h"
#include "dumpfmt.h"

#define COPYBUFSIZE 65536

u_int32 DumpDumpHeader(XFILE *OX, afs_dump_header *hdr)
{
  u_int32 r;

  if (r = WriteByte(OX, TAG_DUMPHEADER)) return r;
  if (r = WriteInt32(OX, hdr->magic)) return r;
  if (r = WriteInt32(OX, hdr->version)) return r;

  if (hdr->field_mask & F_DUMPHDR_VOLID) {
    if (r = WriteByte(OX, DHTAG_VOLID)) return r;
    if (r = WriteInt32(OX, hdr->volid)) return r;
  }
  if (hdr->field_mask & F_DUMPHDR_VOLNAME) {
    if (r = WriteByte(OX, DHTAG_VOLNAME)) return r;
    if (r = WriteString(OX, hdr->volname)) return r;
  }
  if (hdr->field_mask & (F_DUMPHDR_FROM | F_DUMPHDR_TO)) {
    if (r = WriteByte(OX, DHTAG_DUMPTIMES)) return r;
    if (r = WriteInt16(OX, 2)) return r;
    if (r = WriteInt32(OX, (hdr->field_mask & F_DUMPHDR_FROM)
                       ? hdr->from_date : 0))
      return r;
    if (r = WriteInt32(OX, (hdr->field_mask & F_DUMPHDR_TO)
                       ? hdr->to_date : time(0)))
      return r;
  }
  return 0;
}


u_int32 DumpVolumeHeader(XFILE *OX, afs_vol_header *hdr)
{
  u_int32 r;
  int i;

  if (r = WriteByte(OX, TAG_VOLHEADER)) return r;

  if (hdr->field_mask & F_VOLHDR_VOLID) {
    if (r = WriteByte(OX, VHTAG_VOLID)) return r;
    if (r = WriteInt32(OX, hdr->volid)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_VOLVERS) {
    if (r = WriteByte(OX, VHTAG_VERS)) return r;
    if (r = WriteInt32(OX, hdr->volvers)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_VOLNAME) {
    if (r = WriteByte(OX, VHTAG_VOLNAME)) return r;
    if (r = WriteString(OX, hdr->volname)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_INSERV) {
    if (r = WriteByte(OX, VHTAG_INSERV)) return r;
    if (r = WriteByte(OX, hdr->flag_inservice)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_BLESSED) {
    if (r = WriteByte(OX, VHTAG_BLESSED)) return r;
    if (r = WriteByte(OX, hdr->flag_blessed)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_VOLUNIQ) {
    if (r = WriteByte(OX, VHTAG_VUNIQ)) return r;
    if (r = WriteInt32(OX, hdr->voluniq)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_VOLTYPE) {
    if (r = WriteByte(OX, VHTAG_TYPE)) return r;
    if (r = WriteByte(OX, hdr->voltype)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_PARENT) {
    if (r = WriteByte(OX, VHTAG_PARENT)) return r;
    if (r = WriteInt32(OX, hdr->parent_volid)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_CLONE) {
    if (r = WriteByte(OX, VHTAG_CLONE)) return r;
    if (r = WriteInt32(OX, hdr->clone_volid)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_MAXQ) {
    if (r = WriteByte(OX, VHTAG_MAXQUOTA)) return r;
    if (r = WriteInt32(OX, hdr->maxquota)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_MINQ) {
    if (r = WriteByte(OX, VHTAG_MINQUOTA)) return r;
    if (r = WriteInt32(OX, hdr->minquota)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_DISKUSED) {
    if (r = WriteByte(OX, VHTAG_DISKUSED)) return r;
    if (r = WriteInt32(OX, hdr->diskused)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_NFILES) {
    if (r = WriteByte(OX, VHTAG_FILECNT)) return r;
    if (r = WriteInt32(OX, hdr->nfiles)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_ACCOUNT) {
    if (r = WriteByte(OX, VHTAG_ACCOUNT)) return r;
    if (r = WriteInt32(OX, hdr->account_no)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_OWNER) {
    if (r = WriteByte(OX, VHTAG_OWNER)) return r;
    if (r = WriteInt32(OX, hdr->owner)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_CREATE_DATE) {
    if (r = WriteByte(OX, VHTAG_CREAT)) return r;
    if (r = WriteInt32(OX, hdr->create_date)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_ACCESS_DATE) {
    if (r = WriteByte(OX, VHTAG_ACCESS)) return r;
    if (r = WriteInt32(OX, hdr->access_date)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_UPDATE_DATE) {
    if (r = WriteByte(OX, VHTAG_UPDATE)) return r;
    if (r = WriteInt32(OX, hdr->update_date)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_EXPIRE_DATE) {
    if (r = WriteByte(OX, VHTAG_EXPIRE)) return r;
    if (r = WriteInt32(OX, hdr->expire_date)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_BACKUP_DATE) {
    if (r = WriteByte(OX, VHTAG_BACKUP)) return r;
    if (r = WriteInt32(OX, hdr->backup_date)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_OFFLINE_MSG) {
    if (r = WriteByte(OX, VHTAG_OFFLINE)) return r;
    if (r = WriteString(OX, hdr->offline_msg)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_MOTD) {
    if (r = WriteByte(OX, VHTAG_MOTD)) return r;
    if (r = WriteString(OX, hdr->motd_msg)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_WEEKUSE) {
    if (r = WriteByte(OX, VHTAG_WEEKUSE)) return r;
    if (r = WriteInt16(OX, 7)) return r;
    for (i = 0; i < 7; i++)
      if (r = WriteInt32(OX, hdr->weekuse[i])) return r;
  }
  if (hdr->field_mask & F_VOLHDR_DAYUSE_DATE) {
    if (r = WriteByte(OX, VHTAG_DUDATE)) return r;
    if (r = WriteInt32(OX, hdr->dayuse_date)) return r;
  }
  if (hdr->field_mask & F_VOLHDR_DAYUSE) {
    if (r = WriteByte(OX, VHTAG_DAYUSE)) return r;
    if (r = WriteInt32(OX, hdr->dayuse)) return r;
  }
  return 0;
}


u_int32 DumpVNode(XFILE *OX, afs_vnode *v)
{
  u_int32 r;

  if (r = WriteByte(OX, TAG_VNODE)) return r;
  if (r = WriteInt32(OX, v->vnode)) return r;
  if (r = WriteInt32(OX, v->vuniq)) return r;

  if (v->field_mask & F_VNODE_TYPE) {
    if (r = WriteByte(OX, VTAG_TYPE)) return r;
    if (r = WriteByte(OX, v->type)) return r;
  }
  if (v->field_mask & F_VNODE_NLINKS) {
    if (r = WriteByte(OX, VTAG_NLINKS)) return r;
    if (r = WriteInt16(OX, v->nlinks)) return r;
  }
  if (v->field_mask & F_VNODE_DVERS) {
    if (r = WriteByte(OX, VTAG_DVERS)) return r;
    if (r = WriteInt32(OX, v->datavers)) return r;
  }
  if (v->field_mask & F_VNODE_SDATE) {
    if (r = WriteByte(OX, VTAG_SERVER_DATE)) return r;
    if (r = WriteInt32(OX, v->server_date)) return r;
  }
  if (v->field_mask & F_VNODE_AUTHOR) {
    if (r = WriteByte(OX, VTAG_AUTHOR)) return r;
    if (r = WriteInt32(OX, v->author)) return r;
  }
  if (v->field_mask & F_VNODE_OWNER) {
    if (r = WriteByte(OX, VTAG_OWNER)) return r;
    if (r = WriteInt32(OX, v->owner)) return r;
  }
  if (v->field_mask & F_VNODE_GROUP) {
    if (r = WriteByte(OX, VTAG_GROUP)) return r;
    if (r = WriteInt32(OX, v->group)) return r;
  }
  if (v->field_mask & F_VNODE_MODE) {
    if (r = WriteByte(OX, VTAG_MODE)) return r;
    if (r = WriteInt16(OX, v->mode)) return r;
  }
  if (v->field_mask & F_VNODE_PARENT) {
    if (r = WriteByte(OX, VTAG_PARENT)) return r;
    if (r = WriteInt32(OX, v->parent)) return r;
  }
  if (v->field_mask & F_VNODE_CDATE) {
    if (r = WriteByte(OX, VTAG_CLIENT_DATE)) return r;
    if (r = WriteInt32(OX, v->client_date)) return r;
  }
  if (v->field_mask & F_VNODE_ACL) {
    if (r = WriteByte(OX, VTAG_ACL)) return r;
    if (r = xfwrite(OX, v->acl, SIZEOF_LARGEDISKVNODE - SIZEOF_SMALLDISKVNODE))
      return r;
  }
  return 0;
}


u_int32 DumpVNodeData(XFILE *OX, char *buf, u_int32 size)
{
  u_int32 r;

  if (r = WriteByte(OX, VTAG_DATA)) return r;
  if (r = WriteInt32(OX, size)) return r;
  if (r = xfwrite(OX, buf, size)) return r;
  return 0;
}


u_int32 CopyVNodeData(XFILE *OX, XFILE *X, u_int32 size)
{
  u_int32 r, n;
  static char buf[COPYBUFSIZE];

  if (r = WriteByte(OX, VTAG_DATA)) return r;
  if (r = WriteInt32(OX, size)) return r;
  while (size) {
    n = (size > COPYBUFSIZE) ? COPYBUFSIZE : size;
    if (r = xfread(X, buf, n)) return r;
    if (r = xfwrite(OX, buf, n)) return r;
    size -= n;
  }
  return 0;
}


u_int32 DumpDumpEnd(XFILE *OX) {
  u_int32 r;

  if (r = WriteByte(OX, TAG_DUMPEND)) return r;
  if (r = WriteInt32(OX, DUMPENDMAGIC)) return r;
  return 0;
}
