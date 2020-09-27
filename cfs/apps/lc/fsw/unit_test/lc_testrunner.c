 /*************************************************************************
 ** File:
 **   $Id: lc_testrunner.c 1.3 2017/01/22 17:24:51EST sstrege Exp  $
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
 **   This file contains the unit test runner for the LC application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 ** Notes:
 **
 *************************************************************************/

/*
 * Includes
 */

#include "uttest.h"
#include "lc_app_test.h"
#include "lc_action_test.h"
#include "lc_cmds_test.h"
#include "lc_custom_test.h"
#include "lc_watch_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    LC_App_Test_AddTestCases();
    LC_Action_Test_AddTestCases();
    LC_Cmds_Test_AddTestCases();
    LC_Custom_Test_AddTestCases();
    LC_Watch_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */


/************************/
/*  End of File Comment */
/************************/
