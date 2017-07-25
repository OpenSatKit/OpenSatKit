
/*
 * Filename: to-lab_testrunner.c
 *
 * Purpose: Run the unit tests for the template application.
 *
 */

/*
 * Includes
 */

#include "uttest.h"

/*
 *  Text parsers text cases
 */

void CMDMGR_AddTestCase(void);
void TBLMGR_AddTestCase(void);
void EXOBJ_AddTestCase(void);

/*
 * Function Definitions
 */

int main(void)
{   

   CMDMGR_AddTestCase();
   TBLMGR_AddTestCase();
   EXOBJ_AddTestCase();

   return(UtTest_Run());

} /* End main() */
