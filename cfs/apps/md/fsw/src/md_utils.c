/*************************************************************************
** File: md_utils.c
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
**
** Purpose: 
**   Utility functions used for processing CFS Memory Dwell commands
**
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "md_utils.h"
#include "md_app.h"
#include <string.h>
extern MD_AppData_t MD_AppData;


/******************************************************************************/

bool MD_TableIsInMask(int16 TableId, uint16 TableMask)
{
   uint16  LocalMask = TableMask;
   bool Status = false;
   
   /* Shift TableId - 1 times */
   if (TableId - 1)
   {
      LocalMask = LocalMask >> (TableId -1);
   }
   
   /* If result is odd, */
   /* then table is in mask. */
   if ( (LocalMask & (uint16) 1) == (uint16) 1)
      Status = true;
    
   return Status;
   
} /* End MD_TableIsInMask */

/******************************************************************************/

void MD_UpdateDwellControlInfo (uint16 TableIndex)
{
    /* Initialize local data */
    uint16 EntryIndex               = 0;
    uint16 NumDwellAddresses        = 0;
    uint16 NumDwellDataBytes        = 0;
    uint32 NumDwellDelayCounts      = 0;
    MD_DwellPacketControl_t *TblPtr = &MD_AppData.MD_DwellTables[TableIndex]; 
    
    /* 
    ** Sum Address Count, Data Size, and Rate from Dwell Entries 
    */
    while ((EntryIndex <  MD_DWELL_TABLE_SIZE) &&
            (TblPtr->Entry[EntryIndex].Length != 0))
    {
        NumDwellAddresses++;
        NumDwellDataBytes   += TblPtr->Entry[EntryIndex].Length;
        NumDwellDelayCounts += TblPtr->Entry[EntryIndex].Delay;
        EntryIndex++;
    }
    
    /* Copy totals to dwell control structure. */
    TblPtr->AddrCount = NumDwellAddresses;
    TblPtr->DataSize  = NumDwellDataBytes;
    TblPtr->Rate      = NumDwellDelayCounts;
    
    return;
} /* MD_UpdateDwellControlInfo */

/******************************************************************************/

/*
**  Data Validation Functions
*/
/******************************************************************************/
bool MD_ValidEntryId            ( uint16 EntryId )
{
    bool IsValid = false;
    
    if ((EntryId >= 1) && ( EntryId <= MD_DWELL_TABLE_SIZE ))  
    {
        /* validate  value (1..MD_DWELL_TABLE_SIZE ) */
        IsValid = true;
    }
    
    return IsValid;
}


/******************************************************************************/

bool MD_ValidAddrRange( cpuaddr Addr, uint32 Size )
{
    bool IsValid = false;

    if ( CFE_PSP_MemValidateRange (Addr,Size, CFE_PSP_MEM_ANY) == OS_SUCCESS ) 
    {
        IsValid = true;
    }
    
    return IsValid;
}
/******************************************************************************/
bool MD_ValidTableId( uint16 TableId)
{
    bool IsValid = false;
    
    if ((TableId >= 1) && (TableId <= MD_NUM_DWELL_TABLES)) 
    {
       IsValid=true;
    }
      
    return IsValid;
}

/******************************************************************************/

bool MD_ValidFieldLength( uint16 FieldLength)
{

    bool IsValid = false;

    if ( (FieldLength == 0)  || 
         (FieldLength == 1)  ||
         (FieldLength == 2)  || 
         (FieldLength == 4) )
    {
        IsValid= true;
    }
    
    return IsValid;
}

/******************************************************************************/

/************************/
/*  End of File Comment */
/************************/
