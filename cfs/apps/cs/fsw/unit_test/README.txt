
                     -----------------------------------
                     | README.TXT for CS App Unit Test |
                     -----------------------------------


--------
Contents
--------

1. Environment
2. Make Instructions
  2.1 Compiler options and build sequence
  2.2 Directory/Folder Layout

3. Overall Coverage Statistics
  3.1 Comments on Functions with less than 100% Coverage



-----------
Environment
-----------

The round of unit tests run under CFS_FSW_DCR 11542 were executed in a 64-bit CentOS (Linux) Virtual 
Machine running under Parallels on an iMac.  Although it is a 64-bit OS, the code was compiled under 
a 32-bit configuration.

The makefile psp/fsw/pc-linux/make/compiler-opts.mak includes the following modified line:

     ARCH_OPTS=-m32 -fPIC
     
This allows the UTF files to be built as 32-bit code under the 64-bit OS.

The unit test reads instructions from the input file cs_utf_cmds.in (located in the unit_test directory), and 
generates an output file named cs_utf_test.out.


-----------------
Make instructions
-----------------


-----------------------------------
Compiler Options and Build Sequence
-----------------------------------

For the 64-bit CentOS environment, I had to add "-m32 -fPIC" to the DEFAULT-COPT definition, and "-m32" to the
LOPT definition in the unit test makefile.

The unit test makefile (named "makefile") contains options to build the unit test plus default tables, as well
as all of the test tables.

When running for the first time (or to make an entirely clean unit test build), from the unit test directory,
run:


  make cleantest   (this removes all .o, .tbl, .bin, and gcov files to all files to be rebuilt)

  make fulltest    (this builds not just the unit test and default tables, but all test tables as well. it
                  also runs the elf2cfetbl tool to generate the .tbl files and moves them to their correct
                  subdirectories.)

  ./utf_test_cs.bin  (this runs the unit test)

  make gcov        (this generates the code coverage reports)



Once the test tables have been built for the first time, you can just run the sequence:

  make clean
  make
  ./utf_test_cs.bin
  make gcov

to generate fresh coverage reports and output files.


The makefile will always rebuild the default the tables and move them to the /cf/apps/DefaultTables directory.

I brute-forced the table processing in the makefile -- there is probably a cleaner way to do it, but this worked.


-----------------------
Directory/Folder layout
-----------------------

The makefile uses relative paths (from the unit_test directory) to find the appropriate header and source files.  
The directory layout used is:


<top-level path>
 |
 |--cfs-mission <--from the "cfs-mission" MKS project
 |   |
 |   |--apps <-- from the "CFS_REPOSITORY" MKS project
 |   |   |
 |   |   |--cs
 |   |   |   |
 |   |   |   |--docs
 |   |   |   |
 |   |   |   |--fsw
 |   |   |   |   |
 |   |   |   |   |--for_build (not used in CS unit test)
 |   |   |   |   |
 |   |   |   |   |--mission_inc
 |   |   |   |   |
 |   |   |   |   |--platform_inc
 |   |   |   |   |
 |   |   |   |   |--public_inc
 |   |   |   |   |
 |   |   |   |   |--src
 |   |   |   |   |
 |   |   |   |   |--tables
 |   |   |   |   |
 |   |   |   |   |--unit_test <------- CS unit test is compiled and run from this directory
 |   |   |   |       |
 |   |   |   |       |--cf
 |   |   |   |           |
 |   |   |   |           |--apps
 |   |   |   |               |
 |   |   |   |               |--InvldNameNonFullTable
 |   |   |   |               |
 |   |   |   |               |--CorrFullTable
 |   |   |   |               |
 |   |   |   |               |--InvldStNonFullTable
 |   |   |   |               |
 |   |   |   |               |--CorrQQTable
 |   |   |   |               |
 |   |   |   |               |--CorrAllTables (no longer used for CS unit test)
 |   |   |   |               |
 |   |   |   |               |--DupEntryNonFullTable
 |   |   |   |               |
 |   |   |   |               |--DefaultTable <--compiled from the source in cs/fsw/tables
 |   |   |   |               |
 |   |   |   |               |--InvldEmptyNonFullTable
 |   |   |   |               |
 |   |   |   |               |--CorrNonFullTable
 |   |   |   |
 |   |   |   |--test_and_ground (not used in CS unit test)
 |   |   |   
 |   |   |--inc (not used in CS unit test)   
 |   |
 |   |--build (not used in CS unit test)
 |   |
 |   |--cfe <--from the "MKS-CFE-PROJECT" MKS project
 |   |   |
 |   |   |--tools
 |   |   |   |
 |   |   |   |--elf2cfetbl
 |   |   |   |
 |   |   |   |--utf
 |   |   |       |
 |   |   |       |--src
 |   |   |       |
 |   |   |       |--inc
 |   |   |   
 |   |   |
 |   |   |--fsw
 |   |       |
 |   |       |--mission_inc
 |   |       |
 |   |       |--platform_inc
 |   |       |   |
 |   |       |   |--cpu1
 |   |       |    
 |   |       |--cfe-core
 |   |           |
 |   |           |--src
 |   |           |   |
 |   |           |   |--sb
 |   |           |   |
 |   |           |   |--time
 |   |           |   |
 |   |           |   |--es
 |   |           |   |
 |   |           |   |--evs
 |   |           |   |
 |   |           |   |--fs
 |   |           |   |
 |   |           |   |--tbl
 |   |           |   |
 |   |           |   |--inc
 |   |           |   
 |   |           |--os
 |   |               |
 |   |               |--inc
 |   |
 |   |--docs (not used in CS unit test)
 |   |   |
 |   |   |
 |   |
 |   |
 |   |--osal <--from the "MKS-OSAL-REPOSITORY" MKS project
 |   |   |
 |   |   |--build
 |   |   |   |
 |   |   |   |--inc
 |   |   |
 |   |   |--src
 |   |       |
 |   |       |--os
 |   |       |   |
 |   |       |   |--inc
 |   |       |   |
 |   |       |   |--posix
 |   |       |
 |   |       |--bsp
 |   |           |
 |   |           |--pc-linux
 |   |               |
 |   |               |--src
 |   |
 |   |
 |   |--psp <--from the "CFE-PSP-REPOSITORY" MKS project
 |       |
 |       |--fsw
 |           |
 |           |--inc
 |           |
 |           |--pc-linux
 |               |
 |               |--src
 |               |
 |               |--inc
 |
      


---------------------------
Overall Coverage Statistics
---------------------------

                                                     Lines executed       Branches executed
                Filename                          Percent       Total   Percent         Total
cs_eepromtbl.gcda:stamp mismatch with graph file
cs_memorytbl.gcda:stamp mismatch with graph file
cs_tablestbl.gcda:stamp mismatch with graph file
cs_apptbl.gcda:stamp mismatch with graph file
gcov:   '../src/cs_app.c'                         100.00%       299     * 88.06%        134
gcov:   '../src/cs_app_cmds.c'                    100.00%       80       100.00%        30
gcov:   '../src/cs_cmds.c'                        100.00%       134      100.00%        48
gcov:   '../src/cs_compute.c'                    * 99.12%       339      100.00%        116
gcov:   '../src/cs_eeprom_cmds.c'                 100.00%       130      100.00%        58
gcov:   '../src/cs_memory_cmds.c'                 100.00%       132      100.00%        58
gcov:   '../src/cs_table_cmds.c'                  100.00%       80       100.00%        30
gcov:   '../src/cs_table_processing.c'            100.00%       417      100.00%        232
gcov:   '../src/cs_utils.c'                       100.00%       291      100.00%        130
Processed 117 records
 
There are 13 filenames to process
 Removing cs_eepromtbl.c.gcov from argument list (does not exist)
 Removing cs_memorytbl.c.gcov from argument list (does not exist)
 Removing cs_tablestbl.c.gcov from argument list (does not exist)
 Removing cs_apptbl.c.gcov from argument list (does not exist)
cs_app.c.gcov:197: warning: branch  1 taken 0%
cs_app.c.gcov:244: warning: branch  2 taken 0% (fallthrough)
cs_app.c.gcov:520: warning: branch  1 taken 0%
cs_app.c.gcov:553: warning: branch  0 taken 0% (fallthrough)
cs_app.c.gcov:553: warning: branch  2 never executed
cs_app.c.gcov:553: warning: branch  3 never executed
cs_app.c.gcov:553: warning: branch  4 never executed
cs_app.c.gcov:553: warning: branch  5 never executed
cs_app.c.gcov:574: warning: branch  0 taken 0% (fallthrough)
cs_app.c.gcov:574: warning: branch  2 never executed
cs_app.c.gcov:574: warning: branch  3 never executed
cs_app.c.gcov:574: warning: branch  4 never executed
cs_app.c.gcov:574: warning: branch  5 never executed
cs_app.c.gcov:594: warning: branch  0 taken 0% (fallthrough)
cs_app.c.gcov:594: warning: branch  2 never executed
cs_app.c.gcov:594: warning: branch  3 never executed
cs_app.c.gcov:594: warning: branch  4 never executed
cs_app.c.gcov:594: warning: branch  5 never executed
cs_app.c.gcov:616: warning: branch  0 taken 0% (fallthrough)
cs_app.c.gcov:616: warning: branch  2 never executed
cs_app.c.gcov:616: warning: branch  3 never executed
cs_app.c.gcov:616: warning: branch  4 never executed
cs_app.c.gcov:616: warning: branch  5 never executed
cs_app.c.gcov:851: warning: branch  2 taken 0%
cs_app.c.gcov:923: warning: branch  2 taken 0%
cs_app.c.gcov:956: warning: branch  2 taken 0%
cs_app.c.gcov:958: warning: branch  1 taken 0%
cs_app_cmds.c.gcov:56: warning: branch  2 taken 0%
cs_app_cmds.c.gcov:80: warning: branch  2 taken 0%
cs_app_cmds.c.gcov:107: warning: branch  2 taken 0%
cs_app_cmds.c.gcov:161: warning: branch  2 taken 0%
cs_app_cmds.c.gcov:247: warning: branch  2 taken 0%
cs_app_cmds.c.gcov:307: warning: branch  2 taken 0%
cs_cmds.c.gcov:53: warning: branch  2 taken 0%
cs_cmds.c.gcov:87: warning: branch  2 taken 0%
cs_cmds.c.gcov:111: warning: branch  2 taken 0%
cs_cmds.c.gcov:136: warning: branch  2 taken 0%
cs_cmds.c.gcov:162: warning: branch  2 taken 0%
cs_cmds.c.gcov:187: warning: branch  2 taken 0%
cs_cmds.c.gcov:212: warning: branch  2 taken 0%
cs_cmds.c.gcov:243: warning: branch  2 taken 0%
cs_cmds.c.gcov:276: warning: branch  2 taken 0%
cs_cmds.c.gcov:340: warning: branch  2 taken 0%
cs_cmds.c.gcov:404: warning: branch  2 taken 0%
cs_cmds.c.gcov:488: warning: branch  2 taken 0%
cs_cmds.c.gcov:491: warning: branch  3 taken 0%
cs_compute.c.gcov:278: warning: branch  1 taken 0%
cs_compute.c.gcov:311: warning: Line not executed
cs_compute.c.gcov:312: warning: Line not executed
cs_compute.c.gcov:313: warning: Line not executed
cs_compute.c.gcov:549: warning: branch  1 taken 0%
cs_compute.c.gcov:553: warning: branch  1 taken 0%
cs_compute.c.gcov:553: warning: branch  3 taken 0%
cs_compute.c.gcov:802: warning: branch  3 taken 0% (fallthrough)
cs_compute.c.gcov:804: warning: branch  4 taken 0%
cs_compute.c.gcov:841: warning: branch  1 taken 0%
cs_eeprom_cmds.c.gcov:57: warning: branch  2 taken 0%
cs_eeprom_cmds.c.gcov:82: warning: branch  2 taken 0%
cs_eeprom_cmds.c.gcov:112: warning: branch  2 taken 0%
cs_eeprom_cmds.c.gcov:181: warning: branch  2 taken 0%
cs_eeprom_cmds.c.gcov:280: warning: branch  2 taken 0%
cs_eeprom_cmds.c.gcov:353: warning: branch  2 taken 0%
cs_eeprom_cmds.c.gcov:430: warning: branch  2 taken 0%
cs_eeprom_cmds.c.gcov:440: warning: branch  3 taken 0%
cs_eeprom_cmds.c.gcov:440: warning: branch  5 taken 0%
cs_memory_cmds.c.gcov:55: warning: branch  2 taken 0%
cs_memory_cmds.c.gcov:80: warning: branch  2 taken 0%
cs_memory_cmds.c.gcov:109: warning: branch  2 taken 0%
cs_memory_cmds.c.gcov:177: warning: branch  2 taken 0%
cs_memory_cmds.c.gcov:277: warning: branch  2 taken 0%
cs_memory_cmds.c.gcov:349: warning: branch  2 taken 0%
cs_memory_cmds.c.gcov:425: warning: branch  2 taken 0%
cs_memory_cmds.c.gcov:435: warning: branch  3 taken 0%
cs_memory_cmds.c.gcov:435: warning: branch  5 taken 0%
cs_table_cmds.c.gcov:53: warning: branch  2 taken 0%
cs_table_cmds.c.gcov:77: warning: branch  2 taken 0%
cs_table_cmds.c.gcov:104: warning: branch  2 taken 0%
cs_table_cmds.c.gcov:158: warning: branch  2 taken 0%
cs_table_cmds.c.gcov:244: warning: branch  2 taken 0%
cs_table_cmds.c.gcov:305: warning: branch  2 taken 0%
cs_table_processing.c.gcov:118: warning: branch  1 taken 0%
cs_table_processing.c.gcov:208: warning: branch  1 taken 0%
cs_table_processing.c.gcov:307: warning: branch  1 taken 0%
cs_table_processing.c.gcov:326: warning: branch  1 taken 0%
cs_table_processing.c.gcov:397: warning: branch  1 taken 0%
cs_table_processing.c.gcov:433: warning: branch  1 taken 0%
cs_table_processing.c.gcov:452: warning: branch  1 taken 0%
cs_table_processing.c.gcov:625: warning: branch  4 taken 0% (fallthrough)
cs_table_processing.c.gcov:838: warning: branch  1 taken 0%
cs_table_processing.c.gcov:844: warning: branch  1 taken 0%
cs_table_processing.c.gcov:866: warning: branch  1 taken 0%
cs_table_processing.c.gcov:890: warning: branch  1 taken 0%
cs_utils.c.gcov:226: warning: branch  4 taken 0%
cs_utils.c.gcov:259: warning: branch  4 taken 0%
cs_utils.c.gcov:291: warning: branch  4 taken 0%
cs_utils.c.gcov:323: warning: branch  4 taken 0%
cs_utils.c.gcov:690: warning: branch  1 taken 0%
cs_utils.c.gcov:715: warning: branch  1 taken 0%
cs_utils.c.gcov:789: warning: branch  1 taken 0%
Processed 9 files
!!! 99 warnings found !!!


---------------------------------------------------------------------------------------


Comments on Functions with less than 100% Coverage
-----------------------------------------------


Function  CS_ComputeTables from cs_compute.c


      830:  272:        if (NumBytesRemainingCycles <= 0)
        -:  273:        {
        -:  274:            /* We are finished CS'ing all of the parts for this Entry */
      830:  275:            *DoneWithEntry = TRUE;
        -:  276:            
      830:  277:            if (ResultsEntry -> ComputedYet == TRUE)
        -:  278:            {
        -:  279:                /* This is NOT the first time through this Entry.  
        -:  280:                 We have already computed a CS value for this Entry */
      734:  281:                if (NewChecksumValue != ResultsEntry -> ComparisonValue)
        -:  282:                {
        -:  283:                    /* If the just-computed value differ from the saved value */
       24:  284:                    Status = CS_ERROR;                    
        -:  285:                }
        -:  286:                else
        -:  287:                {
        -:  288:                    /* The checksum passes the test. */
        -:  289:                }
        -:  290:            }
        -:  291:            else
        -:  292:            {
        -:  293:                /* This is the first time through this Entry */
       96:  294:                ResultsEntry -> ComputedYet = TRUE;
       96:  295:                ResultsEntry -> ComparisonValue = NewChecksumValue;
        -:  296:            }
        -:  297:            
      830:  298:            *ComputedCSValue = NewChecksumValue;
      830:  299:            ResultsEntry -> ByteOffset = 0;
      830:  300:            ResultsEntry -> TempChecksumValue = 0;
        -:  301:        }
        -:  302:        else
        -:  303:        {
        -:  304:            /* We have  not finished this Entry.  Will try to finish during next wakeup */
    #####:  305:            ResultsEntry -> ByteOffset       += NumBytesThisCycle;
    #####:  306:            ResultsEntry -> TempChecksumValue = NewChecksumValue;
    #####:  307:            *ComputedCSValue = NewChecksumValue;      
        -:  308:        }


These three lines are executed when there is more of a table to be checksummed than is possible in one checksum 
cycle. These lines are impossible to reach currently because the max number of bytes to checksum in one cycle 
CS_MAX_NUM_BYTES_PER_CYCLE is set at 16,384 bytes, while the maximum table size is 16,384 bytes. This means that any 
valid table will get checksummed in one checksum cycle.


BRANCH COVERAGE
---------------

Note that the CFS Development Standards Document (582-2007-043) does not explicitly require 100% branch 
coverage, only 100% code coverage.  However, full branch coverage is a goal for future CS unit tests. 


Comments on Functions with branches that never executed
-------------------------------------------------------

Function CS_AppPipe from cs_app.c:

        -:  549:            /* update each table if there is no recompute happening on that table */
        -:  550:            
       21:  551:            if (!((CS_AppData.ChildTaskInUse == TRUE)  && 
branch  0 taken 0% (fallthrough)
branch  1 taken 100%
branch  2 never executed
branch  3 never executed
branch  4 never executed
branch  5 never executed
        -:  552:                  ( CS_AppData.OneShotTaskInUse == FALSE) && 
        -:  553:                  (CS_AppData.ChildTaskTable == CS_EEPROM_TABLE)))
        -:  554:            {
       21:  555:                Result = CS_HandleTableUpdate ((void*) & CS_AppData.DefEepromTblPtr,
call    0 returned 100%
        -:  556:                                               (void*) & CS_AppData.ResEepromTblPtr,
        -:  557:                                               CS_AppData.DefEepromTableHandle,
        -:  558:                                               CS_AppData.ResEepromTableHandle,
        -:  559:                                               CS_EEPROM_TABLE,
        -:  560:                                               CS_MAX_NUM_EEPROM_TABLE_ENTRIES);
        -:  561:                


The conditional statement above is interpreted by the compiler as:

[1] evaluate CS_AppData.ChildTaskInUse, if true branch to [2] (branch 0) else branch to [4] (branch 1)
[2] evaluate CS_AppData.OneShotTaskInUse, if true branch to [3] (branch 2) else branch to [4] (branch 3)
[3] evaluate CS_AppData.ChildTaskTable==CS_EEPROM_TABLE, if true branch to [5] (branch 4) else branch to [4] (branch 5)
[4] execute next statement inside block (line 555, call to CS_HandleTableUpdate)
[5] skip block and execute next statement after the block (not shown)

The gcov report shows that CS_AppData.ChildTaskInUse was evaluated and found to be False, so branch 0 was looked at but
not taken, and branch 1 was always taken.  Since branch 1 jumps to step [4] shown above, the subsequent branches 
identified in steps [2] and [3] were never taken.

A future version of this unit test will be modified to evaluate the branches not currently taken.

Other branches reported as "never executed" also occurred in the function CS_AppPipe and are shown below.  These
branches are of the same form shown above and will be corrected in a similar manner.

       21:  572:            if (!((CS_AppData.ChildTaskInUse == TRUE)  && 
branch  0 taken 0% (fallthrough)
branch  1 taken 100%
branch  2 never executed
branch  3 never executed
branch  4 never executed
branch  5 never executed
        -:  573:                  ( CS_AppData.OneShotTaskInUse == FALSE) && 
        -:  574:                  (CS_AppData.ChildTaskTable == CS_MEMORY_TABLE)))
        -:  575:            {
       21:  576:                Result = CS_HandleTableUpdate ((void*) & CS_AppData.DefMemoryTblPtr,
call    0 returned 100%
        -:  577:                                               (void*) & CS_AppData.ResMemoryTblPtr,
        -:  578:                                               CS_AppData.DefMemoryTableHandle,
        -:  579:                                               CS_AppData.ResMemoryTableHandle,
        -:  580:                                               CS_MEMORY_TABLE,
        -:  581:                                               CS_MAX_NUM_MEMORY_TABLE_ENTRIES);


See explanation above.  A future version of this unit test will be modified to evaluate the branches not currently taken.

        
       21:  592:            if (!((CS_AppData.ChildTaskInUse == TRUE)  && 
branch  0 taken 0% (fallthrough)
branch  1 taken 100%
branch  2 never executed
branch  3 never executed
branch  4 never executed
branch  5 never executed
        -:  593:                  ( CS_AppData.OneShotTaskInUse == FALSE) && 
        -:  594:                  (CS_AppData.ChildTaskTable == CS_APP_TABLE)))
        -:  595:            {
       21:  596:                Result = CS_HandleTableUpdate ((void*) & CS_AppData.DefAppTblPtr,
call    0 returned 100%
        -:  597:                                               (void*) & CS_AppData.ResAppTblPtr,
        -:  598:                                               CS_AppData.DefAppTableHandle,
        -:  599:                                               CS_AppData.ResAppTableHandle,
        -:  600:                                               CS_APP_TABLE,
        -:  601:                                               CS_MAX_NUM_APP_TABLE_ENTRIES);


See explanation above.  A future version of this unit test will be modified to evaluate the branches not currently taken.


       21:  614:            if (!((CS_AppData.ChildTaskInUse == TRUE)  && 
branch  0 taken 0% (fallthrough)
branch  1 taken 100%
branch  2 never executed
branch  3 never executed
branch  4 never executed
branch  5 never executed
        -:  615:                  ( CS_AppData.OneShotTaskInUse == FALSE) && 
        -:  616:                  (CS_AppData.ChildTaskTable == CS_TABLES_TABLE)))
        -:  617:            {
       21:  618:                Result = CS_HandleTableUpdate ((void*) & CS_AppData.DefTablesTblPtr,
call    0 returned 100%
        -:  619:                                               (void*) & CS_AppData.ResTablesTblPtr,
        -:  620:                                               CS_AppData.DefTablesTableHandle,
        -:  621:                                               CS_AppData.ResTablesTableHandle,
        -:  622:                                               CS_TABLES_TABLE,
        -:  623:                                               CS_MAX_NUM_TABLES_TABLE_ENTRIES);
        -:  624:                


See explanation above.  A future version of this unit test will be modified to evaluate the branches not currently taken.


Comments on Functions with branches taken 0%
--------------------------------------------

All branches that were taken 0% were part of decision conditional statements that executed other paths that led
to the same code (giving us 100% code coverage).  Future CS unit tests will attempt to provide full branch
coverage.

        