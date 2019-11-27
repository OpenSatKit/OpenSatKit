 /*************************************************************************
 ** File:
 **   $Id: fm_test_utils.c 1.2.1.2 2017/01/23 21:53:20EST sstrege Exp  $
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
 ** Purpose: 
 **   This file contains unit test utilities for the FM application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 *************************************************************************/

/*
 * Includes
 */

#include "fm_test_utils.h"
#include "fm_msg.h"

extern FM_GlobalData_t    FM_GlobalData;

/*
 * Function Definitions
 */

void FM_Test_Setup(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&FM_GlobalData, 0, sizeof(FM_GlobalData_t));
    
    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
} /* end FM_Test_Setup */

void FM_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end FM_Test_TearDown */

/************************/
/*  End of File Comment */
/************************/
