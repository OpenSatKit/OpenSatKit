/******************************************************************************
** File: sbn_app.h
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
**      This header file contains definitions of the SBN event ID's and
**      ranges for modules.
**
** Author:   Chris Knight, NASA Ames Research Center
**
******************************************************************************/
#include "cfe.h"

#ifndef _sbn_events_h_
#define _sbn_events_h_

#define SBN_FIRST_EID                   0x0001

#define SBN_UDP_FIRST_EID               0x0100
#define SBN_TCP_FIRST_EID               0x0200
#define SBN_SPACEWIRE_RMAP_FIRST_EID    0x0300
#define SBN_SPACEWIRE_PKT_FIRST_EID     0x0400
#define SBN_SHMEM_FIRST_EID             0x0500
#define SBN_SERIAL_FIRST_EID            0x0600
#define SBN_1553_FIRST_EID              0x0700
#define SBN_DTN_FIRST_EID               0x0800

#endif /* _sbn_events_h_ */
