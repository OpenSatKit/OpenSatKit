/*************************************************************************
** File: md_utils.h
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
**   Specification for the CFS Memory Dwell utility functions.
**
** Notes:
**
** 
*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _md_utils_h_
#define _md_utils_h_

/*************************************************************************
** Includes
*************************************************************************/

/* md_msg needs to be included for MD_SymAddr_t definition */ 
#include "md_msg.h"
#include "cfe.h"



/*****************************************************************************/
/**
** \brief  Determine if specified TableId is contained in argument mask.   
**
** \par Description
**          Determines whether specified Table Id is contained in argument mask.
**
** \param[in] TableId    identifies dwell table  (1..#MD_NUM_DWELL_TABLES)
**
** \param[in] TableMask  Mask representing current status of all dwell tables.
**
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
******************************************************************************/
bool MD_TableIsInMask(int16 TableId, uint16 TableMask);

/*****************************************************************************/
/**
** \brief Update Dwell Table Control Info
**
** \par Description
**          Updates the control structure used by the application for
**          dwell packet processing with address count, data size, and rate.
** 
** \par Assumptions, External Events, and Notes:
**          A zero value for length in a dwell table entry
**    represents the end of the active portion of a dwell table.
**
** \param[in] TableIndex identifies dwell control structure  (0..#MD_NUM_DWELL_TABLES-1)
**                                      
** \retval None
******************************************************************************/
void MD_UpdateDwellControlInfo (uint16 TableIndex);

/*****************************************************************************/
/**
** \brief Validate Entry Index
**
** \par Description
**        Checks for valid value (1..MD_DWELL_TABLE_SIZE ) for entry id
**        specified in Jam command.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] EntryId  EntryId (starting at one) for dwell control structure entry.
**                                      
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
**
******************************************************************************/
bool MD_ValidEntryId            ( uint16 EntryId );

/*****************************************************************************/
/**
** \brief Validate Dwell Address
**
** \par Description
**        This function validates that the memory range as specified by the
**        input address and size is valid for reading.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] Addr  Dwell address.
**                                      
** \param[in] Size Size, in bytes, of field to be read.
**
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
**
******************************************************************************/
bool MD_ValidAddrRange( cpuaddr Addr, uint32 Size );

/*****************************************************************************/
/**
** \brief Validate Table ID
**
** \par Description
**        Check valid range for TableId argument used in several
**        Memory Dwell commands.
**        Valid range is 1..#MD_NUM_DWELL_TABLES.
** 
** \par Assumptions, External Events, and Notes:
**        Note that this value will be internally converted to 
**        0..(#MD_NUM_DWELL_TABLES-1) for indexing into arrays.
**
** \param[in] TableId  Table ID.
**                                      
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
**
******************************************************************************/
bool MD_ValidTableId( uint16 TableId );

/*****************************************************************************/
/**
** \brief Validate Field Length
**
** \par Description
**        Check valid range for dwell field length.
** 
** \par Assumptions, External Events, and Notes:
**   Valid values for dwell field length are 0, 1, 2, and 4.
**   0 corresponds to a null entry in Dwell Table.
**
** \param[in] FieldLength  Length of field, in bytes, to be copied for dwell.
**                                      
** \returns
** \retstmt Returns TRUE or FALSE   
** \endreturns
**
******************************************************************************/
bool MD_ValidFieldLength(uint16 FieldLength);





#endif /* _md_utils_ */
/************************/
/*  End of File Comment */
/************************/
