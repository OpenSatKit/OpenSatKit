/******************************************************************************
 ** \file sbn_remap.c
 **
 **      Copyright (c) 2004-2006, United States government as represented by the
 **      administrator of the National Aeronautics Space Administration.
 **      All rights reserved. This software(cFE) was created at NASA's Goddard
 **      Space Flight Center pursuant to government contracts.
 **
 **      This software may be used only pursuant to a United States government
 **      sponsored project and the United States government may not be charged
 **      for use thereof.
 **
 ** Purpose:
 **      This file contains source code for the Software Bus Network
 **      Application.
 **
 ** Authors: C. Knight/ARC Code TI
 ******************************************************************************/

#include "sbn_app.h"
#include "sbn_remap.h"
#include "sbn_tbl.h"
#include "sbn_constants.h"
#include "sbn_main_events.h"

#include <string.h> /* memcpy */
#include <stdlib.h> /* qsort */

static int RemapTblCompar(const void *a, const void *b)
{
    SBN_RemapTblEntry_t *aEntry = (SBN_RemapTblEntry_t *)a;
    SBN_RemapTblEntry_t *bEntry = (SBN_RemapTblEntry_t *)b;

    if(aEntry->ProcessorID != bEntry->ProcessorID)
    {   
        return aEntry->ProcessorID - bEntry->ProcessorID;
    }
    return aEntry->FromMID - bEntry->FromMID;
}

/* finds the entry or the one that would immediately follow it */
static int BinarySearch(void *Entries, void *SearchEntry,
    size_t EntryCnt, size_t EntrySz,
    int (*EntryCompare)(const void *, const void *))
{
    int start, end, midpoint, found;

    for(start = 0, end = EntryCnt - 1, found = 0;
        found == 0 && start <= end;
        )
    {
        midpoint = (end + start) / 2;
        int c = EntryCompare(SearchEntry, Entries + EntrySz * midpoint);
        if (c == 0)
        {
            return midpoint;
        }
        else if (c > 0)
        {
            start = midpoint + 1;
        }
        else
        {   
           end = midpoint - 1;
        }/* end if */
    }/* end while */

    if(found == 0)
    {
        return EntryCnt;
    }

    return midpoint;
}/* end BinarySearch */

static int RemapTblSearch(uint32 ProcessorID, CFE_SB_MsgId_t MID)
{
    SBN_RemapTblEntry_t Entry = {ProcessorID, MID, 0x0000};
    return BinarySearch(SBN.RemapTbl->Entries, &Entry,
        SBN.RemapTbl->EntryCnt,
        sizeof(SBN_RemapTblEntry_t),
        RemapTblCompar);
}/* end RemapTblSearch */

void SBN_RemapTblSort(SBN_RemapTbl_t *Tbl)
{
    /* sort the entries on ProcessorID and from MID */
    /* note: qsort is recursive, so it will use some stack space
     * (O[N log N] * <some small amount of stack>). If this is a concern,
     * consider using a non-recursive (insertion, bubble, etc) sort algorithm.
     */

    qsort(Tbl->Entries, Tbl->EntryCnt, sizeof(SBN_RemapTblEntry_t),
        RemapTblCompar);
}

/* Remap table from fields are sorted and unique, use a binary search. */
CFE_SB_MsgId_t SBN_RemapMsgID(uint32 ProcessorID, CFE_SB_MsgId_t FromMID)
{
    CFE_SB_MsgId_t ToMID = 0x0000;

    if(OS_MutSemTake(SBN.RemapMutex) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR, "unable to take mutex");
        return SBN_ERROR;
    }/* end if */

    int i = RemapTblSearch(ProcessorID, FromMID);

    if(i < SBN.RemapTbl->EntryCnt
        && SBN.RemapTbl->Entries[i].ProcessorID == ProcessorID
        && SBN.RemapTbl->Entries[i].FromMID == FromMID)
    {
        ToMID =  SBN.RemapTbl->Entries[i].ToMID;
    }
    else
    {
        if(SBN.RemapTbl->RemapDefaultFlag == SBN_REMAP_DEFAULT_SEND)
        {
            ToMID = FromMID;
        }/* end if */
    }/* end if */

    if(OS_MutSemGive(SBN.RemapMutex) != OS_SUCCESS)
    {   
        CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR, "unable to give mutex");
        return SBN_ERROR;
    }/* end if */

    return ToMID;
}/* end SBN_RemapMsgID */

uint32 SBN_RemapAdd(uint32 ProcessorID,
    CFE_SB_MsgId_t FromMID, CFE_SB_MsgId_t ToMID)
{
    uint32 Status = SBN_SUCCESS;

    if(OS_MutSemTake(SBN.RemapMutex) != OS_SUCCESS)
    {   
        CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR, "unable to take mutex");
        return SBN_ERROR;
    }/* end if */

    /* see if there's already an entry */
    int i = RemapTblSearch(ProcessorID, FromMID);

    SBN_RemapTblEntry_t *Entry = &SBN.RemapTbl->Entries[i];
    if(i == SBN.RemapTbl->EntryCnt
        || Entry->ProcessorID != ProcessorID
        || Entry->FromMID != FromMID)
    {
        /* adding a new entry */
        if(SBN.RemapTbl->EntryCnt >= SBN_REMAP_TABLE_SIZE)
        {
            CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_ERROR,
                "remap table full");
            Entry = NULL;
            Status = SBN_ERROR;
        }
        else
        {
            /* create space for the new entry */
            int j;

            for(j = SBN.RemapTbl->EntryCnt; j > i; j--)
            {
                memcpy(&SBN.RemapTbl->Entries[j],
                    &SBN.RemapTbl->Entries[j - 1],
                    sizeof(SBN_RemapTblEntry_t));
            }/* end for */
        }/* end if */
    }

    if(Entry)
    {
        Entry->ProcessorID = ProcessorID;
        Entry->FromMID = FromMID;
        Entry->ToMID = ToMID;

        SBN.RemapTbl->EntryCnt++;
    }/* end if */

    if(OS_MutSemGive(SBN.RemapMutex) != OS_SUCCESS)
    {   
        CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR, "unable to give mutex");
    }/* end if */

    return Status;
}/* end SBN_RemapAdd */

uint32 SBN_RemapDel(uint32 ProcessorID, CFE_SB_MsgId_t FromMID)
{
    uint32 Status = SBN_SUCCESS;

    if(OS_MutSemTake(SBN.RemapMutex) != OS_SUCCESS)
    {   
        CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR, "unable to take mutex");
        return SBN_ERROR;
    }/* end if */

    /* see if there's already an entry */
    int i = RemapTblSearch(ProcessorID, FromMID);

    if(i < SBN.RemapTbl->EntryCnt
        && SBN.RemapTbl->Entries[i].ProcessorID == ProcessorID
        && SBN.RemapTbl->Entries[i].FromMID == FromMID)
    {
        for(;i < SBN.RemapTbl->EntryCnt - 1; i++)
        {
            memcpy(&SBN.RemapTbl->Entries[i],
                &SBN.RemapTbl->Entries[i + 1],
                    sizeof(SBN_RemapTblEntry_t));
        }/* end for */

        SBN.RemapTbl->EntryCnt--;
    }/* end if */

    if(OS_MutSemGive(SBN.RemapMutex) != OS_SUCCESS)
    {   
        CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR, "unable to give mutex");
    }/* end if */

    return Status;
}/* end SBN_RemapDel */
