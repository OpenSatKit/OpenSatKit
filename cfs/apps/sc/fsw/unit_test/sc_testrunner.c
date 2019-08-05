 /*************************************************************************
 ** File:
 **   $Id: sc_testrunner.c 1.2 2016/02/04 16:03:11EST czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the unit test runner for the SC.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_testrunner.c  $
 **   Revision 1.2 2016/02/04 16:03:11EST czogby 
 **   Add End-of-file and end-of-function comments
 **   Revision 1.1 2015/08/18 14:03:02EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/sc/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "uttest.h"
#include "sc_app_test.h"
#include "sc_cmds_test.h"
#include "sc_state_test.h"
#include "sc_atsrq_test.h"
#include "sc_rtsrq_test.h"
#include "sc_loads_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    SC_App_Test_AddTestCases();
    SC_Cmds_Test_AddTestCases();
    SC_State_Test_AddTestCases();
    SC_Atsrq_Test_AddTestCases();
    SC_Rtsrq_Test_AddTestCases();
    SC_Loads_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */

/************************/
/*  End of File Comment */
/************************/
