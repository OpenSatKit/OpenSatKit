/* 
** Purpose: Implement a @Template@ application.
**
** Notes:
**   None
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Includes
*/

#include <string.h>
#include "@template@_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

@TEMPLATE@_Class  @Template@;
@TEMPLATE@_HkPkt  @Template@HkPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ (&(@Template@.CmdMgr))
#define  TBLMGR_OBJ (&(@Template@.TblMgr))
#define  EX_OBJ     (&(@Template@.ExObj))
#define  EX_TBL     (&(@Template@.ExTbl))

/******************************************************************************
** Function: @TEMPLATE@_AppMain
**
*/
void @TEMPLATE@_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(@TEMPLATE@_MAIN_PERF_ID);
   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);


   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS)
   {
       Status = InitApp();
   }

   /*
   ** At this point many flight apps use CFE_ES_WaitForStartupSync() to
   ** synchronize their startup timing with other apps. This is not
   ** needed for this simple app.
   */

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      /*
      ** This is just a an example loop. There are many ways to control the
      ** main loop execution flow.
      */
	  
	   CFE_ES_PerfLogExit(@TEMPLATE@_MAIN_PERF_ID);
      OS_TaskDelay(@TEMPLATE@_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(@TEMPLATE@_MAIN_PERF_ID);

      EXOBJ_Execute();

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("@TEMPLATE@ Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(@TEMPLATE@_EXIT_ERR_EID, CFE_EVS_CRITICAL, "@TEMPLATE@ Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(@TEMPLATE@_MAIN_PERF_ID);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of @TEMPLATE@_Main() */


/******************************************************************************
** Function: @TEMPLATE@_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean @TEMPLATE@_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (@TEMPLATE@_CMD_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for @TEMPLATE@ version %d.%d",
                      @TEMPLATE@_MAJOR_VERSION,@TEMPLATE@_MINOR_VERSION);

   return TRUE;


} /* End @TEMPLATE@_NoOpCmd() */


/******************************************************************************
** Function: @TEMPLATE@_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean @TEMPLATE@_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   EXTBL_ResetStatus();
   EXOBJ_ResetStatus();

   return TRUE;

} /* End @TEMPLATE@_ResetAppCmd() */


/******************************************************************************
** Function: @TEMPLATE@_SendHousekeepingPkt
**
*/
void @TEMPLATE@_SendHousekeepingPkt(void)
{

   /* Good design practice in case app expands to more than one table */
   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);

   
   /*
   ** CMDMGR Data
   */

   @Template@HkPkt.ValidCmdCnt   = @Template@.CmdMgr.ValidCmdCnt;
   @Template@HkPkt.InvalidCmdCnt = @Template@.CmdMgr.InvalidCmdCnt;

   
   /*
   ** EXTBL/EXOBJ Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */

   @Template@HkPkt.LastAction       = LastTbl->LastAction;
   @Template@HkPkt.LastActionStatus = LastTbl->LastActionStatus;

   @Template@HkPkt.ExObjExecCnt = @Template@.ExObj.ExecCnt;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &@Template@HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &@Template@HkPkt);

} /* End @TEMPLATE@_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
    int32 Status = CFE_SUCCESS;

    
    /*
    ** Initialize 'entity' objects
    */

    EXTBL_Constructor(EX_TBL, EXOBJ_GetTblPtr, EXOBJ_LoadTbl, EXOBJ_LoadTblEntry);
    EXOBJ_Constructor(EX_OBJ);

    /*
    ** Initialize cFE interfaces 
    */

    CFE_SB_CreatePipe(&@Template@.CmdPipe, @TEMPLATE@_CMD_PIPE_DEPTH, @TEMPLATE@_CMD_PIPE_NAME);
    CFE_SB_Subscribe(@TEMPLATE@_CMD_MID, @Template@.CmdPipe);
    CFE_SB_Subscribe(@TEMPLATE@_SEND_HK_MID, @Template@.CmdPipe);

    /*
    ** Initialize App Framework Components 
    */

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, @TEMPLATE@_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, @TEMPLATE@_ResetAppCmd, 0);
    
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXTBL_LOAD_CMD_FC,  TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXTBL_DUMP_CMD_FC,  TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXOBJ_DEMO_CMD_FC,  EX_OBJ,     EXOBJ_DemoCmd,     EXOBJ_DEMO_CMD_DATA_LEN);

    TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, EXTBL_LoadCmd, EXTBL_DumpCmd, @TEMPLATE@_EXTBL_DEF_LOAD_FILE);
                         
    CFE_SB_InitMsg(&@Template@HkPkt, @TEMPLATE@_HK_TLM_MID, @TEMPLATE@_TLM_HK_LEN, TRUE);

                        
    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(@TEMPLATE@_INIT_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "@TEMPLATE@ Initialized. Version %d.%d.%d.%d",
                               @TEMPLATE@_MAJOR_VERSION,
                               @TEMPLATE@_MINOR_VERSION,
                               @TEMPLATE@_REVISION,
                               @TEMPLATE@_MISSION_REV);

    return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
*/
static void ProcessCommands(void)
{

   int32           Status;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, @Template@.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case @TEMPLATE@_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case @TEMPLATE@_SEND_HK_MID:
            @TEMPLATE@_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(@TEMPLATE@_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */
