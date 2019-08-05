/******************************************************************************
 ** \file sbn_remap.h
 **
 **      Copyright (c) 2004-2006, United States government as represented by the
 **      administrator of the National Aeronautics Space Administration.
 **      All rights reserved. This software(cFE) was created at NASA's Goddard
 **      Space Flight Center pursuant to government contracts.
 **
 **      This software may be used only pursuant to a United States government
 **      sponsored project and the United States government may not be charged
 **      for use thereof.
 **
 ** Purpose:
 **      This file contains source code for the Software Bus Network
 **      Application.
 **
 ** Authors: C. Knight/ARC Code TI
 ******************************************************************************/

#ifndef _sbn_remap_h_
#define _sbn_remap_h_

#include "cfe_sb.h"

/* TODO: document */
void SBN_RemapTblSort(SBN_RemapTbl_t *Tbl);

/* TODO: document */
CFE_SB_MsgId_t SBN_RemapMsgID(uint32 ProcessorID, CFE_SB_MsgId_t FromMID);

/* TODO: document */
uint32 SBN_RemapAdd(uint32 ProcessorID, CFE_SB_MsgId_t FromMID,
    CFE_SB_MsgId_t ToMID);

/* TODO: document */
uint32 SBN_RemapDel(uint32 ProcessorID, CFE_SB_MsgId_t FromMID);

#endif /* _sbn_remap_h_ */
