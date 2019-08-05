/************************************************************************
**   sbn_events.h
**   
**   2014/11/21 ejtimmon
**
**   Specification for the Software Bus Network event identifers.
**
*************************************************************************/

#ifndef _sbn_main_events_h_
#define _sbn_main_events_h_

#include "sbn_events.h"

#define SBN_SB_EID          SBN_FIRST_EID + 0
#define SBN_INIT_EID        SBN_FIRST_EID + 1
#define SBN_MSG_EID         SBN_FIRST_EID + 2
#define SBN_TBL_EID         SBN_FIRST_EID + 3
#define SBN_PEER_EID        SBN_FIRST_EID + 4
#define SBN_PROTO_EID       SBN_FIRST_EID + 5
#define SBN_CMD_EID         SBN_FIRST_EID + 6
#define SBN_SUB_EID         SBN_FIRST_EID + 7
#define SBN_REMAP_EID       SBN_FIRST_EID + 8
#define SBN_PEERTASK_EID    SBN_FIRST_EID + 9
#define SBN_DEBUG_EID       SBN_FIRST_EID + 10

#endif /* _sbn_main_events_h_ */
