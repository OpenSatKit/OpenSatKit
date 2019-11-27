##############################################################################
## $Id: README.txt 1.4.1.2 2017/01/25 00:20:22EST mdeschu Exp  $
##
## Purpose: CFS FM application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
##############################################################################


-------------------------
FM Unit Test Instructions
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

FM 2.5.2.0 Unit Test Results:

Tests Executed:    195
Assert Pass Count: 897
Assert Fail Count: 0

gcov: '../src/fm_tbl.c' 100.00%  52
gcov: '../src/fm_child.c' 100.00%  514
gcov: '../src/fm_app.c' 100.00%  122
gcov: '../src/fm_cmds.c' 100.00%  304
gcov: '../src/fm_cmd_utils.c' 100.00%  172

==========================================================================
fm_tbl.c - 100.00% coverage

==========================================================================
fm_child.c - 100.00% coverage

==========================================================================
fm_app.c - 100.00% coverage

==========================================================================
fm_cmds.c - 100.00% coverage

==========================================================================
fm_cmd_utils.c - 100.00% coverage

==========================================================================
