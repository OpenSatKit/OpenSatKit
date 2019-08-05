 /*************************************************************************
 ** File:
 **   $Id: mm_test_utils.c 1.1 2016/02/03 17:11:38EST czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test utilities for the MM application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: mm_test_utils.c  $
 **   Revision 1.1 2016/02/03 17:11:38EST czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/mm/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "mm_test_utils.h"
#include "mm_msg.h"

extern MM_AppData_t    MM_AppData;

/*
 * Function Definitions
 */

void MM_Test_Setup(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&MM_AppData, 0, sizeof(MM_AppData_t));
    
    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
} /* end MM_Test_Setup */

void MM_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end MM_Test_TearDown */


/************************/
/*  End of File Comment */
/************************/
