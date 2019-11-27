/*
** $Id: fm_tbl.c 1.3.1.2 2017/01/23 21:53:22EST sstrege Exp  $
**
**  Copyright (c) 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Title: File Manager (FM) Application Table Definitions
**
** Purpose: Provides functions for the initialization, validation, and
**          management of the FM File System Free Space Table
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
**
*/

#include "cfe.h"
#include "fm_platform_cfg.h"
#include "fm_msg.h"
#include "fm_tbl.h"
#include "fm_events.h"
#include "cfs_utils.h"

#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM table function -- startup initialization                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 FM_TableInit(void)
{
    int32 Status;

    /* Initialize file system free space table pointer */
    FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;

    /* Register the file system free space table - this must succeed! */
    Status = CFE_TBL_Register(&FM_GlobalData.FreeSpaceTableHandle,
                               FM_TABLE_CFE_NAME, sizeof(FM_FreeSpaceTable_t),
                             (CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP),
                             (CFE_TBL_CallbackFuncPtr_t) FM_ValidateTable);

    if (Status == CFE_SUCCESS)
    {
        /* Make an attempt to load the default table data - OK if this fails */
        CFE_TBL_Load(FM_GlobalData.FreeSpaceTableHandle,
                     CFE_TBL_SRC_FILE, FM_TABLE_DEF_NAME);

        /* Allow cFE a chance to dump, update, etc. */
        FM_AcquireTablePointers();
    }

    return(Status);

} /* End FM_TableInit */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM table function -- table data verification                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 FM_ValidateTable(void *TableData)
{
    FM_FreeSpaceTable_t *TablePtr = (FM_FreeSpaceTable_t *) TableData;
    int32  Result = CFE_SUCCESS;
    int32  NameLength;
    int32  i;

    int32 CountGood   = 0;
    int32 CountBad    = 0;
    int32 CountUnused = 0;

    /*
    ** Free space table data verification
    **
    ** -- table entries must be enabled or disabled or unused
    **
    ** -- enabled table entries may be disabled by command
    ** -- disabled table entries may be enabled by command
    ** -- unused table entries cannot be modified by command
    **
    ** -- enabled or disabled entries must have a valid file system name
    **
    ** -- file system name for unused entries is ignored
    */
    for (i = 0; i < FM_TABLE_ENTRY_COUNT; i++)
    {
        /* Validate file system name if state is enabled or disabled */
        if ((TablePtr->FileSys[i].State == FM_TABLE_ENTRY_ENABLED) ||
            (TablePtr->FileSys[i].State == FM_TABLE_ENTRY_DISABLED))
        {
            /* Search file system name buffer for a string terminator */
            for (NameLength = 0; NameLength < OS_MAX_PATH_LEN; NameLength++)
            {
                if (TablePtr->FileSys[i].Name[NameLength] == '\0')
                {
                    break;
                }
            }

            if (NameLength == 0)
            {
                /* Error - must have a non-zero file system name length */
                CountBad++;

                /* Send event describing first error only*/
                if (CountBad == 1)
                {
                    CFE_EVS_SendEvent(FM_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                       "Free Space Table verify error: index = %d, empty name string", (int)i);
                }
            }
            else if (NameLength == OS_MAX_PATH_LEN)
            {
                /* Error - file system name does not have a string terminator */
                CountBad++;

                /* Send event describing first error only*/
                if (CountBad == 1)
                {
                    CFE_EVS_SendEvent(FM_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                       "Free Space Table verify error: index = %d, name too long", (int)i);
                }
            }
            else if (!CFS_IsValidFilename(TablePtr->FileSys[i].Name, NameLength))
            {
                /* Error - file system name has invalid characters */
                CountBad++;

                /* Send event describing first error only*/
                if (CountBad == 1)
                {
                    CFE_EVS_SendEvent(FM_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                       "Free Space Table verify error: index = %d, invalid name = %s",
                                      (int)i, TablePtr->FileSys[i].Name);
                }
            }
            else
            {
                /* Maintain count of good in-use table entries */
                CountGood++;
            }
        }
        else if (TablePtr->FileSys[i].State == FM_TABLE_ENTRY_UNUSED)
        {
            /* Ignore (but count) unused table entries */
            CountUnused++;
        }
        else
        {
            /* Error - table entry state is invalid */
            CountBad++;

            /* Send event describing first error only*/
            if (CountBad == 1)
            {
                CFE_EVS_SendEvent(FM_TABLE_VERIFY_ERR_EID, CFE_EVS_ERROR,
                   "Table verify error: index = %d, invalid state = %d",
                                  (int)i, (int)TablePtr->FileSys[i].State);
            }
        }
    }

    /* Display verify results */
    CFE_EVS_SendEvent(FM_TABLE_VERIFY_EID, CFE_EVS_INFORMATION,
       "Free Space Table verify results: good entries = %d, bad = %d, unused = %d",
                      (int)CountGood, (int)CountBad, (int)CountUnused);

    if (CountBad != 0)
    {
        Result = FM_TABLE_VALIDATION_ERR;
    }

    return(Result);

} /* End FM_ValidateTable */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM table function -- acquire table data pointer                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_AcquireTablePointers(void)
{
    int32  Status;

    /* Allow cFE an opportunity to make table updates */
    CFE_TBL_Manage(FM_GlobalData.FreeSpaceTableHandle);

    /* Acquire pointer to file system free space table */
    Status = CFE_TBL_GetAddress((void *) &FM_GlobalData.FreeSpaceTablePtr,
                                          FM_GlobalData.FreeSpaceTableHandle);

    if (Status == CFE_TBL_ERR_NEVER_LOADED)
    {
        /* Make sure we don't try to use the empty table buffer */
        FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;
    }

    return;

} /* End FM_AcquireTablePointers */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM table function -- release table data pointer                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ReleaseTablePointers(void)
{
    /* Release pointer to file system free space table */
    CFE_TBL_ReleaseAddress(FM_GlobalData.FreeSpaceTableHandle);

    /* Prevent table pointer use while released */
    FM_GlobalData.FreeSpaceTablePtr = (FM_FreeSpaceTable_t *) NULL;

    return;

} /* End FM_ReleaseTablePointers */


/************************/
/*  End of File Comment */
/************************/

