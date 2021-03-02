/************************************************************************
** File: ds_table.c 
**
**  NASA Docket No. GSC-18448-1, and identified as "cFS Data Storage (DS) 
**  application version 2.5.2” 
**  
**  Copyright © 2019 United States Government as represented by the Administrator 
**  of the National Aeronautics and Space Administration.  All Rights Reserved. 
**
**  Licensed under the Apache License, Version 2.0 (the "License"); 
**  you may not use this file except in compliance with the License. 
**  You may obtain a copy of the License at 
**  http://www.apache.org/licenses/LICENSE-2.0 
**  Unless required by applicable law or agreed to in writing, software 
**  distributed under the License is distributed on an "AS IS" BASIS, 
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
**  See the License for the specific language governing permissions and 
**  limitations under the License. 
**  
** Purpose:
**  CFS Data Storage (DS) table management functions
**
*************************************************************************/

#include "cfe.h"

#include "cfs_utils.h"

#include "ds_msgids.h"

#include "ds_platform_cfg.h"
#include "ds_verify.h"

#include "ds_appdefs.h"

#include "ds_app.h"
#include "ds_table.h"
#include "ds_msg.h"
#include "ds_events.h"


#define DS_CDS_NAME  "DS_CDS"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableInit() - DS application table initialization            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 DS_TableInit(void)
{
    int32 Result1 = CFE_SUCCESS;
    int32 Result2 = CFE_SUCCESS;
    bool NeedToLoadDestTable = false;
    bool NeedToLoadFilterTable = false;
    uint16 TableRegisterFlags = CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP;

    #if (DS_MAKE_TABLES_CRITICAL == 1)
    TableRegisterFlags |= CFE_TBL_OPT_CRITICAL;
    #endif

    /*
    ** If registration fails for either table then the DS app will
    **   terminate immediately. Without valid table handles, the DS
    **   app will never be able to load table data.
    **
    ** However, as long as both tables were successfully registered,
    **   it doesn't matter that one or both table loads fail.  The
    **   DS app can still continue - or at least the DS app can limp
    **   along until an external influence (ground or RTS) can manage
    **   to get both tables loaded.
    */
    Result1 = CFE_TBL_Register(&DS_AppData.DestFileTblHandle, DS_DESTINATION_TBL_NAME,
                                sizeof (DS_DestFileTable_t), TableRegisterFlags,
                               (CFE_TBL_CallbackFuncPtr_t) DS_TableVerifyDestFile);

    if (Result1 == CFE_TBL_INFO_RECOVERED_TBL)
    {
        /*
        ** cFE registered the table and restored the table data
        */
        NeedToLoadDestTable = false;

        CFE_EVS_SendEvent(DS_INIT_TBL_CDS_EID, CFE_EVS_EventType_DEBUG,
                         "Destination File Table data restored from CDS");
        /*
        ** This is not an error so clear the result value for later tests
        */
        Result1 = CFE_SUCCESS;
    }
    else if (Result1 == CFE_SUCCESS)
    {
        /*
        ** cFE registered the table - we need to load the table data
        */
        NeedToLoadDestTable = true;
    }
    else
    {
        /*
        ** cFE did not register the table - we cannot continue
        */ 
        CFE_EVS_SendEvent(DS_INIT_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                         "Unable to register Destination File Table: Error = 0x%08X",
                          (unsigned int)Result1);
    }

    if (Result1 == CFE_SUCCESS)
    {
        Result1 = CFE_TBL_Register(&DS_AppData.FilterTblHandle, DS_FILTER_TBL_NAME,
                                    sizeof (DS_FilterTable_t), TableRegisterFlags,
                                   (CFE_TBL_CallbackFuncPtr_t) DS_TableVerifyFilter);

        if (Result1 == CFE_TBL_INFO_RECOVERED_TBL)
        {
            /*
            ** cFE registered the table and restored the table data
            */
            NeedToLoadFilterTable = false;

            CFE_EVS_SendEvent(DS_INIT_TBL_CDS_EID, CFE_EVS_EventType_DEBUG,
                             "Filter Table data restored from CDS");
            /*
            ** This is not an error so clear the result value for later tests
            */
            Result1 = CFE_SUCCESS;
        }
        else if (Result1 == CFE_SUCCESS)
        {
            /*
            ** cFE registered the table - we need to load the table data
            */
            NeedToLoadFilterTable = true;
        }
        else
        {
            /*
            ** cFE did not register the table - we cannot continue
            */ 
            CFE_EVS_SendEvent(DS_INIT_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                             "Unable to register Filter Table: Error = 0x%08X",
                              (unsigned int)Result1);
        }
    }

    /*
    ** Now load the tables - but only if the registration succeeded
    **   and the table data has not already been restored from the
    **   Critical Data Store.
    */
    if (Result1 == CFE_SUCCESS)
    {
        if (NeedToLoadDestTable)
        {
            Result2 = CFE_TBL_Load(DS_AppData.DestFileTblHandle,
                                   CFE_TBL_SRC_FILE, DS_DEF_DEST_FILENAME);

            if (Result2 != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(DS_INIT_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                   "Unable to load default Destination File Table: Filename = '%s', Error = 0x%08X",
                                  DS_DEF_DEST_FILENAME, (unsigned int)Result2);
            }
        }

        if (NeedToLoadFilterTable)
        {
            Result2 = CFE_TBL_Load(DS_AppData.FilterTblHandle,
                                   CFE_TBL_SRC_FILE, DS_DEF_FILTER_FILENAME);

            if (Result2 != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(DS_INIT_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                   "Unable to load default Filter Table: Filename = '%s', Error = 0x%08X",
                                  DS_DEF_FILTER_FILENAME, (unsigned int)Result2);
            }
        }

        /*
        ** Get initial table data pointers...
        */
        DS_TableManageDestFile();
        DS_TableManageFilter();
    }

    return(Result1);

} /* End of DS_TableInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableManageDestFile() - manage table data updates            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void DS_TableManageDestFile(void)
{
    int32 i = 0, Result = CFE_SUCCESS;

    /*
    ** Pointer will be NULL until first successful table load...
    */
    if (DS_AppData.DestFileTblPtr == (DS_DestFileTable_t *) NULL)
    {
        /*
        ** Still waiting for the first table load...
        */
        CFE_TBL_ReleaseAddress(DS_AppData.DestFileTblHandle);
        CFE_TBL_Manage(DS_AppData.DestFileTblHandle);
        Result = CFE_TBL_GetAddress((void *)&DS_AppData.DestFileTblPtr,
                                             DS_AppData.DestFileTblHandle);

        if (Result == CFE_TBL_INFO_UPDATED)
        {
            /*
            ** Got a pointer to initial table data...
            */
            DS_AppData.DestTblLoadCounter++;

            /*
            ** Keep local copies of table values that software will modify...
            */
            for (i = 0; i < DS_DEST_FILE_CNT; i++)
            {
                DS_AppData.FileStatus[i].FileState = DS_AppData.DestFileTblPtr->File[i].EnableState;
                DS_AppData.FileStatus[i].FileCount = DS_AppData.DestFileTblPtr->File[i].SequenceCount;
            }

            /*
            ** Store local values in the Critical Data Store (CDS)...
            */
            DS_TableUpdateCDS();
        }
        else if (Result == CFE_TBL_ERR_NEVER_LOADED)
        {
            /*
            ** Still waiting for the first table load...
            */
            DS_AppData.DestTblErrCounter++;

            /*
            ** Make sure we don't try to use the empty table buffer...
            */
            DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
        }
    }
    else
    {
        /*
        ** Already have initial table data...
        */
        Result = CFE_TBL_GetStatus(DS_AppData.DestFileTblHandle);

        if (Result == CFE_TBL_INFO_DUMP_PENDING)
        {
            /*
            ** Dump the current table data...
            */
            CFE_TBL_DumpToBuffer(DS_AppData.DestFileTblHandle);       
        }
        else if (Result == CFE_TBL_INFO_VALIDATION_PENDING)
        {
            /*
            ** Validate the pending table data...
            */
            CFE_TBL_Validate(DS_AppData.DestFileTblHandle);
        }
        else if (Result == CFE_TBL_INFO_UPDATE_PENDING)
        {
            /*
            ** Update the current table with new data...
            */
            DS_AppData.DestTblLoadCounter++;

            /*
            ** Allow cFE to update the table data...
            */
            CFE_TBL_ReleaseAddress(DS_AppData.DestFileTblHandle);
            CFE_TBL_Update(DS_AppData.DestFileTblHandle);
            CFE_TBL_GetAddress((void *)&DS_AppData.DestFileTblPtr,
                                        DS_AppData.DestFileTblHandle);
            /*
            ** Keep local copies of table values that software will modify...
            */
            for (i = 0; i < DS_DEST_FILE_CNT; i++)
            {
                DS_AppData.FileStatus[i].FileState = DS_AppData.DestFileTblPtr->File[i].EnableState;
                DS_AppData.FileStatus[i].FileCount = DS_AppData.DestFileTblPtr->File[i].SequenceCount;
            }

            /*
            ** Store local values in the Critical Data Store (CDS)...
            */
            DS_TableUpdateCDS();
        }
    }

    return;

} /* End of DS_TableManageDestFile() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableManageFilter() - manage table data updates              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void DS_TableManageFilter(void)
{
    int32 Result = CFE_SUCCESS;

    /*
    ** Pointer will be NULL until first successful filter table load...
    */
    if (DS_AppData.FilterTblPtr == (DS_FilterTable_t *) NULL)
    {
        /*
        ** Still waiting for the first filter table load...
        */
        CFE_TBL_ReleaseAddress(DS_AppData.FilterTblHandle);
        CFE_TBL_Manage(DS_AppData.FilterTblHandle);
        Result = CFE_TBL_GetAddress((void *)&DS_AppData.FilterTblPtr,
                                             DS_AppData.FilterTblHandle);

        if (Result == CFE_TBL_INFO_UPDATED)
        {
            /*
            ** Got a pointer to initial filter table data...
            */
            DS_AppData.FilterTblLoadCounter++;

            /*
            ** Subscribe to the packets in the new filter table...
            */
            DS_TableSubscribe();

            /*
            ** Create hash table for messageID's in new filter table...
            */
            DS_TableCreateHash();
        }
        else if (Result == CFE_TBL_ERR_NEVER_LOADED)
        {
            /*
            ** Still waiting for the first filter table load...
            */
            DS_AppData.FilterTblErrCounter++;

            /*
            ** Make sure we don't try to use the empty table buffer...
            */
            DS_AppData.FilterTblPtr = (DS_FilterTable_t *) NULL;
        }
    }
    else
    {
        /*
        ** Already have initial filter table data...
        */
        Result = CFE_TBL_GetStatus(DS_AppData.FilterTblHandle);

        if (Result == CFE_TBL_INFO_DUMP_PENDING)
        {
            /*
            ** Dump the current filter table data...
            */
            CFE_TBL_DumpToBuffer(DS_AppData.FilterTblHandle);       
        }
        else if (Result == CFE_TBL_INFO_VALIDATION_PENDING)
        {
            /*
            ** Validate the pending filter table data...
            */
            CFE_TBL_Validate(DS_AppData.FilterTblHandle);
        }
        else if (Result == CFE_TBL_INFO_UPDATE_PENDING)
        {
            /*
            ** Update the current filter table with new data...
            */
            DS_AppData.FilterTblLoadCounter++;

            /*
            ** Un-subscribe to the packets in the old filter table...
            */
            DS_TableUnsubscribe();

            /*
            ** Allow cFE to update the filter table data...
            */
            CFE_TBL_ReleaseAddress(DS_AppData.FilterTblHandle);
            CFE_TBL_Update(DS_AppData.FilterTblHandle);
            CFE_TBL_GetAddress((void *)&DS_AppData.FilterTblPtr,
                                        DS_AppData.FilterTblHandle);
            /*
            ** Subscribe to the packets in the new filter table...
            */
            DS_TableSubscribe();

            /*
            ** Create hash table for messageID's in new filter table...
            */
            DS_TableCreateHash();
        }
    }

    return;

} /* End of DS_TableManageFilter() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyDestFile() - validate table data                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 DS_TableVerifyDestFile(void *TableData)
{
    DS_DestFileTable_t *DestFileTable = (DS_DestFileTable_t *) TableData;
    const char *DescResult = "OK";
    int32 Result = CFE_SUCCESS;
    int32 i = 0;

    int32 CountGood   = 0;
    int32 CountBad    = 0;
    int32 CountUnused = 0;

    /*
    ** Perform the following "per table" validation:
    **
    **   Descriptor = zero terminated text string (optional)
    */
    if (CFS_VerifyString(DestFileTable->Descriptor, DS_DESCRIPTOR_BUFSIZE,
                         DS_STRING_OPTIONAL, DS_DESCRIPTIVE_TEXT) == false)
    {
        CFE_EVS_SendEvent(DS_FIL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                         "Destination file table verify err: invalid descriptor text");

        DescResult = "bad";
        Result = DS_TABLE_VERIFY_ERR;
    }

    /*
    ** Each entry in table will be unused, good or bad
    */
    for (i = 0; i < DS_DEST_FILE_CNT; i++)
    {
        if (DS_TableEntryUnused(&DestFileTable->File[i], sizeof(DS_DestFileEntry_t)) == true)
        {
            CountUnused++;
        }
        else if (DS_TableVerifyDestFileEntry(&DestFileTable->File[i], (uint8) i, CountBad) == true)
        {
            CountGood++;
        }
        else
        {
            CountBad++;
            Result = DS_TABLE_VERIFY_ERR;
        }
    }

    /*
    ** Note that totals include each table entry plus the descriptor
    */
    CFE_EVS_SendEvent(DS_FIL_TBL_EID, CFE_EVS_EventType_INFORMATION,
       "Destination file table verify results: desc text = %s, good entries = %d, bad = %d, unused = %d",
                      DescResult, (int)CountGood, (int)CountBad, (int)CountUnused);

    return(Result);

} /* End of DS_TableVerifyDestFile() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyDestFileEntry() - verify dest table entry         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableVerifyDestFileEntry(DS_DestFileEntry_t *DestFileEntry,
                                    uint8 TableIndex, int32 ErrorCount)
{
    const char *CommonErrorText = "Destination file table verify err:";
    bool Result = true;

    /*
    ** Perform the following "per table entry" validation:
    **
    **   Pathname  = zero terminated text string (required)
    **   Basename  = zero terminated text string (optional)
    **   Extension = zero terminated text string (optional)
    **
    **  FileNameType = DS_BY_COUNT or DS_BY_TIME
    **  EnableState  = DS_ENABLED or DS_DISABLED
    **
    **  MaxFileSize   = cannot be less than DS_FILE_MIN_SIZE_LIMIT
    **  MaxFileAge    = cannot be less than DS_FILE_MIN_AGE_LIMIT
    **  SequenceCount = may be zero, cannot exceed DS_MAX_SEQUENCE_COUNT
    */
    if (CFS_VerifyString(DestFileEntry->Pathname, DS_PATHNAME_BUFSIZE,
                         DS_STRING_REQUIRED, DS_FILENAME_TEXT) == false)
    {
        if (ErrorCount == 0)
        {
            CFE_EVS_SendEvent(DS_FIL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                             "%s index = %d, invalid pathname text",
                              CommonErrorText, TableIndex);
        }
        Result = false;
    }
    else if (CFS_VerifyString(DestFileEntry->Basename, DS_BASENAME_BUFSIZE,
                              DS_STRING_OPTIONAL, DS_FILENAME_TEXT) == false)
    {
        if (ErrorCount == 0)
        {
            CFE_EVS_SendEvent(DS_FIL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                             "%s index = %d, invalid basename text",
                              CommonErrorText, TableIndex);
        }
        Result = false;
    }
    else if (CFS_VerifyString(DestFileEntry->Extension, DS_EXTENSION_BUFSIZE,
                              DS_STRING_OPTIONAL, DS_FILENAME_TEXT) == false)
    {
        if (ErrorCount == 0)
        {
            CFE_EVS_SendEvent(DS_FIL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                             "%s index = %d, invalid extension text",
                              CommonErrorText, TableIndex);
        }
        Result = false;
    }
    else if (DS_TableVerifyType(DestFileEntry->FileNameType) == false)
    {
        if (ErrorCount == 0)
        {
            CFE_EVS_SendEvent(DS_FIL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                             "%s index = %d, filename type = %d",
                              CommonErrorText, TableIndex, DestFileEntry->FileNameType);
        }
        Result = false;
    }
    else if (DS_TableVerifyState(DestFileEntry->EnableState) == false)
    {
        if (ErrorCount == 0)
        {
            CFE_EVS_SendEvent(DS_FIL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                             "%s index = %d, file enable state = %d",
                              CommonErrorText, TableIndex, DestFileEntry->EnableState);
        }
        Result = false;
    }
    else if (DS_TableVerifySize(DestFileEntry->MaxFileSize) == false)
    {
        if (ErrorCount == 0)
        {
            CFE_EVS_SendEvent(DS_FIL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                             "%s index = %d, max file size = %d",
                              CommonErrorText, (int)TableIndex, (int)DestFileEntry->MaxFileSize);
        }
        Result = false;
    }
    else if (DS_TableVerifyAge(DestFileEntry->MaxFileAge) == false)
    {
        if (ErrorCount == 0)
        {
            CFE_EVS_SendEvent(DS_FIL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                             "%s index = %d, max file age = %d",
                              CommonErrorText, (int)TableIndex, (int)DestFileEntry->MaxFileAge);
        }
        Result = false;
    }
    else if (DS_TableVerifyCount(DestFileEntry->SequenceCount) == false)
    {
        if (ErrorCount == 0)
        {
            CFE_EVS_SendEvent(DS_FIL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                             "%s index = %d, sequence count = %d",
                              CommonErrorText, (int)TableIndex, (int)DestFileEntry->SequenceCount);
        }
        Result = false;
    }

    return(Result);

} /* End of DS_TableVerifyDestFileEntry() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyFilter() - validate table data                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 DS_TableVerifyFilter(void *TableData)
{
    DS_FilterTable_t *FilterTable = (DS_FilterTable_t *) TableData;
    const char *DescResult = "OK";
    int32 Result = CFE_SUCCESS;
    int32 i = 0;

    int32 CountGood   = 0;
    int32 CountBad    = 0;
    int32 CountUnused = 0;

    /*
    ** Perform the following validation:
    **
    **   Descriptor = zero terminated text string (may be empty)
    **
    **   MessageID = unlimited, zero means unused
    */
    if (CFS_VerifyString(FilterTable->Descriptor, DS_DESCRIPTOR_BUFSIZE,
                         DS_STRING_OPTIONAL, DS_DESCRIPTIVE_TEXT) == false)
    {
        CFE_EVS_SendEvent(DS_FLT_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                         "Filter table verify err: invalid descriptor text");

        DescResult = "bad";
        Result = DS_TABLE_VERIFY_ERR;
    }

    /*
    ** Each entry in table will be unused, good or bad
    */
    for (i = 0; i < DS_PACKETS_IN_FILTER_TABLE; i++)
    {
        if (FilterTable->Packet[i].MessageID == DS_UNUSED)
        {
            CountUnused++;
        }
        else if (DS_TableVerifyFilterEntry(&FilterTable->Packet[i], (uint8) i, CountBad) == true)
        {
            CountGood++;
        }
        else
        {
            CountBad++;
            Result = DS_TABLE_VERIFY_ERR;
        }
    }

    /*
    ** Note that totals include each table entry plus the descriptor
    */
    CFE_EVS_SendEvent(DS_FLT_TBL_EID, CFE_EVS_EventType_INFORMATION,
       "Filter table verify results: desc text = %s, good entries = %d, bad = %d, unused = %d",
                      DescResult, (int)CountGood, (int)CountBad, (int)CountUnused);

    return(Result);

} /* End of DS_TableVerifyFilter() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyFilterEntry() - verify filter table entry         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableVerifyFilterEntry(DS_PacketEntry_t *PacketEntry,
                                  int32 TableIndex, int32 ErrorCount)
{
    const char *CommonErrorText = "Filter table verify err:";
    DS_FilterParms_t *FilterParms;
    bool Result = true;
    int32 i = 0;

    /*
    ** Each packet filter table entry has multiple filters per packet
    **
    ** Perform the following validation (per filter):
    **
    **   FileTableIndex = must be less than DS_DEST_FILE_CNT
    **   FilterType = must be DS_BY_COUNT or DS_BY_TIME
    **
    **   Algorithm_N = cannot exceed Algorithm_X, zero means filter ALL
    **   Algorithm_X = unlimited
    **   Algorithm_O = must be less than Algorithm_X
    **
    **   Note: unused filters (all zero's) are valid
    */
    for (i = 0; (i < DS_FILTERS_PER_PACKET) && (Result == true); i++)
    {
        FilterParms = &PacketEntry->Filter[i];

        if (DS_TableEntryUnused(FilterParms, sizeof(DS_FilterParms_t)) == false)
        {
            /*
            ** If any filter field is non-zero then all filter fields must be valid
            */
            if (DS_TableVerifyFileIndex((uint32) FilterParms->FileTableIndex) == false)
            {
                if (ErrorCount == 0)
                {
                    CFE_EVS_SendEvent(DS_FLT_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                                     "%s MID = 0x%04X, index = %d, filter = %d, file table index = %d",
                                      CommonErrorText, PacketEntry->MessageID,
                                      (int)TableIndex, (int)i, FilterParms->FileTableIndex);
                }
                Result = false;
            }
            else if (DS_TableVerifyType((uint16) FilterParms->FilterType) == false)
            {
                if (ErrorCount == 0)
                {
                    CFE_EVS_SendEvent(DS_FLT_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                                     "%s MID = 0x%04X, index = %d, filter = %d, filter type = %d",
                                      CommonErrorText, PacketEntry->MessageID,
                                      (int)TableIndex, (int)i, FilterParms->FilterType);
                }
                Result = false;
            }
            else if (DS_TableVerifyParms(FilterParms->Algorithm_N,
                                         FilterParms->Algorithm_X,
                                         FilterParms->Algorithm_O) == false)
            {
                if (ErrorCount == 0)
                {
                    CFE_EVS_SendEvent(DS_FLT_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                                     "%s MID = 0x%04X, index = %d, filter = %d, filter parms N = %d, X = %d, O = %d",
                                      CommonErrorText, PacketEntry->MessageID, (int)TableIndex, (int)i,
                                      FilterParms->Algorithm_N,
                                      FilterParms->Algorithm_X,
                                      FilterParms->Algorithm_O);
                }
                Result = false;
            }
        }
    }

    return(Result);

} /* End of DS_TableVerifyFilterEntry() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableEntryUnused() - find unused table entries               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableEntryUnused(void *TableEntry, int32 BufferSize)
{
    const char *Buffer = (char *) TableEntry;
    bool Result = true;
    int32 i = 0;

    for (i = 0; i < BufferSize; i++)
    {
        if (Buffer[i] != DS_UNUSED)
        {
            Result = false;
            break;
        }
    }

    return(Result);

} /* End of DS_TableEntryUnused() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyFileIndex() - verify dest file index              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableVerifyFileIndex(uint16 FileTableIndex)
{
    bool Result = true;

    if (FileTableIndex >= DS_DEST_FILE_CNT)
    {
        Result = false;
    }

    return(Result);

} /* End of DS_TableVerifyFileIndex() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyParms() - verify algorithm parameters             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableVerifyParms(uint16 Algorithm_N, uint16 Algorithm_X, uint16 Algorithm_O)
{
    bool Result = true;

    /*
    ** Unused entries (all zero's) are valid
    */
    if ((Algorithm_N != 0) || (Algorithm_X != 0) || (Algorithm_O != 0))
    {
        if (Algorithm_N > Algorithm_X)
        {
            /*
            ** "pass this many" cannot exceed "out of this many"
            */
            Result = false;
        }
        else if (Algorithm_O >= Algorithm_X)
        {
            /*
            ** "at this offset" must be less than "out of this many"
            */
            Result = false;
        }
    }

    return(Result);

} /* End of DS_TableVerifyParms() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyType() - verify filter or filename type           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableVerifyType(uint16 TimeVsCount)
{
    bool Result = true;

    if ((TimeVsCount != DS_BY_COUNT) && (TimeVsCount != DS_BY_TIME))
    {
        Result = false;
    }

    return(Result);

} /* End of DS_TableVerifyType() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyState() - verify file ena/dis state               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableVerifyState(uint16 EnableState)
{
    bool Result = true;

    if ((EnableState != DS_ENABLED) && (EnableState != DS_DISABLED))
    {
        Result = false;
    }

    return(Result);

} /* End of DS_TableVerifyState() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifySize() - verify file size limit                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableVerifySize(uint32 MaxFileSize)
{
    bool Result = true;

    if (MaxFileSize < DS_FILE_MIN_SIZE_LIMIT)
    {
        Result = false;
    }

    return(Result);

} /* End of DS_TableVerifySize() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyAge() - verify file age limit                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableVerifyAge(uint32 MaxFileAge)
{
    bool Result = true;

    if (MaxFileAge < DS_FILE_MIN_AGE_LIMIT)
    {
        Result = false;
    }

    return(Result);

} /* End of DS_TableVerifyAge() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableVerifyCount() - verify sequence count                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DS_TableVerifyCount(uint32 SequenceCount)
{
    bool Result = true;

    if (SequenceCount > DS_MAX_SEQUENCE_COUNT)
    {
        Result = false;
    }

    return(Result);

} /* End of DS_TableVerifyCount() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableSubscribe() - process new filter table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void DS_TableSubscribe(void)
{
    DS_PacketEntry_t *FilterPackets = NULL;
    CFE_SB_MsgId_t MessageID = 0;
    int32 i = 0;

    FilterPackets = DS_AppData.FilterTblPtr->Packet;

    /*
    ** Check each entry in "new" packet filter table...
    */
    for (i = 0; i < DS_PACKETS_IN_FILTER_TABLE; i++)
    {
        MessageID = FilterPackets[i].MessageID;

        /*
        ** Already subscribe to DS command packets...
        */
        if ((MessageID != DS_UNUSED) &&
            (MessageID != DS_CMD_MID) &&
            (MessageID != DS_SEND_HK_MID))
        {
            CFE_SB_SubscribeEx(MessageID, DS_AppData.InputPipe,
                CFE_SB_Default_Qos, DS_PER_PACKET_PIPE_LIMIT);
        }
    }

    return;

} /* End of DS_TableSubscribe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableUnsubscribe() - process old filter table                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void DS_TableUnsubscribe(void)
{
    DS_PacketEntry_t *FilterPackets = NULL;
    CFE_SB_MsgId_t MessageID = 0;
    int32 i = 0;

    FilterPackets = DS_AppData.FilterTblPtr->Packet;

    /*
    ** Check each entry in "old" packet filter table...
    */
    for (i = 0; i < DS_PACKETS_IN_FILTER_TABLE; i++)
    {
        MessageID = FilterPackets[i].MessageID;

        /*
        ** Do not un-subscribe to unused or DS command packets...
        */
        if ((MessageID != DS_UNUSED) &&
            (MessageID != DS_CMD_MID) &&
            (MessageID != DS_SEND_HK_MID))
        {
            CFE_SB_Unsubscribe(MessageID, DS_AppData.InputPipe);
        }
    }

    return;

} /* End of DS_TableUnsubscribe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableCreateCDS() - create DS storage area in CDS             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 DS_TableCreateCDS(void)
{
    /* Store file sequence counts and task ena/dis state in CDS */
    uint32 DataStoreBuffer[DS_DEST_FILE_CNT + 1] = {0};
    int32 Result = CFE_SUCCESS;
    int32 i = 0;

    /*
    ** Request for CDS area from cFE Executive Services...
    */
    Result = CFE_ES_RegisterCDS(&DS_AppData.DataStoreHandle,
                                 sizeof(DataStoreBuffer), DS_CDS_NAME);

    if (Result == CFE_SUCCESS)
    {
        /*
        ** New CDS area - write to Critical Data Store...
        */
        for (i = 0; i < DS_DEST_FILE_CNT; i++)
        {
            DataStoreBuffer[i] = DS_AppData.FileStatus[i].FileCount;
        }

        DataStoreBuffer[DS_DEST_FILE_CNT] = DS_AppData.AppEnableState;

        Result = CFE_ES_CopyToCDS(DS_AppData.DataStoreHandle, DataStoreBuffer);
    }
    else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /*
        ** Pre-existing CDS area - read from Critical Data Store...
        */
        Result = CFE_ES_RestoreFromCDS(DataStoreBuffer, DS_AppData.DataStoreHandle);

        if (Result == CFE_SUCCESS)
        {
            for (i = 0; i < DS_DEST_FILE_CNT; i++)
            {
                DS_AppData.FileStatus[i].FileCount = DataStoreBuffer[i];
            }

            #if (DS_CDS_ENABLE_STATE == 1)
            /* Only restore enable/disable state if configured */
            DS_AppData.AppEnableState = (uint8) DataStoreBuffer[DS_DEST_FILE_CNT];
            #endif
        }
    }

    if (Result != CFE_SUCCESS)
    {
        /*
        ** CDS is broken - prevent further errors...
        */
        DS_AppData.DataStoreHandle = 0;

        CFE_EVS_SendEvent(DS_INIT_CDS_ERR_EID, CFE_EVS_EventType_ERROR,
                         "Critical Data Store access error = 0x%08X", (unsigned int)Result);
        /*
        ** CDS errors are not fatal - DS can still run...
        */
        Result = CFE_SUCCESS;
    }

    return(Result);

} /* End of DS_TableCreateCDS() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableUpdateCDS() - update DS storage area in CDS             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void DS_TableUpdateCDS(void)
{
    /* Store file sequence counts and task ena/dis state in CDS */
    uint32 DataStoreBuffer[DS_DEST_FILE_CNT + 1] = {0};
    int32 Result = CFE_SUCCESS;
    int32 i = 0;

    /*
    ** Handle is non-zero when CDS is active...
    */
    if (DS_AppData.DataStoreHandle != 0)
    {
        /*
        ** Copy file sequence counts values to the data array...
        */
        for (i = 0; i < DS_DEST_FILE_CNT; i++)
        {
            DataStoreBuffer[i] = DS_AppData.FileStatus[i].FileCount;
        }

        /*
        ** Always save the DS enable/disable state in the CDS...
        **  (DS_CDS_ENABLE_STATE controls restoring the state)
        */
        DataStoreBuffer[DS_DEST_FILE_CNT] = DS_AppData.AppEnableState;

        /*
        ** Update DS portion of Critical Data Store...
        */
        Result = CFE_ES_CopyToCDS(DS_AppData.DataStoreHandle, DataStoreBuffer);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(DS_INIT_CDS_ERR_EID, CFE_EVS_EventType_ERROR,
                             "Critical Data Store access error = 0x%08X", (unsigned int)Result);
            /*
            ** CDS is broken - prevent further errors...
            */
            DS_AppData.DataStoreHandle = 0;
        }
    }

    return;

} /* End of DS_TableUpdateCDS() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableHashFunction() - convert messageID to hash table index  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32 DS_TableHashFunction(CFE_SB_MsgId_t MessageID)
{
    /*
    **   The purpose of a hash function is to take the input value
    ** and convert it to an index into the hash table. Assume that
    ** the range of input values is much different (larger) than
    ** the number of entries in the hash table. Then multiple input
    ** values must resolve to the same output table index. This is ok
    ** because each entry in the hash table is a linked list of all
    ** the inputs with the same hash function result.
    **
    **   This particular hash function takes advantage of knowledge
    ** regarding the format of the input values (cFE MessageID). By
    ** ignoring the bits that define version number, packet type and
    ** secondary header (high 5 bits of 16) we are left with the bits
    ** (mask = 0x7FF) that can identify 2048 unique input telemetry
    ** packets. Also, by using a fixed hash table size of 256 entries
    ** and using only the lower 8 bits of the bitmask as the result
    ** of the hash function, no single hash table entry will have more
    ** than 8 elements in its linked list.
    **
    **   To look up a MessageID in the DS packet filter table, rather
    ** than search the entire filter table, DS does the following:
    **
    **   - call the hash function
    **     (input = MessageID, output = hash table index)
    **
    **   - search hash table entry linked list for matching MessageID
    **     (each linked list contains, at most, 8 linked list elements)
    **
    **   - matching linked list element has index into filter table
    **     (can now go directly to the correct filter table entry)
    */
    return((uint32) (MessageID & DS_HASH_TABLE_MASK));

} /* End of DS_TableHashFunction() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableCreateHash() - create and populate hash table           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void DS_TableCreateHash(void)
{
    int32 FilterIndex = 0;

    /*
    ** Initialize global hash table structures...
    */
    CFE_PSP_MemSet(DS_AppData.HashLinks, 0, (sizeof(DS_HashLink_t) * DS_PACKETS_IN_FILTER_TABLE));
    CFE_PSP_MemSet(DS_AppData.HashTable, 0, (sizeof(DS_HashLink_t *) * DS_HASH_TABLE_ENTRIES));

    for (FilterIndex = 0; FilterIndex < DS_PACKETS_IN_FILTER_TABLE; FilterIndex++)
    {
        DS_TableAddMsgID(DS_AppData.FilterTblPtr->Packet[FilterIndex].MessageID, FilterIndex);
    }

    return;

} /* End of DS_TableCreateHash() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableFindMsgID() - get filter table index for MID            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 DS_TableAddMsgID(CFE_SB_MsgId_t MessageID, int32 FilterIndex)
{
    int32 HashIndex = 0;
    DS_HashLink_t *NewLink = NULL;
    DS_HashLink_t *LinkList = NULL;

    /* Get unused linked list entry (one link entry per filter table entry) */
    NewLink = &DS_AppData.HashLinks[FilterIndex];

    /* Set filter table data values for new linked list entry */
    NewLink->Index = FilterIndex;
    NewLink->MessageID = MessageID;

    /* Hash table function converts MID into hash table index */
    HashIndex = DS_TableHashFunction(NewLink->MessageID);

    if (DS_AppData.HashTable[HashIndex] == (DS_HashLink_t *) NULL)
    {
        /* Set first link in this hash table entry linked list */
        DS_AppData.HashTable[HashIndex] = NewLink;
    }
    else
    {
        /* Get start of linked list (all MID's with same hash result) */
        LinkList = DS_AppData.HashTable[HashIndex];

        /* Find last link */
        while (LinkList->Next != (DS_HashLink_t *) NULL)
        {
            LinkList = LinkList->Next;
        }

        /* Add new link */
        LinkList->Next = NewLink;
    }

    return(HashIndex);

} /* End of DS_TableAddMsgID() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS_TableFindMsgID() - get filter table index for MID            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 DS_TableFindMsgID(CFE_SB_MsgId_t MessageID)
{
    DS_PacketEntry_t *FilterPackets = NULL;
    DS_HashLink_t *HashLink = NULL;
    int32 HashTableIndex = 0;
    int32 FilterTableIndex = 0;

    /* De-reference filter table packet array */
    FilterPackets = DS_AppData.FilterTblPtr->Packet;

    /* Set search result to "not found" */
    FilterTableIndex = DS_INDEX_NONE;

    /* Hash table function converts MID into hash table index */
    HashTableIndex = DS_TableHashFunction(MessageID);

    /* Get start of linked list (all MID's with same hash result) */
    HashLink = DS_AppData.HashTable[HashTableIndex];

    /* NULL when list is empty or end of list */
    while (HashLink != (DS_HashLink_t *) NULL)
    {
        /* Compare this linked list entry for matching MessageID */
        if (FilterPackets[HashLink->Index].MessageID == MessageID)
        {
            /* Stop the search - we found it */
            FilterTableIndex = HashLink->Index;
            break;
        }

        /* Max of 8 links per design */
        HashLink = HashLink->Next;
    }

    return(FilterTableIndex);

} /* End of DS_TableFindMsgID() */


/************************/
/*  End of File Comment */
/************************/
