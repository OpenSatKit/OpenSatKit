 /*************************************************************************
 ** File:
 **   $Id: lc_test_utils.c 1.3 2017/01/22 17:24:41EST sstrege Exp  $
 **
 **  Copyright (c) 2007-2014 United States Government as represented by the 
 **  Administrator of the National Aeronautics and Space Administration. 
 **  All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   This file contains unit test utilities for the LC application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 ** Notes:
 **
 *************************************************************************/

#include "lc_test_utils.h"

extern LC_AppData_t     LC_AppData;
extern LC_OperData_t    LC_OperData;

/* Global table variables for table pointers contained in LC_OperData */
LC_WDTEntry_t       WDTable[LC_MAX_WATCHPOINTS];
LC_ADTEntry_t       ADTable[LC_MAX_ACTIONPOINTS];
LC_WRTEntry_t       WRTable[LC_MAX_WATCHPOINTS];
LC_ARTEntry_t       ARTable[LC_MAX_ACTIONPOINTS];

LC_MessageList_t    HashTable[LC_HASH_TABLE_ENTRIES];

LC_WatchPtList_t    WatchPtList[LC_HASH_TABLE_ENTRIES][LC_MAX_WATCHPOINTS];

/*
 * Function Definitions
 */

void LC_Test_Setup(void)
{
    uint16 i;

    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&LC_AppData, 0, sizeof(LC_AppData_t));
    CFE_PSP_MemSet(&LC_OperData, 0, sizeof(LC_OperData_t));

    LC_OperData.WDTPtr = WDTable;
    LC_OperData.ADTPtr = ADTable;
    LC_OperData.WRTPtr = WRTable;
    LC_OperData.ARTPtr = ARTable;

    memset(LC_OperData.WDTPtr, 0, sizeof(LC_WDTEntry_t)*LC_MAX_WATCHPOINTS);
    memset(LC_OperData.ADTPtr, 0, sizeof(LC_ADTEntry_t)*LC_MAX_ACTIONPOINTS);
    memset(LC_OperData.WRTPtr, 0, sizeof(LC_WRTEntry_t)*LC_MAX_WATCHPOINTS);
    memset(LC_OperData.ARTPtr, 0, sizeof(LC_ARTEntry_t)*LC_MAX_ACTIONPOINTS);

    for (i = 0; i < LC_HASH_TABLE_ENTRIES; i++)
    {
        LC_OperData.HashTable[i]  = &HashTable[i];

        memset(LC_OperData.HashTable[i], 0, sizeof(LC_MessageList_t));

        LC_OperData.HashTable[i]->WatchPtList = WatchPtList[i];

        memset(LC_OperData.HashTable[i]->WatchPtList, 0, sizeof(LC_WatchPtList_t)*LC_MAX_WATCHPOINTS);

        LC_OperData.MessageLinks[i].WatchPtList = WatchPtList[i];
    }
    
    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
} /* end LC_Test_Setup */

void LC_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end LC_Test_TearDown */


/************************/
/*  End of File Comment */
/************************/
