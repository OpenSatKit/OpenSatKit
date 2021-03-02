##############################################################################
## $Id: README.txt 1.2 2017/07/05 13:00:24EDT mdeschu Exp  $
##
## Purpose: CFS MD application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
## $Log: README.txt  $
## Revision 1.2 2017/07/05 13:00:24EDT mdeschu 
## Reran default and alt config unit tests, updated gcov results, logs and README for MD 2.3.1
## Revision 1.1 2017/01/27 18:56:32EST czogby 
## Initial revision
## Member added to project /CFS-APPs-PROJECT/md/fsw/unit_test/project.pj
##############################################################################


-------------------------
MD Unit Test Instructions
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

MD 2.3.1.0 Unit Test Results (default configuration of MD_SIGNATURE_OPTION == 1, MD_ENFORCE_DWORD_ALIGN == 1):

Tests Executed:    105
Assert Pass Count: 430
Assert Fail Count: 0

gcov: '../src/md_dwell_tbl.c' 100.00%  123
gcov: '../src/md_cmds.c' 100.00%  127
gcov: '../src/md_app.c' 98.73%  236
gcov: '../src/md_dwell_pkt.c' 100.00%  60
gcov: '../src/md_utils.c' 100.00%  44

==========================================================================
md_dwell_tbl.c - 100.00% coverage

==========================================================================
md_cmds.c - 100.00% coverage

==========================================================================
md_app.c - 98.73% coverage

Cannot test default case in ExecRequest, because that would require the pre-defined constant structure MD_CmdHandlerTbl to have an entry
with a command code other than the handled cases.  It does not, except for the last entry, which does not work because it's of type MD_TERM_MSGTYPE

Cannot test 2nd case of "FoundMatch = TRUE" in MD_SearchCmdHndlrTbl, because that would require an entry in MD_CmdHandlerTbl other than 
MD_CMD_MSGTYPE and MD_TERM_MSGTYPE, which it does not have.

==========================================================================
md_dwell_pkt.c - 100.00% coverage

==========================================================================
md_utils.c - 100.00% coverage

==========================================================================

MD 2.3.1.0 Unit Test Results (alternate configuration of MD_SIGNATURE_OPTION == 0, MD_ENFORCE_DWORD_ALIGN == 0):

Tests Executed:    99
Assert Pass Count: 392
Assert Fail Count: 0

gcov: '../src/md_dwell_tbl.c' 100.00%  108
gcov: '../src/md_cmds.c' 100.00%  109
gcov: '../src/md_app.c' 98.71%  232
gcov: '../src/md_dwell_pkt.c' 100.00%  58
gcov: '../src/md_utils.c' 100.00%  44

==========================================================================
md_dwell_tbl.c - 100.00% coverage

==========================================================================
md_cmds.c - 100.00% coverage

==========================================================================
md_app.c - 98.71% coverage

Cannot test default case in ExecRequest, because that would require the pre-defined constant structure MD_CmdHandlerTbl to have an entry
with a command code other than the handled cases.  It does not, except for the last entry, which does not work because it's of type MD_TERM_MSGTYPE

Cannot test 2nd case of "FoundMatch = TRUE" in MD_SearchCmdHndlrTbl, because that would require an entry in MD_CmdHandlerTbl other than 
MD_CMD_MSGTYPE and MD_TERM_MSGTYPE, which it does not have.

==========================================================================
md_dwell_pkt.c - 100.00% coverage

==========================================================================
md_utils.c - 100.00% coverage

==========================================================================

