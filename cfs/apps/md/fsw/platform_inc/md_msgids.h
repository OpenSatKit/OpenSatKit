/*************************************************************************
** File: md_msgids.h 
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
**   Specification for the CFS Memory Dwell macro constants that can
**   be configured from one mission to another
**
**
** Notes:
**
** 
*************************************************************************/
#ifndef _md_msgids_h_
#define _md_msgids_h_

/*************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name MD Telemetry Message Ids  */ 
/** \{ */
#define MD_HK_TLM_MID         0x0890 /**< \brief Message Id for Memory Dwell's housekeeping pkt */
#define MD_DWELL_PKT_MID_BASE 0x0891 /**< \brief Base Message Id for Memory Dwell's dwell packets.  MIDs will be base, base + 1, etc.  */

/** \} */

/**
** \name MD Command Message Ids */ 
/** \{ */
#define MD_CMD_MID            0x1890 /**< \brief Message Id for Memory Dwell's ground command */
#define MD_SEND_HK_MID        0x1891 /**< \brief Message Id for Memory Dwell's 'Send Housekeeping' message */
#define MD_WAKEUP_MID         0x1892 /**< \brief Message Id for Memory Dwell's wakeup message */
/** \} */


#endif /*_md_msgids_h_*/

/************************/
/*  End of File Comment */
/************************/
