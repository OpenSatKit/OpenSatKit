 /*************************************************************************
 ** File:
 **   $Id: fm_testrunner.c 1.2.1.2 2017/01/23 21:52:46EST sstrege Exp  $
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
 **   This file contains the unit test runner for the FM.
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

#include "uttest.h"
#include "fm_app_test.h"
#include "fm_child_test.h"
#include "fm_cmds_test.h"
#include "fm_tbl_test.h"
#include "fm_cmd_utils_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    FM_App_Test_AddTestCases();
    FM_Child_Test_AddTestCases();
    FM_Cmds_Test_AddTestCases();
    FM_Tbl_Test_AddTestCases();
    FM_Cmd_Utils_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */

/************************/
/*  End of File Comment */
/************************/
