 /*************************************************************************
 ** File:
 **   $Id: sc_test_utils.c 1.2 2016/02/04 16:03:12EST czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test utilities for the SC application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_test_utils.c  $
 **   Revision 1.2 2016/02/04 16:03:12EST czogby 
 **   Add End-of-file and end-of-function comments
 **   Revision 1.1 2015/08/18 14:03:02EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/sc/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "sc_test_utils.h"

/*
 * Function Definitions
 */

void SC_Test_Setup(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&SC_OperData, 0, sizeof(SC_OperData_t));
    CFE_PSP_MemSet(&SC_AppData,  0, sizeof(SC_AppData_t));
    
    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();

    SC_AppData.ContinueAtsOnFailureFlag = SC_CONT_ON_FAILURE_START;
    SC_AppData.NextProcNumber           = SC_NONE;
    SC_AppData.NextCmdTime[SC_ATP]      = SC_MAX_TIME;
    SC_AppData.NextCmdTime[SC_RTP]      = SC_MAX_TIME;
    SC_AppData.AutoStartRTS             = RTS_ID_AUTO_POWER_ON;

    CFE_SB_InitMsg(&SC_OperData.HkPacket, SC_HK_TLM_MID, sizeof(SC_HkTlm_t), TRUE);
} /* end SC_Test_Setup */

void SC_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end SC_Test_TearDown */
/************************/
/*  End of File Comment */
/************************/
