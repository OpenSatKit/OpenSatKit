##############################################################################
## $Id: README.txt 1.5 2017/07/06 12:26:36EDT mdeschu Exp  $
##
## Purpose: CFS LC application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
##############################################################################


-------------------------
LC Unit Test Instructions
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

LC 2.1.0.0 Unit Test Results for default configuration (LC_SAVE_TO_CDS not defined):
GCOV results this configuration can be found under output_noCDS

Tests Executed:    233
Assert Pass Count: 779
Assert Fail Count: 0

gcov: '../src/lc_custom.c' 100.00%  11
gcov: '../src/lc_action.c' 100.00%  199
gcov: '../src/lc_app.c' 96.67%  240
gcov: '../src/lc_cmds.c' 100.00%  357
gcov: '../src/lc_watch.c' 99.71%  341

==========================================================================
lc_custom.c - 100.00% coverage

==========================================================================
lc_action.c - 100.00% coverage

==========================================================================
lc_app.c - 96.67% coverage

Unit test code for CDS is covered by the alternate configuration

==========================================================================
lc_cmds.c - 100.00% coverage

==========================================================================
lc_watch.c - 99.70% coverage

==========================================================================

LC 2.1.0.0 Unit Test Results for alternate configuration (LC_SAVE_TO_CDS defined):
GCOV results for this configuration can be found under output_CDS

Tests Executed:    231
Assert Pass Count: 761
Assert Fail Count: 0

gcov: '../src/lc_custom.c' 100.00%  11
gcov: '../src/lc_action.c' 100.00%  199
gcov: '../src/lc_app.c' 95.02%  241
gcov: '../src/lc_cmds.c' 100.00%  357
gcov: '../src/lc_watch.c' 99.71%  341

==========================================================================
lc_custom.c - 100.00% coverage

==========================================================================
lc_action.c - 100.00% coverage

==========================================================================
lc_app.c - 95.02% coverage

Lines not covered here are already covered in the default configuration.

==========================================================================
lc_cmds.c - 100.00% coverage

==========================================================================
lc_watch.c - 99.71% coverage

==========================================================================

