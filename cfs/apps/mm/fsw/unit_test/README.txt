##############################################################################
## $Id: README.txt 1.4 2016/10/31 22:11:03EDT mdeschu Exp  $
##
## Purpose: CFS MM application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
## $Log: README.txt  $
## Revision 1.4 2016/10/31 22:11:03EDT mdeschu 
## Update gcov results, README and log file for MM unit tests for 2.4.1
## Revision 1.3 2016/10/24 18:35:24EDT czogby 
## Code Walkthough Updates
## Revision 1.2 2016/03/04 20:07:42EST czogby 
## Update log file for run with official release of MM 2.4.0
## Revision 1.1 2016/02/03 17:11:38EST czogby 
## Initial revision
## Member added to project /CFS-APPs-PROJECT/mm/fsw/unit_test/project.pj
##############################################################################


-------------------------
MM Unit Test Instructions
-------------------------
This unit test was run in a virtual machine running CentOS and uses the ut-assert stubs and default hooks for the 
cFE, OSAL and PSP. The ut-assert framework, stubs, and hooks are located in the directory cfe/tools/ut-assert and 
are configuration managed in MKS in the FSW tools repository on server tlserver3.

Note: Not testing any variations in preprocessor macro definitions - default values 
in mm_platform_config.c cause all #if statements to evalute to TRUE

To run the unit test enter the following commands at the command line prompt in
unit test directory.

make clean
make 
make run
make gcov

MM 2.4.1.0 Unit Test Results:

Tests Executed:    249
Assert Pass Count: 1035
Assert Fail Count: 0

gcov: '../src/mm_mem32.c' 100.00%  109
gcov: '../src/mm_mem8.c' 100.00%  108
gcov: '../src/mm_dump.c' 100.00%  343
gcov: '../src/mm_app.c' 100.00%  192
gcov: '../src/mm_load.c' 100.00%  432
gcov: '../src/mm_mem16.c' 100.00%  108
gcov: '../src/mm_utils.c' 100.00%  93

==========================================================================
mm_mem32.c - 100.00% coverage

==========================================================================
mm_mem8.c - 100.00% coverage

==========================================================================
mm_dump.c - 100.00% coverage

==========================================================================
mm_app.c - 100.00% coverage

==========================================================================
mm_load.c - 100.00% coverage

==========================================================================
mm_mem16.c - 100.00% coverage

==========================================================================
mm_utils.c - 100.00% coverage

==========================================================================
