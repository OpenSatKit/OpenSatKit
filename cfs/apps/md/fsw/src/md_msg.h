/************************************************************************
** File: md_msg.h
**
** NASA Docket No. GSC-18,450-1, identified as “Core Flight Software System (CFS)
** Memory Dwell Application Version 2.3.3” 
**
** Copyright © 2019 United States Government as represented by the Administrator of
** the National Aeronautics and Space Administration. All Rights Reserved. 
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
** http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License. 
*
** Purpose: 
**   Specification for the CFS Memory Dwell command and telemetry 
**   messages.
**
** Notes:
**
** 
*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _md_msg_h_
#define _md_msg_h_

/*
** Required header files...
*/
#include "md_platform_cfg.h"
#include "cfe.h"
/* cfs_utils.h needed for CFS_SymAddr_t */
#include "cfs_utils.h"
#include "md_msgdefs.h"

/*************************************************************************/
/************************************************************************
** Type Definitions
*************************************************************************/

/********************************/
/* Command Message Data Formats */
/********************************/
/**
** \brief Generic "no arguments" command
**
** This command structure is used for commands that do not have any parameters.
** This includes:
** -# The Housekeeping Request Message
** -# The Wakeup Message
** -# The No-Op Command (For details, see #MD_NOOP_CC)
** -# The Reset Counters Command (For details, see #MD_RESET_CNTRS_CC)
*/
typedef struct
{
    CFE_SB_CmdHdr_t   CmdHeader;              /**< \brief cFE Software Bus Command Message Header */

} MD_NoArgsCmd_t;

/**
** \brief Start and Stop Dwell Commands
**
** For command details, see #MD_START_DWELL_CC and #MD_STOP_DWELL_CC
**
**/
typedef struct                             
{
    CFE_SB_CmdHdr_t  Header;           /**< \brief cFE Software Bus Command Message Header */
    uint16           TableMask;        /**< \brief 0x0001=TBL1  bit,
            0x0002=TBL2 bit,0x0004=TBL3 bit,0x0008=TBL4 enable bit, etc. */
    uint16           Padding;          /**< \brief structure padding */
} MD_CmdStartStop_t;



/**
** \brief Jam Dwell Command
**
** For command details, see #MD_JAM_DWELL_CC
**
**/
typedef struct                              /* for MD_JAM_DWELL */
{
    CFE_SB_CmdHdr_t  Header;    /**< \brief cFE Software Bus Command Message Header */

    uint16   TableId;           /**< \brief Table Id: 1..#MD_NUM_DWELL_TABLES */
    uint16   EntryId;           /**< \brief Address index: 1..#MD_DWELL_TABLE_SIZE  */
    uint16	 FieldLength;       /**< \brief Length of Dwell Field : 0, 1, 2, or 4  */
    uint16	 DwellDelay;        /**< \brief Dwell Delay (number of task wakeup calls before following dwell) */
    CFS_SymAddr_t DwellAddress; /**< \brief Dwell Address in #CFS_SymAddr_t format */
} MD_CmdJam_t;

#if MD_SIGNATURE_OPTION == 1  
/**
** \brief Set Signature Command
**
** For command details, see #MD_SET_SIGNATURE_CC
**
**/
typedef struct                              
{
    CFE_SB_CmdHdr_t  Header;    /**< \brief cFE Software Bus Command Message Header */
    
    uint16   TableId;      /**< \brief Table Id: 1..MD_NUM_DWELL_TABLES */
    uint16   Padding; /**< \brief Padding  */
    char     Signature[MD_SIGNATURE_FIELD_LENGTH];
} MD_CmdSetSignature_t;

#endif

/*************************************************************************/
/**********************************/
/* Telemetry Message Data Formats */
/**********************************/
/** 
**  \mdtlm Memory Dwell HK Telemetry format
**/

typedef struct
    {
    CFE_SB_TlmHdr_t   TlmHeader;       /**< \brief cFE SB Tlm Msg Hdr */

    /*
    ** Task command interface counters...
    */
	uint8   InvalidCmdCntr;     /**< \mdtlmmnemonic \MD_CMDEC 
                                     \brief Count of invalid commands received */
    uint8   ValidCmdCntr;       /**< \mdtlmmnemonic \MD_CMDPC 
                                     \brief Count of valid commands received */
    uint16  DwellEnabledMask;   /**< \mdtlmmnemonic \MD_ENABLEMASK 
                                     \brief Each bit in bit mask enables a table
                                      0x0001=TBL1 enable bit,0x0002=TBL2 enable bit
                                      0x0004=TBL3 enable bit,0x0008=TBL4 enable bit, etc. */

    uint16  DwellTblAddrCount[MD_NUM_DWELL_TABLES]; /**< \mdtlmmnemonic \MD_ADDRCNT
                                                         \brief Number of dwell addresses in table */
    uint16  NumWaitsPerPkt[MD_NUM_DWELL_TABLES];    /**< \mdtlmmnemonic \MD_RATES
                                                         \brief Number of delay counts in table */
	uint16  ByteCount[MD_NUM_DWELL_TABLES];         /**< \mdtlmmnemonic \MD_DATASIZE
                                                         \brief Number of bytes of data specified by table */
	uint16  DwellPktOffset[MD_NUM_DWELL_TABLES];    /**< \mdtlmmnemonic \MD_DWPKTOFFSET 
                                                         \brief Current write offset within dwell pkt data region  */
	uint16  DwellTblEntry[MD_NUM_DWELL_TABLES];     /**< \mdtlmmnemonic \MD_DWTBLENTRY 
                                                         \brief Next dwell table entry to be processed  */

	uint16  Countdown[MD_NUM_DWELL_TABLES];         /**< \mdtlmmnemonic \MD_COUNTDOWN 
                                                         \brief Current value of countdown timer  */

    } MD_HkTlm_t;

#define MD_HK_TLM_LNGTH        sizeof(MD_HkTlm_t)


/**********************************/
/** 
**  \mdtlm Memory Dwell Telemetry Packet format 
**/
typedef struct                          /* Actual Dwell information */
    {
    CFE_SB_TlmHdr_t   TlmHeader;       /**< \brief cFE SB Tlm Msg Hdr */
    
    uint8             TableId;         /**< \mdtlmmnemonic \MD_TABLEID
                                            \brief TableId from 1 to #MD_NUM_DWELL_TABLES */
    
    uint8             AddrCount;       /**< \mdtlmmnemonic \MD_NUMADDRESSES 
                                            \brief Number of addresses being sent - 1..#MD_DWELL_TABLE_SIZE valid */
    
    uint16            ByteCount;       /**< \mdtlmmnemonic \MD_PKTDATASIZE
                                            \brief Number of bytes of dwell data contained in packet */

    
    uint32            Rate;            /**< \mdtlmmnemonic \MD_RATE \brief Number of counts between packet sends*/

#if MD_SIGNATURE_OPTION == 1      
    char              Signature[MD_SIGNATURE_FIELD_LENGTH];    
                                       /**< \mdtlmmnemonic \MD_SIGNATURE \brief Signature */
    
#endif

    uint8             Data[MD_DWELL_TABLE_SIZE*4];   
                                      /**< \mdtlmmnemonic \MD_DWELLDATA 
                                           \brief Dwell data ( number of bytes varies up to MD_DWELL_TABLE_SIZE *4) */
    
    } MD_DwellPkt_t;

#define MD_DWELL_PKT_LNGTH         (sizeof(MD_DwellPkt_t))


/*************************************************************************/

#endif /* _md_msg_ */

/************************/
/*  End of File Comment */
/************************/
