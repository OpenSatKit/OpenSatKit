 /*************************************************************************
 ** File:
 **   $Id: hk_test_utils.c 1.1 2016/06/24 14:43:50EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test utilities for the HK application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hk_test_utils.c  $
 **   Revision 1.1 2016/06/24 14:43:50EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hk/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "hk_test_utils.h"
#include "hk_msg.h"

extern HK_AppData_t     HK_AppData;

/*
 * Function Definitions
 */

void HK_Test_Setup(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&HK_AppData, 0, sizeof(HK_AppData_t));
    
    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
} /* end HK_Test_Setup */

void HK_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end HK_Test_TearDown */

/************************/
/*  End of File Comment */
/************************/
