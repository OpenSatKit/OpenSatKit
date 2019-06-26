/******************************************************************************
** File: sbn_subs.h
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
**      This header file contains prototypes for private functions related to 
**      handling message subscriptions
**
** Authors:   J. Wilmot/GSFC Code582
**            R. McGraw/SSI
**            E. Timmons/GSFC Code587
**            C. Knight/ARC Code TI
**
******************************************************************************/

#ifndef _sbn_subs_h_
#define _sbn_subs_h_

#include "sbn_app.h"

void  SBN_SendLocalSubsToPeer(SBN_PeerInterface_t *Peer);
int32 SBN_CheckSubscriptionPipe(void); 
void  SBN_ProcessSubsFromPeer(SBN_PeerInterface_t *Peer, void *submsg);
void  SBN_ProcessUnsubsFromPeer(SBN_PeerInterface_t *Peer, void *submsg);
void  SBN_ProcessAllSubscriptions(CFE_SB_PrevSubMsg_t *Ptr); 
void  SBN_RemoveAllSubsFromPeer(SBN_PeerInterface_t *Peer); 
void  SBN_SendSubsRequests(void); 

#endif /* _sbn_subs_h_ */
