/* 
** Purpose: C App Framework packet utilities
**
** Notes:
**   1. PktUtil_IsPacketFiltered is taken from cfs_utils. My initial motivation
**      was the macros defined in C file and not a header and no structures.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _pkt_util_
#define _pkt_util_


/*
** Includes
*/

#include "osk_c_fw_cfg.h"


/***********************/
/** Macro Definitions **/
/***********************/

#define PKTUTIL_MAX_APP_ID    (0x0800)  /* Maximum CCSDS v1 ApId */

#define PKTUTIL_PRI_HDR_BYTES (sizeof(CCSDS_SpacePacket_t))
#define PKTUTIL_PRI_HDR_WORDS (sizeof(CCSDS_SpacePacket_t)/2)


#define PKTUTIL_16_MSB_SUBSECS_SHIFT  16
#define PKTUTIL_11_LSB_SECONDS_MASK   0x07FF
#define PKTUTIL_11_LSB_SECONDS_SHIFT  4
#define PKTUTIL_4_MSB_SUBSECS_MASK    0xF000
#define PKTUTIL_4_MSB_SUBSECS_SHIFT   12


/**********************/
/** Type Definitions **/
/**********************/

/*
** PktUtil_IsPacketFiltered() should be used by ground commands
** or tables that accept a filter type.
*/

typedef enum {
   
   PKTUTIL_FILTER_ALWAYS     = 1,
   PKTUTIL_FILTER_BY_SEQ_CNT = 2,
   PKTUTIL_FILTER_BY_TIME    = 3,
   PKTUTIL_FILTER_NEVER      = 4
   
} PktUtil_FilterType;

/* 
** N of X packets ("group size") will be sent starting at offset O
*/ 
typedef struct {
   
   uint16 N;
   uint16 X;
   uint16 O;
   
} PktUtil_FilterParam;

typedef struct {
   
   PktUtil_FilterType   Type;
   PktUtil_FilterParam  Param;

} PktUtil_Filter;


/*
** Define a generic command without parameters so every app doesn't need to
** repeat the definition. 
*/

typedef struct {

    uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} PKTUTIL_NoParamCmdMsg;
#define PKTUTIL_NO_PARAM_CMD_DATA_LEN  ((sizeof(PKTUTIL_NoParamCmdMsg) - CFE_SB_CMD_HDR_SIZE))


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: PktUtil_IsPacketFiltered
**
*/
boolean PktUtil_IsPacketFiltered(const CFE_SB_MsgPtr_t MessagePtr, const PktUtil_Filter *Filter);


/******************************************************************************
** Function: PktUtil_IsFilterTypeValid
**
** Notes:
**   1. Intended for for parameter validation. It uses uint16 becaue command
**      packet definitions typically don't use enumerated types so they can 
**      control the storage size (prior to C++11).
*/
boolean PktUtil_IsFilterTypeValid(uint16 FilterType);


#endif /* _pkt_util_ */
