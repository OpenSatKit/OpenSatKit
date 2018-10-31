##############################################################################
## $Id: README.txt 1.4 2016/10/28 11:04:15EDT mdeschu Exp  $
##
## Purpose: CFS HK application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
## $Log: README.txt  $
## Revision 1.4 2016/10/28 11:04:15EDT mdeschu 
## Updates to unit tests for 2.4.1
## Revision 1.3 2016/10/18 17:28:57EDT czogby 
## Fix test HK_AppInit_Test_Nominal
## Revision 1.2 2016/10/07 14:33:15EDT czogby 
## Code Walkthrough Updates
## Revision 1.1 2016/06/24 14:43:52EDT czogby 
## Initial revision
## Member added to project /CFS-APPs-PROJECT/hk/fsw/unit_test/project.pj
##############################################################################


-------------------------
HK Unit Test Instructions
-------------------------
This unit test was run in a virtual machine running CentOS and uses the ut-assert stubs and default hooks for the 
cFE, OSAL and PSP. The ut-assert framework, stubs, and hooks are located in the directory cfe/tools/ut-assert and 
are configuration managed in MKS in the FSW tools repository on server tlserver3.

To run the unit test enter the following commands at the command line prompt in
unit test directory.

make clean
make 
make run
make gcov

HK 2.4.1.0 Unit Test Results:

Tests Executed:    51
Assert Pass Count: 145
Assert Fail Count: 0

gcov: '../src/hk_app.c' 100.00%  153
gcov: '../src/hk_utils.c' 100.00%  163

==========================================================================
hk_app.c - 100.00% coverage

==========================================================================
hk_utils.c - 100.00% coverage

==========================================================================
