/************************************************************************
** File: md_tbldefs.h
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
**   Data structure definitions for Memory Dwell table loads.
**
** Notes:
**
** 
*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _md_tbldefs_h_
#define _md_tbldefs_h_

/*
** Required header files...
*/
#include "md_platform_cfg.h"
#include "cfe.h"
/* cfs_utils.h needed for CFS_SymAddr_t */
#include "cfs_utils.h"

/************************************************************************
** Type Definitions
*************************************************************************/

/* 
**  \brief Memory Dwell structure for individual memory dwell specifications
**
**  \par Description
**          To be valid, entry must either be a null entry
**          (specified by a zero field length) or all of its
**          address and length fields must pass various checks.
**
**          The following checks must be met for a valid non-null entry:
**
**          * Length fields must be 1, 2, or 4.
**          * Delay can be any value in a uint16 field.
**          * DwellAddress.SymName must be a null string or must be found in the Symbol Table.
**          * The sum of DwellAddress.SymName plus DwellAddress.Offset must be in a valid range,
**          as determined by OSAL routines.
**          * If the Length field is 2, the sum of DwellAddress.SymName plus DwellAddress.Offset
**          must be even.
**          * If the Length field is 4, the sum of DwellAddress.SymName plus DwellAddress.Offset
**          must be evenly divisable by 4.
*/    
typedef struct
{
	uint16          Length;       /**< \brief  Length of dwell  field in bytes.  0 indicates null entry. */
	uint16          Delay;        /**< \brief  Delay before following dwell sample in terms of number of task wakeup calls */
    CFS_SymAddr_t   DwellAddress; /**< \brief  Dwell Address in #CFS_SymAddr_t format */
} MD_TableLoadEntry_t;

#define MD_TBL_LOAD_ENTRY_LNGTH sizeof(MD_TableLoadEntry_t)

/** 
**  \brief Memory Dwell Table Load structure
**
**  \par Description
**          To be valid, each of the Entry structures must be valid.  See #MD_TableLoadEntry_t for details.
**          Tables will be processed beginning with the first entry if it is non-null 
**          and continuing until the first null entry is reached.
**          Note that non-null entries may follow a terminator entry; however they will not be processed.
**
**          In order to be processed, all of the following must be true: 
**          # There are one or more non-null entries beginning with the first entry of the table.
**          # The sum of individual entry delays, beginning with the first entry and up until the 
**            terminator entry or the end of the table, must be non-zero.
**          # The table's Enabled field must be set to TRUE.  This is initially set in the load, and
**            is controlled with the #MD_START_DWELL_CC and #MD_STOP_DWELL_CC commands.
*/    
typedef struct
{
    uint32 Enabled; /**< \brief Is table is enabled for dwell?  Valid values are MD_DWELL_STREAM_DISABLED, MD_DWELL_STREAM_ENABLED */
#if MD_SIGNATURE_OPTION == 1  
    char Signature[MD_SIGNATURE_FIELD_LENGTH];
#endif
    MD_TableLoadEntry_t Entry[MD_DWELL_TABLE_SIZE];  /**< \brief Array of individual memory dwell specifications */
} MD_DwellTableLoad_t;

#define MD_TBL_LOAD_LNGTH        sizeof(MD_DwellTableLoad_t)



/*************************************************************************/

#endif /* _md_tbldefs_h_ */

/************************/
/*  End of File Comment */
/************************/
