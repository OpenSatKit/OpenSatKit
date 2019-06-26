
/*
 * Filename: expat_testrunner.c
 *
 * Purpose: Run the unit tests expat parser.
 *
 */

/*
 * Includes
 */

#include "uttest.h"

/*
 *  Text parsers text cases
 */

void EXPAT_AddTestCase(void);

/*
 * Function Definitions
 */

int main(void)
{   

   EXPAT_AddTestCase();
 
    return(UtTest_Run());
}
