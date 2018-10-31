 /*************************************************************************
 ** File:
 **   $Id: hk_testrunner.c 1.1 2016/06/24 14:43:50EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the unit test runner for the HK application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hk_testrunner.c  $
 **   Revision 1.1 2016/06/24 14:43:50EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hk/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "uttest.h"
#include "hk_app_test.h"
#include "hk_utils_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    HK_App_Test_AddTestCases();
    HK_Utils_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */

/************************/
/*  End of File Comment */
/************************/
