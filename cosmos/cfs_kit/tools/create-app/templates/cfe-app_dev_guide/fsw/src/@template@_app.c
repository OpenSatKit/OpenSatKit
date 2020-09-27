/*******************************************************************************
** File: @template@_app.c
**
** Purpose:
**   This file contains the source code for the @Template@ App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "@template@_app.h"
#include "@template@_app_perfids.h"
#include "@template@_app_msgids.h"


/*
** System Header files
*/
#include <string.h>

/*
** @TEMPLATE@ global data
*/
@TEMPLATE@_AppData_t @TEMPLATE@_AppData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @TEMPLATE@_AppMain() -- Application entry point and main process loop   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void @TEMPLATE@_AppMain(void)
{
    int32 Status;
    
    /*
    ** Register the Application with Executive Services
    */
    CFE_ES_RegisterApp();

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(@TEMPLATE@_APPMAIN_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Intialization fails, set the RunStatus to CFE_ES_APP_ERROR
    ** and the App will not enter the RunLoop.
    */
    Status = @TEMPLATE@_AppInit();
    if ( Status != CFE_SUCCESS )
    {
        @TEMPLATE@_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** Application Main Loop. Call CFE_ES_RunLoop to check for changes
    ** in the Application's status. If there is a request to kill this
    ** App, it will be passed in through the RunLoop call.
    */
    while ( CFE_ES_RunLoop(&@TEMPLATE@_AppData.RunStatus) == TRUE )
    {
    
        /*
        ** Performance Log Exit Stamp.
        */
        CFE_ES_PerfLogExit(@TEMPLATE@_APPMAIN_PERF_ID);
   
        /*
        ** Pend on the arrival of the next Software Bus message.
        */
        Status = CFE_SB_RcvMsg(&@TEMPLATE@_AppData.MsgPtr, @TEMPLATE@_AppData.CmdPipe, CFE_SB_PEND_FOREVER);
        
        /*
        ** Performance Log Entry Stamp.
        */      
        CFE_ES_PerfLogEntry(@TEMPLATE@_APPMAIN_PERF_ID);

        /*
        ** Check the return status from the Software Bus.
        */        
        if (Status == CFE_SUCCESS)
        {
            /*
            ** Process Software Bus message. If there are fatal errors
            ** in command processing the command can alter the global 
            ** RunStatus variable to exit the main event loop.
            */
            @TEMPLATE@_AppPipe(@TEMPLATE@_AppData.MsgPtr);
            
            /* 
            ** Update Critical Data Store. Because this data is only updated
            ** in one command, this could be moved the command processing function.
            */
            CFE_ES_CopyToCDS(@TEMPLATE@_AppData.CDSHandle, &@TEMPLATE@_AppData.WorkingCriticalData);

        }
        else
        {
            /*
            ** This is an example of exiting on an error.
            ** Note that a SB read error is not always going to 
            ** result in an app quitting. 
            */
            CFE_EVS_SendEvent(@TEMPLATE@_PIPE_ERR_EID, CFE_EVS_ERROR,
                               "@TEMPLATE@: SB Pipe Read Error, @TEMPLATE@ App Will Exit.");
            
            @TEMPLATE@_AppData.RunStatus = CFE_ES_APP_ERROR;
         }
                    
    } /* end while */

    /*
    ** Performance Log Exit Stamp.
    */
    CFE_ES_PerfLogExit(@TEMPLATE@_APPMAIN_PERF_ID);

    /*
    ** Exit the Application.
    */
    CFE_ES_ExitApp(@TEMPLATE@_AppData.RunStatus);
    
} /* End of @TEMPLATE@_AppMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @TEMPLATE@_AppInit() -- @TEMPLATE@ initialization                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 @TEMPLATE@_AppInit(void)
{
    int32  Status;
    int32  ResetType;
    uint32 ResetSubType;

    /* 
    ** Determine Reset Type
    */
    ResetType = CFE_ES_GetResetType(&ResetSubType);
    
    /*
    ** For a Poweron Reset, initialize the Critical variables.
    ** If it is a Processor Reset, these variables will be restored from
    ** the Critical Data Store later in this function.
    */
    if ( ResetType == CFE_ES_POWERON_RESET )
    {
       @TEMPLATE@_AppData.WorkingCriticalData.DataPtOne   = 1;
       @TEMPLATE@_AppData.WorkingCriticalData.DataPtTwo   = 2;
       @TEMPLATE@_AppData.WorkingCriticalData.DataPtThree = 3;
       @TEMPLATE@_AppData.WorkingCriticalData.DataPtFour  = 4;
       @TEMPLATE@_AppData.WorkingCriticalData.DataPtFive  = 5;
    }
    
    /*
    ** Setup the RunStatus variable
    */
    @TEMPLATE@_AppData.RunStatus = CFE_ES_APP_RUN;
    
    /*
    ** Initialize app command execution counters.
    */
    @TEMPLATE@_AppData.CmdCounter = 0;
    @TEMPLATE@_AppData.ErrCounter = 0;

    /*
    ** Initialize app configuration data.
    */
    strcpy(@TEMPLATE@_AppData.PipeName, "@TEMPLATE@_CMD_PIPE");
    @TEMPLATE@_AppData.PipeDepth = @TEMPLATE@_PIPE_DEPTH;

    @TEMPLATE@_AppData.LimitHK   = @TEMPLATE@_LIMIT_HK;
    @TEMPLATE@_AppData.LimitCmd  = @TEMPLATE@_LIMIT_CMD;

    /*
    ** Initialize event filter table for envents we want to filter.
    */
    @TEMPLATE@_AppData.EventFilters[0].EventID = @TEMPLATE@_PROCESS_INF_EID;
    @TEMPLATE@_AppData.EventFilters[0].Mask    = CFE_EVS_EVERY_FOURTH_ONE;
    
    @TEMPLATE@_AppData.EventFilters[1].EventID = @TEMPLATE@_RESET_INF_EID;
    @TEMPLATE@_AppData.EventFilters[1].Mask    = CFE_EVS_NO_FILTER;
    
    @TEMPLATE@_AppData.EventFilters[2].EventID = @TEMPLATE@_CC1_ERR_EID;
    @TEMPLATE@_AppData.EventFilters[2].Mask    = CFE_EVS_EVERY_OTHER_TWO;
    
    @TEMPLATE@_AppData.EventFilters[3].EventID = @TEMPLATE@_LEN_ERR_EID;
    @TEMPLATE@_AppData.EventFilters[3].Mask    = CFE_EVS_FIRST_8_STOP;


    /*
    ** Register event filter table.
    */
    Status = CFE_EVS_Register(@TEMPLATE@_AppData.EventFilters, 4,
                               CFE_EVS_BINARY_FILTER);
    
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("@TEMPLATE@ App: Error Registering Events, RC = 0x%08X\n", Status);
       return ( Status );
    }
             
    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_SB_InitMsg(&@TEMPLATE@_AppData.HkPacket, @TEMPLATE@_HK_TLM_MID, sizeof(@TEMPLATE@_HkPacket_t), TRUE);
   
    /*
    ** Create Software Bus message pipe.
    */
    Status = CFE_SB_CreatePipe(&@TEMPLATE@_AppData.CmdPipe, @TEMPLATE@_AppData.PipeDepth, @TEMPLATE@_AppData.PipeName);
    if ( Status != CFE_SUCCESS )
    {
       /*
       ** Could use an event at this point
       */
       CFE_ES_WriteToSysLog("@TEMPLATE@ App: Error Creating SB Pipe, RC = 0x%08X\n", Status);
       return ( Status );
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_Subscribe(@TEMPLATE@_SEND_HK_MID,@TEMPLATE@_AppData.CmdPipe);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("@TEMPLATE@ App: Error Subscribing to HK Request, RC = 0x%08X\n", Status);
       return ( Status );
    }

    /*
    ** Subscribe to @TEMPLATE@ ground command packets
    */
    Status = CFE_SB_Subscribe(@TEMPLATE@_CMD_MID,@TEMPLATE@_AppData.CmdPipe);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("@TEMPLATE@ App: Error Subscribing to @TEMPLATE@ Command, RC = 0x%08X\n", Status);
       return ( Status );
    }

    /*
    ** Register tables with cFE and load default data
    */
    Status = CFE_TBL_Register(&@TEMPLATE@_AppData.TblHandles[0], "MyFirstTbl",
                              sizeof(@TEMPLATE@_Tbl_1_t), CFE_TBL_OPT_DEFAULT,
                              @TEMPLATE@_FirstTblValidationFunc);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("@TEMPLATE@ App: Error Registering Table 1, RC = 0x%08X\n", Status);
       return ( Status );
    }
    else
    {
       Status = CFE_TBL_Load(@TEMPLATE@_AppData.TblHandles[0], CFE_TBL_SRC_FILE, @TEMPLATE@_FIRST_TBL_FILE);
    }
    
    Status = CFE_TBL_Register(&@TEMPLATE@_AppData.TblHandles[1], "MySecondTbl",
                              sizeof(@TEMPLATE@_Tbl_2_t), CFE_TBL_OPT_DEFAULT,
                              @TEMPLATE@_SecondTblValidationFunc);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("@TEMPLATE@ App: Error Registering Table 2, RC = 0x%08X\n", Status);
       return ( Status );
    }
    else
    {
      Status = CFE_TBL_Load(@TEMPLATE@_AppData.TblHandles[1], CFE_TBL_SRC_FILE, @TEMPLATE@_SECOND_TBL_FILE);
    }
                 

    /*
    ** Create and manage the Critical Data Store 
    */
    Status = CFE_ES_RegisterCDS(&@TEMPLATE@_AppData.CDSHandle, sizeof(@TEMPLATE@_CdsDataType_t), @TEMPLATE@_CDS_NAME);
    if(Status == CFE_SUCCESS)
    {
       /* 
       ** Setup Initial contents of Critical Data Store 
       */
       CFE_ES_CopyToCDS(@TEMPLATE@_AppData.CDSHandle, &@TEMPLATE@_AppData.WorkingCriticalData);
       
    }
    else if(Status == CFE_ES_CDS_ALREADY_EXISTS)
    {
       /* 
       ** Critical Data Store already existed, we need to get a copy 
       ** of its current contents to see if we can use it
       */
       Status = CFE_ES_RestoreFromCDS(&@TEMPLATE@_AppData.WorkingCriticalData, @TEMPLATE@_AppData.CDSHandle);
       if(Status == CFE_SUCCESS)
       {
          /*
          ** Perform any logical verifications, if necessary, to validate data 
          */
          CFE_ES_WriteToSysLog("@TEMPLATE@ App CDS data preserved\n");
       }
       else
       {
          /* 
          ** Restore Failied, Perform baseline initialization 
          */
          @TEMPLATE@_AppData.WorkingCriticalData.DataPtOne   = 1;
          @TEMPLATE@_AppData.WorkingCriticalData.DataPtTwo   = 2;
          @TEMPLATE@_AppData.WorkingCriticalData.DataPtThree = 3;
          @TEMPLATE@_AppData.WorkingCriticalData.DataPtFour  = 4;
          @TEMPLATE@_AppData.WorkingCriticalData.DataPtFive  = 5;
          CFE_ES_WriteToSysLog("Failed to Restore CDS. Re-Initialized CDS Data.\n");
       }
    }
    else 
    {
       /* 
       ** Error creating my critical data store 
       */
       CFE_ES_WriteToSysLog("@TEMPLATE@: Failed to create CDS (Err=0x%08x)", Status);
       return(Status);
    }

    /*
    ** Application startup event message.
    */
    CFE_EVS_SendEvent(@TEMPLATE@_INIT_INF_EID,CFE_EVS_INFORMATION, "@TEMPLATE@: Application Initialized");
                         
    return(CFE_SUCCESS);

} /* End of @TEMPLATE@_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @TEMPLATE@_AppPipe() -- Process command pipe message                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void @TEMPLATE@_AppPipe(CFE_SB_MsgPtr_t msg)
{
    CFE_SB_MsgId_t MessageID;
    uint16 CommandCode;

    MessageID = CFE_SB_GetMsgId(msg);
    switch (MessageID)
    {
        /*
        ** Housekeeping telemetry request.
        */
        case @TEMPLATE@_SEND_HK_MID:
            @TEMPLATE@_HousekeepingCmd(msg);
            break;

        /*
        ** @TEMPLATE@ ground commands.
        */
        case @TEMPLATE@_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(msg);
            switch (CommandCode)
            {
                case @TEMPLATE@_NOOP_CC:
                    @TEMPLATE@_NoopCmd(msg);
                    break;

                case @TEMPLATE@_RESET_CC:
                    @TEMPLATE@_ResetCmd(msg);
                    break;
                    
                case @TEMPLATE@_PROCESS_CC:
                    @TEMPLATE@_RoutineProcessingCmd(msg);
                    break;

                default:
                    CFE_EVS_SendEvent(@TEMPLATE@_CC1_ERR_EID, CFE_EVS_ERROR,
                     "Invalid ground command code: ID = 0x%X, CC = %d",
                                      MessageID, CommandCode);
                    break;
            }
            break;

        default:

            CFE_EVS_SendEvent(@TEMPLATE@_MID_ERR_EID, CFE_EVS_ERROR,
                             "Invalid command pipe message ID: 0x%X",
                              MessageID);
            break;
    }

    return;

} /* End of @TEMPLATE@_AppPipe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @TEMPLATE@_HousekeepingCmd() -- On-board command (HK request)           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void @TEMPLATE@_HousekeepingCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(@TEMPLATE@_NoArgsCmd_t);
    uint16 i;

    /*
    ** Verify command packet length
    */
    if (@TEMPLATE@_VerifyCmdLength(msg, ExpectedLength))
    {
        /*
        ** Get command execution counters
        */
        @TEMPLATE@_AppData.HkPacket.CmdCounter = @TEMPLATE@_AppData.CmdCounter;
        @TEMPLATE@_AppData.HkPacket.ErrCounter = @TEMPLATE@_AppData.ErrCounter;

        /*
        ** Send housekeeping telemetry packet
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &@TEMPLATE@_AppData.HkPacket);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &@TEMPLATE@_AppData.HkPacket);

        /*
        ** Manage any pending table loads, validations, etc.
        */
        for (i=0; i<@TEMPLATE@_NUM_TABLES; i++)
        {
            CFE_TBL_Manage(@TEMPLATE@_AppData.TblHandles[i]);
        }
        
        /*
        ** This command does not affect the command execution counter
        */
        
    } /* end if */

    return;

} /* End of @TEMPLATE@_HousekeepingCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @TEMPLATE@_NoopCmd() -- @TEMPLATE@ ground command (NO-OP)                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void @TEMPLATE@_NoopCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(@TEMPLATE@_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (@TEMPLATE@_VerifyCmdLength(msg, ExpectedLength))
    {
        @TEMPLATE@_AppData.CmdCounter++;

        CFE_EVS_SendEvent(@TEMPLATE@_NOOP_INF_EID, CFE_EVS_INFORMATION,
                         "@TEMPLATE@ Version 1.0.0: No-op command");
    }

    return;

} /* End of @TEMPLATE@_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @TEMPLATE@_ResetCmd() -- @TEMPLATE@ ground command (reset counters)             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void @TEMPLATE@_ResetCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(@TEMPLATE@_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (@TEMPLATE@_VerifyCmdLength(msg, ExpectedLength))
    {
        @TEMPLATE@_AppData.CmdCounter = 0;
        @TEMPLATE@_AppData.ErrCounter = 0;

        CFE_EVS_SendEvent(@TEMPLATE@_RESET_INF_EID, CFE_EVS_INFORMATION,
                         "@TEMPLATE@: Reset Counters command");
    }

    return;

} /* End of @TEMPLATE@_ResetCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  @TEMPLATE@_RoutineProcessingCmd() - Common Processing for each cmd.    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void @TEMPLATE@_RoutineProcessingCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(@TEMPLATE@_NoArgsCmd_t);
    @TEMPLATE@_Tbl_1_t   *MyFirstTblPtr;
    @TEMPLATE@_Tbl_2_t   *MySecondTblPtr;

    /*
    ** Verify command packet length
    */
    if (@TEMPLATE@_VerifyCmdLength(msg, ExpectedLength))
    {
        /* Obtain access to table data addresses */
        CFE_TBL_GetAddress((void *)&MyFirstTblPtr, @TEMPLATE@_AppData.TblHandles[0]);
        CFE_TBL_GetAddress((void *)&MySecondTblPtr, @TEMPLATE@_AppData.TblHandles[1]);
        
        /* Perform routine processing accessing table data via pointers */
        /*                            .                                 */
        /*                            .                                 */
        /*                            .                                 */
        
        /* Once completed with using tables, release addresses          */
        CFE_TBL_ReleaseAddress(@TEMPLATE@_AppData.TblHandles[0]);
        CFE_TBL_ReleaseAddress(@TEMPLATE@_AppData.TblHandles[1]);
        
        /*
        ** Update Critical variables. These variables will be saved
        ** in the Critical Data Store and preserved on a processor reset.
        */
        @TEMPLATE@_AppData.WorkingCriticalData.DataPtOne++;
        @TEMPLATE@_AppData.WorkingCriticalData.DataPtTwo++;
        @TEMPLATE@_AppData.WorkingCriticalData.DataPtThree++;
        @TEMPLATE@_AppData.WorkingCriticalData.DataPtFour++;
        @TEMPLATE@_AppData.WorkingCriticalData.DataPtFive++;
        
        CFE_EVS_SendEvent(@TEMPLATE@_PROCESS_INF_EID,CFE_EVS_INFORMATION, "@TEMPLATE@: Routine Processing Command");
        
        @TEMPLATE@_AppData.CmdCounter++;
    }

    return;

} /* End of @TEMPLATE@_RoutineProcessingCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @TEMPLATE@_VerifyCmdLength() -- Verify command packet length            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean @TEMPLATE@_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{
    boolean result = TRUE;
    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length...
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(msg);
        uint16 CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(@TEMPLATE@_LEN_ERR_EID, CFE_EVS_ERROR,
           "@TEMPLATE@: Invalid cmd pkt: ID = 0x%X,  CC = %d, Len = %d",
                          MessageID, CommandCode, ActualLength);
        result = FALSE;
        @TEMPLATE@_AppData.ErrCounter++;
    }

    return(result);

} /* End of @TEMPLATE@_VerifyCmdLength() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @TEMPLATE@_FirstTblValidationFunc() -- Verify contents of First Table   */
/*                                buffer contents                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 @TEMPLATE@_FirstTblValidationFunc(void *TblData)
{
    int32              ReturnCode = CFE_SUCCESS;
    @TEMPLATE@_Tbl_1_t *TblDataPtr = (@TEMPLATE@_Tbl_1_t *)TblData;
    
    if (TblDataPtr->TblElement1 > @TEMPLATE@_TBL_ELEMENT_1_MAX)
    {
        /* First element is out of range, return an appropriate error code */
        ReturnCode = @TEMPLATE@_TBL_1_ELEMENT_OUT_OF_RANGE_ERR_CODE;
    }
    
    return ReturnCode;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* @TEMPLATE@_SecondTblValidationFunc() -- Verify contents of Second Table */
/*                                 buffer contents                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 @TEMPLATE@_SecondTblValidationFunc(void *TblData)
{
    int32               ReturnCode = CFE_SUCCESS;
    @TEMPLATE@_Tbl_2_t *TblDataPtr = (@TEMPLATE@_Tbl_2_t *)TblData;
    
    if (TblDataPtr->TblElement3 > @TEMPLATE@_TBL_ELEMENT_3_MAX)
    {
        /* Third element is out of range, return an appropriate error code */
        ReturnCode = @TEMPLATE@_TBL_2_ELEMENT_OUT_OF_RANGE_ERR_CODE;
    }
    

    return ReturnCode;
}

/************************/
/*  End of File Comment */
/************************/
