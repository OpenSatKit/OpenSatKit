/*************************************************************************
** File: md_dwell_pkt.h 
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
**   Specification for functions used to populate and send Memory Dwell packets.
**
**
** Notes:
**
** 
*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _md_dwell_pkt_h_
#define _md_dwell_pkt_h_

#include "common_types.h"



/*****************************************************************************/
/**
** \brief Process Dwell Packets
**
** \par Description
**      Look at each table.  If the table is enabled and
**      its countdown counter times out, add dwell data to dwell packet until a 
**      packet delay is reached or the dwell packet is completed.
**      If dwell packet is completed, send the packet and reset the counter.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval None
******************************************************************************/
void MD_DwellLoop( void );


/*****************************************************************************/
/**
** \brief Read a Single Dwell Address
**
** \par Description
**          Copy memory value from a single dwell address to a dwell packet.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblIndex identifies source dwell table (0..)
**
** \param[in] EntryIndex identifies dwell entry within specified table (0..)
**                                      
** \retval CFE_SUCCESS if copy was performed successfully; non-zero otherwise.
******************************************************************************/
int32 MD_GetDwellData( uint16 TblIndex, uint16 EntryIndex );


/*****************************************************************************/
/**
** \brief Send Memory Dwell Packet
**
** \par Description
**          Send contents of memory pointed to by a table as a
**          telemetry packet.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TableIndex Identifies source dwell table for the dwell packet.
**                                      
** \retval None
******************************************************************************/
void MD_SendDwellPkt( uint16 TableIndex );

/*****************************************************************************/
/**
** \brief Start Dwell Stream
**
** \par Description
**          Initialize parameters used to control generation of dwell packets.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TableIndex Identifies source dwell table for the dwell packet.
**                                      
** \retval None
******************************************************************************/
void MD_StartDwellStream (uint16 TableIndex );




#endif /* _md_dwell_pkt_ */
/************************/
/*  End of File Comment */
/************************/
