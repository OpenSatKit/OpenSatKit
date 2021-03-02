 /*************************************************************************
 ** File: md_testrunner.c 
 **
 ** NASA Docket No. GSC-18,450-1, identified as “Core Flight Software System (CFS)
 ** Memory Dwell Application Version 2.3.3” 
 **
 ** Copyright © 2019 United States Government as represented by the Administrator of
 ** the National Aeronautics and Space Administration. All Rights Reserved. 
 **
 ** Licensed under the Apache License, Version 2.0 (the "License"); 
 ** you may not use this file except in compliance with the License. 
 ** You may obtain a copy of the License at 
 ** http://www.apache.org/licenses/LICENSE-2.0 
 **
 ** Unless required by applicable law or agreed to in writing, software 
 ** distributed under the License is distributed on an "AS IS" BASIS, 
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 ** See the License for the specific language governing permissions and 
 ** limitations under the License. 
 **
 ** Purpose: 
 **   This file contains the unit test runner for the MD application.
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
#include "md_app_test.h"
#include "md_cmds_test.h"
#include "md_dwell_pkt_test.h"
#include "md_dwell_tbl_test.h"
#include "md_utils_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    MD_App_Test_AddTestCases();
    MD_Cmds_Test_AddTestCases();
    MD_Dwell_Pkt_Test_AddTestCases();
    MD_Dwell_Tbl_Test_AddTestCases();
    MD_Utils_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */


/************************/
/*  End of File Comment */
/************************/
