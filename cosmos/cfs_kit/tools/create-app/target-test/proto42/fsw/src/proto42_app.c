/* 
** Purpose: Implement a Proto42 application.
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
#include "proto42_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

PROTO42_Class  Proto42;
PROTO42_HkPkt  Proto42HkPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ (&(Proto42.CmdMgr))
#define  TBLMGR_OBJ (&(Proto42.TblMgr))
#define  EX_OBJ     (&(Proto42.ExObj))
#define  EX_TBL     (&(Proto42.ExTbl))

/******************************************************************************
** Function: PROTO42_Main
**
*/
void PROTO42_Main(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(PROTO42_MAIN_PERF_ID);
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
	    CFE_ES_PerfLogExit(PROTO42_MAIN_PERF_ID);
      OS_TaskDelay(PROTO42_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(PROTO42_MAIN_PERF_ID);

      EXOBJ_Execute();

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("PROTO42 Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(PROTO42_EXIT_ERR_EID, CFE_EVS_CRITICAL, "PROTO42 Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(PROTO42_MAIN_PERF_ID);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of PROTO42_Main() */


/******************************************************************************
** Function: PROTO42_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean PROTO42_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (PROTO42_CMD_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for PROTO42 version %d.%d",
                      PROTO42_MAJOR_VERSION,PROTO42_MINOR_VERSION);

   return TRUE;


} /* End PROTO42_NoOpCmd() */


/******************************************************************************
** Function: PROTO42_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean PROTO42_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   EXTBL_ResetStatus();
   EXOBJ_ResetStatus();

   return TRUE;

} /* End PROTO42_ResetAppCmd() */


/******************************************************************************
** Function: PROTO42_SendHousekeepingPkt
**
*/
void PROTO42_SendHousekeepingPkt(void)
{

   /*
   ** CMDMGR Data
   */

   Proto42HkPkt.ValidCmdCnt   = Proto42.CmdMgr.ValidCmdCnt;
   Proto42HkPkt.InvalidCmdCnt = Proto42.CmdMgr.InvalidCmdCnt;


   /*
   ** EXOBJ Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */

   Proto42HkPkt.ExTblLastLoadValid  = Proto42.ExTbl.LastLoadValid;
   Proto42HkPkt.ExTblErrCnt         = Proto42.ExTbl.ErrCnt;

   Proto42HkPkt.ExObjExecCnt = Proto42.ExObj.ExecCnt;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Proto42HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &Proto42HkPkt);

} /* End PROTO42_SendHousekeepingPkt() */


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

    CFE_SB_CreatePipe(&Proto42.CmdPipe, PROTO42_CMD_PIPE_DEPTH, PROTO42_CMD_PIPE_NAME);
    CFE_SB_Subscribe(PROTO42_CMD_MID, Proto42.CmdPipe);
    CFE_SB_Subscribe(PROTO42_SEND_HK_MID, Proto42.CmdPipe);

    CFE_SB_InitMsg(&Proto42HkPkt, PROTO42_TLM_HK_MID, PROTO42_TLM_HK_LEN, TRUE);

    /*
    ** Initialize App Framework Components 
    */

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_CMD_NOOP_FC,  NULL, PROTO42_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_CMD_RESET_FC, NULL, PROTO42_ResetAppCmd, 0);
    
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXTBL_LOAD_CMD_FC,  TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXTBL_DUMP_CMD_FC,  TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EXOBJ_DEMO_CMD_FC,  EX_OBJ,     EXOBJ_DemoCmd,     EXOBJ_DEMO_CMD_DATA_LEN);

    TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, EXTBL_LoadCmd, EXTBL_DumpCmd, PROTO42_EXTBL_DEF_LOAD_FILE);
                         
                        
    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(PROTO42_INIT_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "PROTO42 Initialized. Version %d.%d.%d.%d",
                               PROTO42_MAJOR_VERSION,
                               PROTO42_MINOR_VERSION,
                               PROTO42_REVISION,
                               PROTO42_MISSION_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, Proto42.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case PROTO42_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case PROTO42_SEND_HK_MID:
            PROTO42_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(PROTO42_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */
