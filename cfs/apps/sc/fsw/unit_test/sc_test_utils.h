 /*************************************************************************
 ** File:
 **   $Id: sc_test_utils.h 1.2 2016/02/04 16:03:12EST czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the function prototypes and global variables for the unit test utilities for the SC application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_test_utils.h  $
 **   Revision 1.2 2016/02/04 16:03:12EST czogby 
 **   Add End-of-file and end-of-function comments
 **   Revision 1.1 2015/08/18 14:03:02EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/sc/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_tbl_hooks.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include "sc_app.h"
#include "sc_msgids.h"
#include "sc_rts.h"
#include "sc_version.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_evs_stubs.h"
#include <time.h>

/*
 * Function Definitions
 */

void SC_Test_Setup(void);
void SC_Test_TearDown(void);

/************************/
/*  End of File Comment */
/************************/
