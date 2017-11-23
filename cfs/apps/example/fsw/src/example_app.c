/* 
** Purpose: Implement a Example application.
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
#include "example_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

EXAMPLE_Class  Example;
EXAMPLE_HkPkt  ExampleHkPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ (&(Example.CmdMgr))
#define  TBLMGR_OBJ (&(Example.TblMgr))
#define  EX_OBJ     (&(Example.ExObj))
#define  EX_TBL     (&(Example.ExTbl))

/******************************************************************************
** Function: EXAMPLE_AppMain
**
*/
void EXAMPLE_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(EXAMPLE_MAIN_PERF_ID);
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
	  
	  CFE_ES_PerfLogExit(EXAMPLE_MAIN_PERF_ID);
      OS_TaskDelay(EXAMPLE_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(EXAMPLE_MAIN_PERF_ID);

      EXOBJ_Execute();

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("EXAMPLE Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(EXAMPLE_EXIT_ERR_EID, CFE_EVS_CRITICAL, "EXAMPLE Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(EXAMPLE_MAIN_PERF_ID);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of EXAMPLE_Main() */


/******************************************************************************
** Function: EXAMPLE_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean EXAMPLE_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (EXAMPLE_CMD_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for EXAMPLE version %d.%d",
                      EXAMPLE_MAJOR_VERSION,EXAMPLE_MINOR_VERSION);

   return TRUE;


} /* End EXAMPLE_NoOpCmd() */


/******************************************************************************
** Function: EXAMPLE_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean EXAMPLE_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   EXTBL_ResetStatus();
   EXOBJ_ResetStatus();

   return TRUE;

} /* End EXAMPLE_ResetAppCmd() */


/******************************************************************************
** Function: EXAMPLE_SendHousekeepingPkt
**
*/
void EXAMPLE_SendHousekeepingPkt(void)
{

   /* Good design practice in case app expands to more than on etable */
   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);

   
   /*
   ** CMDMGR Data
   */

   ExampleHkPkt.ValidCmdCnt   = Example.CmdMgr.ValidCmdCnt;
   ExampleHkPkt.InvalidCmdCnt = Example.CmdMgr.InvalidCmdCnt;

   
   /*
   ** EXTABL/EXOBJ Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */

   ExampleHkPkt.LastAction       = LastTbl->LastAction;
   ExampleHkPkt.LastActionStatus = LastTbl->LastActionStatus;

   ExampleHkPkt.ExObjExecCnt = Example.ExObj.ExecCnt;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &ExampleHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &ExampleHkPkt);

} /* End EXAMPLE_SendHousekeepingPkt() */


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

    CFE_SB_CreatePipe(&Example.CmdPipe, EXAMPLE_CMD_PIPE_DEPTH, EXAMPLE_CMD_PIPE_NAME);
    CFE_SB_Subscribe(EXAMPLE_CMD_MID, Example.CmdPipe);
    CFE_SB_Subscribe(EXAMPLE_SEND_HK_MID, Example.CmdPipe);

    /*
    ** Initialize App Framework Components 
    */

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, EXAMPLE_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, EXAMPLE_ResetAppCmd, 0);
    
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXTBL_LOAD_CMD_FC,  TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXTBL_DUMP_CMD_FC,  TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXOBJ_DEMO_CMD_FC,  EX_OBJ,     EXOBJ_DemoCmd,     EXOBJ_DEMO_CMD_DATA_LEN);

    TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, EXTBL_LoadCmd, EXTBL_DumpCmd, EXAMPLE_EXTBL_DEF_LOAD_FILE);
                         
    CFE_SB_InitMsg(&ExampleHkPkt, EXAMPLE_TLM_HK_MID, EXAMPLE_TLM_HK_LEN, TRUE);

                        
    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(EXAMPLE_INIT_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "EXAMPLE Initialized. Version %d.%d.%d.%d",
                               EXAMPLE_MAJOR_VERSION,
                               EXAMPLE_MINOR_VERSION,
                               EXAMPLE_REVISION,
                               EXAMPLE_MISSION_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, Example.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case EXAMPLE_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case EXAMPLE_SEND_HK_MID:
            EXAMPLE_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(EXAMPLE_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */
