/************************************************************************
**
** $Id: sc_rts002.c 1.2 2015/10/08 15:19:10EDT sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** CFS Stored Command (SC) sample RTS table #2
**
** Note 1: The following source code demonstrates how to create a sample
**         Stored Command RTS table.  The preferred method for creating
**         flight versions of RTS tables is to use custom ground system
**         tools that output the binary table files, skipping this step
**         altogether.
**         
** Note 2: This source file creates a sample RTS table that contains only
**         the following commands that are scheduled as follows:
**
**         SC NOOP command, execution time relative to start of RTS = 0
**         SC NOOP command, execution time relative to prev cmd = 5
**         SC NOOP command, execution time relative to prev cmd = 5
**
** Note 3: The byte following the command code in each command packet
**         secondary header must contain an 8 bit checksum.  Refer to
**         the SC Users Guide for information on how to calculate this
**         checksum.
**
** Note 4: If the command length (in bytes) is odd, a pad byte must be added 
**         to the RTS command structure (opt data portion) to ensure the next 
**         command starts on a word (uint16) boundary.
**
** $Log: sc_rts002.c  $
** Revision 1.2 2015/10/08 15:19:10EDT sstrege 
** Restoration from MKS 2009 Trunk
** Revision 1.9 2015/03/02 13:01:56EST sstrege 
** Added copyright information
** Revision 1.8 2014/12/18 17:13:40EST sstrege 
** Added note to alert users of required pad byte for odd length commands
** Revision 1.7 2014/12/15 10:32:38EST lwalling 
** Force Big Endian ccsds packet primary headers
** Revision 1.6 2014/12/02 19:00:14EST lwalling 
** Remove table compile warning from default tables
** Revision 1.5 2010/04/22 13:30:54EDT lwalling 
** Member renamed from sc_rts2.c to sc_rts002.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/tables/project.pj.
** Revision 1.4 2010/04/22 12:30:54ACT lwalling 
** Change default RTS table names from sc_rts2 to sc_rts002
** Revision 1.3 2010/04/15 16:48:30EDT lwalling 
** Fix typos - change references from RTS1 to RTS2
** Revision 1.2 2010/03/26 18:04:19EDT lwalling 
** Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
** Revision 1.1 2010/03/16 15:43:08EDT lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/tables/project.pj
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */
#include "sim_msgdefs.h"         /* defines SC command code values */
#include "sim_msgids.h"          /* defines SC packet msg ID's */
#include "hc_msgdefs.h"         /* defines SC command code values */
#include "hc_msgids.h"          /* defines SC packet msg ID's */
#include "lc_msgdefs.h"         /* defines SC command code values */
#include "lc_msgids.h"          /* defines SC packet msg ID's */



/*
** Execution time for each sample command
*/
#define CMD1_TIME     1
#define CMD2_TIME     1
#define CMD3_TIME     0
#define CMD4_TIME     0


/*
** Calculate checksum for each sample command
*/
#define CMD1_XSUM     0x0085
#define CMD2_XSUM     0x0096
#define CMD3_XSUM     0x0082
#define CMD4_XSUM     0x0083


/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000


/*
** Length of cmd pkt data (in bytes minus one) that follows primary header (thus, 0xFFFF = 64k)
*/
#define CMD1_LENGTH   5
#define CMD2_LENGTH   2
#define CMD3_LENGTH   3
#define CMD4_LENGTH   3


/*
** Sample cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table003", "SC.RTS_TBL003", "SC Sample RTS_TBL003",
    "sc_rts003.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};


/*
** Sample RTS Table Data
*/
uint16 RTS_Table003[SC_RTS_BUFF_SIZE] =
{
  /*  cmd time,  <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
     CMD3_TIME,  CFE_MAKE_BIG16(HC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD3_LENGTH), ((HC_SET_STATUS_CC << 8) | CMD3_XSUM), 0x0101,
     CMD4_TIME,  CFE_MAKE_BIG16(HC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD4_LENGTH), ((HC_SET_STATUS_CC << 8) | CMD4_XSUM), 0x0100 ,
     CMD2_TIME,  CFE_MAKE_BIG16(SIM_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD2_LENGTH), ((SIM_HEATER_MISBEHAVE_CC << 8) | CMD2_XSUM), 1,
     CMD1_TIME,  CFE_MAKE_BIG16(LC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD1_LENGTH), ((LC_SET_LC_STATE_CC << 8) | CMD1_XSUM), 1, 0 
};

/************************/
/*  End of File Comment */
/************************/
