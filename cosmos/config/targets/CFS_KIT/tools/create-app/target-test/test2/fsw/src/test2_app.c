/* 
** Purpose: Implement a Test2 application.
**
** Notes:
**   1. Template written by David McComas and licensed under the GNU
**      Lesser General Public License (LGPL).
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Includes
*/

#include <string.h>
#include "test2_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

TEST2_Class  Test2;
TEST2_HkPkt  Test2HkPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ (&(Test2.CmdMgr))
#define  TBLMGR_OBJ (&(Test2.TblMgr))
#define  EX_OBJ     (&(Test2.ExObj))
#define  EX_TBL     (&(Test2.ExTbl))

/******************************************************************************
** Function: TEST2_Main
**
*/
void TEST2_Main(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(TEST2_MAIN_PERF_ID);
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
   ** synchronize their startup timing with other apps. 
   */

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      /*
      ** This is just a an example loop. There are many ways to control the
      ** main loop execution.
      */
	    CFE_ES_PerfLogExit(TEST2_MAIN_PERF_ID);
      OS_TaskDelay(TEST2_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(TEST2_MAIN_PERF_ID);

      EXOBJ_Execute();

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("TEST2 Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(TEST2_EXIT_ERR_EID, CFE_EVS_CRITICAL, "TEST2 Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(TEST2_MAIN_PERF_ID);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of TEST2_Main() */


/******************************************************************************
** Function: TEST2_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean TEST2_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (TEST2_CMD_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for TEST2 version %d.%d",
                      TEST2_MAJOR_VERSION,TEST2_MINOR_VERSION);

   return TRUE;


} /* End TEST2_NoOpCmd() */


/******************************************************************************
** Function: TEST2_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean TEST2_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   EXTBL_ResetStatus();
   EXOBJ_ResetStatus();

   return TRUE;

} /* End TEST2_ResetAppCmd() */


/******************************************************************************
** Function: TEST2_SendHousekeepingPkt
**
*/
void TEST2_SendHousekeepingPkt(void)
{

   /*
   ** CMDMGR Data
   */

   Test2HkPkt.ValidCmdCnt   = Test2.CmdMgr.ValidCmdCnt;
   Test2HkPkt.InvalidCmdCnt = Test2.CmdMgr.InvalidCmdCnt;


   /*
   ** EXOBJ Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */

   Test2HkPkt.ExTblLastLoadValid  = Test2.ExTbl.LastLoadValid;
   Test2HkPkt.ExTblErrCnt         = Test2.ExTbl.ErrCnt;

   Test2HkPkt.ExObjExecCnt = Test2.ExObj.ExecCnt;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Test2HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &Test2HkPkt);

} /* End TEST2_SendHousekeepingPkt() */


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

    EXTBL_Constructor(EX_TBL,EXOBJ_LoadTbl,EXOBJ_LoadTblEntry,EXOBJ_GetTblDataPtr);
    EXOBJ_Constructor(EX_OBJ);

    /*
    ** Initialize cFE interfaces 
    */

    CFE_SB_CreatePipe(&Test2.CmdPipe, TEST2_CMD_PIPE_DEPTH, TEST2_CMD_PIPE_NAME);
    CFE_SB_Subscribe(TEST2_CMD_MID, Test2.CmdPipe);
    CFE_SB_Subscribe(TEST2_SEND_HK_MID, Test2.CmdPipe);

    CFE_SB_InitMsg(&Test2HkPkt, TEST2_TLM_HK_MID, TEST2_TLM_HK_LEN, TRUE);

    /*
    ** Initialize App Framework Components 
    */

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, TEST2_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, TEST2_ResetAppCmd, 0);
    
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXTBL_LOAD_CMD_FC,  TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXTBL_DUMP_CMD_FC,  TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXOBJ_DEMO_CMD_FC,  EX_OBJ,     EXOBJ_DemoCmd,     EXOBJ_DEMO_CMD_DATA_LEN);

    TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, EXTBL_LoadCmd, EXTBL_DumpCmd, TEST2_EXTBL_DEF_LOAD_FILE);
                         
                        
    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(TEST2_INIT_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "TEST2 Initialized. Version %d.%d.%d.%d",
                               TEST2_MAJOR_VERSION,
                               TEST2_MINOR_VERSION,
                               TEST2_REVISION,
                               TEST2_MISSION_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, Test2.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case TEST2_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case TEST2_SEND_HK_MID:
            TEST2_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(TEST2_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */
