##############################################################################
## $Id: README.txt 1.5 2016/10/15 16:21:27EDT mdeschu Exp  $
##
## Purpose: CFS SC application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
## $Log: README.txt  $
## Revision 1.5 2016/10/15 16:21:27EDT mdeschu 
## Update unit test readme and log file for SC 2.5.0.0
## Revision 1.4 2016/09/09 19:27:07EDT czogby 
## Update based on code inspection log
## Revision 1.3 2016/03/07 15:23:16EST czogby 
## Fix mistakes in README.txt
## Revision 1.2 2016/03/03 21:46:47EST czogby 
## Update log file & readme for run with official release of SC 2.4.0
## Revision 1.1 2015/08/18 14:02:56EDT czogby 
## Initial revision
## Member added to project /CFS-APPs-PROJECT/sc/fsw/unit_test/project.pj
##############################################################################


-------------------------
SC Unit Test Instructions
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

SC 2.5.0.0 Unit Test Results:

Tests Executed:    192
Assert Pass Count: 680
Assert Fail Count: 0

gcov: '../src/sc_state.c' 100.00%  80
gcov: '../src/sc_atsrq.c' 100.00%  184
gcov: '../src/sc_app.c' 100.00%  213
gcov: '../src/sc_cmds.c' 100.00%  292
gcov: '../src/sc_rtsrq.c' 100.00%  179
gcov: '../src/sc_utils.c' 100.00%  36

'../src/sc_loads.c' not shown in gcov command line results - all lines covered except for 2, which are discussed below.

==========================================================================
sc_state.c - 100.00% coverage

==========================================================================
sc_atsrq.c - 100.00% coverage

==========================================================================
sc_app.c - 100.00% coverage

==========================================================================
sc_cmds.c - 100.00% coverage

==========================================================================
sc_rtsrq.c - 100.00% coverage

==========================================================================
sc_utils.c - 100.00% coverage

==========================================================================

sc_loads.c - 2 Lines of Code NOT executed (lines 191 - 192)

In procedure SC_LoadAts:

      151:  114:        while (StillProcessing)
        -:  115:    {
        -:  116:        /*
        -:  117:         ** Make sure that the pointer as well as the primary packet
        -:  118:         ** header fit in the buffer, so a G.P fault is not caused.
        -:  119:         */
      135:  120:        if (AtsEntryIndex < SC_ATS_BUFF_SIZE)
        -:  121:        {
        -:  122:            /* get the next command number from the buffer */
      134:  123:            AtsCmdNum = ((SC_AtsEntryHeader_t *)&AtsTablePtr[AtsEntryIndex]) ->CmdNumber;
        -:  124:    
      134:  125:            if (AtsCmdNum == 0)
        -:  126:            {   
        -:  127:                /* end of the load reached */
        3:  128:                Result = CFE_SUCCESS;
        3:  129:                StillProcessing = FALSE;
        -:  130:            }
        -:  131:           
        -:  132:                    /* make sure the CmdPtr can fit in a whole Ats Cmd Header at the very least */
      131:  133:            else if (AtsEntryIndex > (SC_ATS_BUFF_SIZE - (sizeof(SC_AtsEntryHeader_t)/SC_BYTES_IN_WORD)))
        -:  134:            {
        -:  135:                /* even the smallest command will not fit in the buffer */
        1:  136:                Result = SC_ERROR;
        1:  137:                StillProcessing = FALSE;
        -:  138:            }  /* else if the cmd number is valid and the command */
        -:  139:            /* has not already been loaded                     */
        -:  140:            else
      388:  141:                if (AtsCmdNum <= SC_MAX_ATS_CMDS &&
      129:  142:                    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][AtsCmdNum - 1] == SC_EMPTY)
        -:  143:                {
        -:  144:                    /* get a pointer to the ats command in the table */
      129:  145:                    AtsEntryPtr = (SC_AtsEntryHeader_t *) &AtsTablePtr[AtsEntryIndex];
      129:  146:                    AtsCmd = (CFE_SB_MsgPtr_t) AtsEntryPtr->CmdHeader;
        -:  147:                                       
        -:  148:                    /* if the length of the command is valid */
      385:  149:                    if (CFE_SB_GetTotalMsgLength(AtsCmd) >= SC_PACKET_MIN_SIZE && 
      128:  150:                        CFE_SB_GetTotalMsgLength(AtsCmd) <= SC_PACKET_MAX_SIZE)
        -:  151:                    {
        -:  152:                        /* get the length of the entry in WORDS (plus 1 to round byte len up to word len) */
      128:  153:                        AtsEntryWords = (CFE_SB_GetTotalMsgLength(AtsCmd) + 1 + SC_ATS_HEADER_SIZE) / SC_BYTES_IN_WORD; 
        -:  154:                        
        -:  155:                        /* if the command does not run off of the end of the buffer */
      128:  156:                        if (AtsEntryIndex + AtsEntryWords <= SC_ATS_BUFF_SIZE)
        -:  157:                        {
        -:  158:                            /* set the command pointer in the command index table */
        -:  159:                            /* CmdNum starts at one....                          */
        -:  160:                            
      127:  161:                            SC_AppData.AtsCmdIndexBuffer[AtsIndex][AtsCmdNum -1] = AtsEntryIndex;
        -:  162:                            
        -:  163:                            /* set the command status to loaded in the command status table */
      127:  164:                            SC_OperData.AtsCmdStatusTblAddr[AtsIndex][AtsCmdNum - 1] = SC_LOADED;
        -:  165:                            
        -:  166:                            /* increment the number of commands loaded */
      127:  167:                            SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands++;
        -:  168:                            
        -:  169:                            /* increment the ats_entry index to the next ats entry */
      127:  170:                            AtsEntryIndex = AtsEntryIndex + AtsEntryWords;
        -:  171:                        }
        -:  172:                        else
        -:  173:                        { /* the command runs off the end of the buffer */
        1:  174:                            Result = SC_ERROR;
        1:  175:                            StillProcessing = FALSE;
        -:  176:                        } /* end if */
        -:  177:                    }
        -:  178:                    else
        -:  179:                    { /* the command length was invalid */
        1:  180:                        Result = SC_ERROR;
        1:  181:                        StillProcessing = FALSE;
        -:  182:                    } /* end if */
        -:  183:                }
        -:  184:                else
        -:  185:                { /* the cmd number is invalid */                    
        1:  186:                    Result = SC_ERROR;
        1:  187:                    StillProcessing = FALSE;
        -:  188:                } /* end if */
        -:  189:        }
        -:  190:        else
        -:  191:        {
        1:  192:            if (AtsEntryIndex == SC_ATS_BUFF_SIZE)
        -:  193:            {
        -:  194:                /* we encountered a load exactly as long as the buffer */
        1:  195:                Result = CFE_SUCCESS;
        1:  196:                StillProcessing = FALSE;
        -:  197:
        -:  198:            }
        -:  199:            else
        -:  200:            { /* the pointer is over the end of the buffer */
        -:  201:                
    #####:  202:                Result = SC_ERROR;
    #####:  203:                StillProcessing = FALSE;
        -:  204:            } /* end if */
        -:  205:        }/*end else */
        -:  206:    } /* end while */

REASON: Unit testing this branch is not possible because it would require AtsEntryIndex to be greater than SC_ATS_BUFF_SIZE at the beginning of the while loop.  The only way that AtsEntryIndex can become greater than SC_ATS_BUFF_SIZE is by reaching line 159 while AtsEntryIndex + AtsEntryWords is greater than SC_ATS_BUFF_SIZE, but this condition prevents line 159 from being reached.


==========================================================================

