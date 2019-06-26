 /*************************************************************************
 ** File:
 **   $Id: mm_testrunner.c 1.1 2016/02/03 17:11:38EST czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the unit test runner for the MM application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: mm_testrunner.c  $
 **   Revision 1.1 2016/02/03 17:11:38EST czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/mm/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "uttest.h"
#include "mm_app_test.h"
#include "mm_load_test.h"
#include "mm_mem8_test.h"
#include "mm_mem16_test.h"
#include "mm_mem32_test.h"
#include "mm_dump_test.h"
#include "mm_utils_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    MM_App_Test_AddTestCases();
    MM_Load_Test_AddTestCases();
    MM_Mem8_Test_AddTestCases();
    MM_Mem16_Test_AddTestCases();
    MM_Mem32_Test_AddTestCases();
    MM_Dump_Test_AddTestCases();
    MM_Utils_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */


/************************/
/*  End of File Comment */
/************************/
