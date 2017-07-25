/* 
** Purpose: Implement a demo application. This app exercises the OpenSat 
**          application framework and serves as a good starting point for
**          trying new ideas.
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Includes
*/

#include <string.h>
#include "fw_demo_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);
static void LoadDataFromTbl(void);

/*
** Global Data
*/

FW_DEMO_Class  FwDemo;
FW_DEMO_HkPkt  FwDemoHkPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ   (&(FwDemo.CmdMgr))
#define  TBLMGR_OBJ   (&(FwDemo.TblMgr))
#define  EXOBJ_OBJ    (&(FwDemo.ExObj))
#define  XMLTBL_OBJ   (&(FwDemo.XmlTbl))
#define  SCANFTBL_OBJ (&(FwDemo.ScanfTbl))

/******************************************************************************
** Function: FW_DEMO_Main
**
*/
void FW_DEMO_Main(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(FW_DEMO_MAIN_PERF_ID);
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
   ** needed for this demo.
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
	    CFE_ES_PerfLogExit(FW_DEMO_MAIN_PERF_ID);
      OS_TaskDelay(FW_DEMO_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(FW_DEMO_MAIN_PERF_ID);

      LoadDataFromTbl();

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("FW_DEMO Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(FW_DEMO_EXIT_ERR_EID, CFE_EVS_CRITICAL, "FW_DEMO Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(FW_DEMO_MAIN_PERF_ID);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of FW_DEMO_Main() */


/******************************************************************************
** Function: FW_DEMO_NoOpCmd
**
*/

boolean FW_DEMO_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (FW_DEMO_CMD_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for FW_DEMO version %d.%d",
                      FW_DEMO_MAJOR_VERSION,FW_DEMO_MINOR_VERSION);

   return TRUE;


} /* End FW_DEMO_NoOpCmd() */


/******************************************************************************
** Function: FW_DEMO_ResetAppCmd
**
*/

boolean FW_DEMO_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   EXOBJ_ResetStatus();
   XMLTBL_ResetStatus();
   SCANFTBL_ResetStatus();
   
   return TRUE;

} /* End FW_DEMO_ResetAppCmd() */


/******************************************************************************
** Function: FW_DEMO_SendHousekeepingPkt
**
*/
void FW_DEMO_SendHousekeepingPkt(void)
{

   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);
   
   /*
   ** CMDMGR Data
   */

   FwDemoHkPkt.ValidCmdCnt   = FwDemo.CmdMgr.ValidCmdCnt;
   FwDemoHkPkt.InvalidCmdCnt = FwDemo.CmdMgr.InvalidCmdCnt;

   /*
   ** EXTBL Data
   */

   
   FwDemoHkPkt.LastAction       = LastTbl->LastAction;
   FwDemoHkPkt.LastActionStatus = LastTbl->LastActionStatus;

   /*
   ** EXOBJ Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */

   FwDemoHkPkt.EnableDataLoad = FwDemo.ExObj.EnableDataLoad;
   FwDemoHkPkt.TblIndex       = FwDemo.ExObj.TblIndex;
   FwDemoHkPkt.Data1          = FwDemo.ExObj.Data1;
   FwDemoHkPkt.Data2          = FwDemo.ExObj.Data2;
   FwDemoHkPkt.Data3          = FwDemo.ExObj.Data3;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FwDemoHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &FwDemoHkPkt);

} /* End FW_DEMO_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
    int32 Status = CFE_SUCCESS;

    OS_printf("FW_DEMO_InitApp() Entry\n");
    
    /*
    ** Initialize 'entity' objects
    */

    XMLTBL_Constructor(XMLTBL_OBJ, EXOBJ_GetXmlTblPtr, EXOBJ_LoadXmlTbl, EXOBJ_LoadXmlTblEntry);
    SCANFTBL_Constructor(SCANFTBL_OBJ, EXOBJ_GetScanfTblPtr, EXOBJ_LoadScanfTbl, EXOBJ_LoadScanfTblEntry);
    
    EXOBJ_Constructor(EXOBJ_OBJ);

    /*
    ** Initialize application managers
    */

    CFE_SB_CreatePipe(&FwDemo.CmdPipe, FW_DEMO_CMD_PIPE_DEPTH, FW_DEMO_CMD_PIPE_NAME);
    CFE_SB_Subscribe(FW_DEMO_CMD_MID, FwDemo.CmdPipe);
    CFE_SB_Subscribe(FW_DEMO_SEND_HK_MID, FwDemo.CmdPipe);

    OS_printf("FW_DEMO_InitApp() Before CMDMGR calls\n");
    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, FW_DEMO_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, FW_DEMO_ResetAppCmd, 0);
    
    CMDMGR_RegisterFunc(CMDMGR_OBJ, FW_DEMO_EXOBJ_TBL_LOAD_CMD_FC,  TBLMGR_OBJ, TBLMGR_LoadTblCmd,      TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, FW_DEMO_EXOBJ_TBL_DUMP_CMD_FC,  TBLMGR_OBJ, TBLMGR_DumpTblCmd,      TBLMGR_DUMP_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, FW_DEMO_ENA_DATA_LOAD_CMD_FC,   EXOBJ_OBJ,  EXOBJ_EnableDataLoadCmd, EXOBJ_ENABLE_DATA_LOAD_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, FW_DEMO_SET_TBL_INDEX_CMD_FC,   EXOBJ_OBJ,  EXOBJ_SetTblIndexCmd,    EXOBJ_SET_TBL_INDEX_CMD_DATA_LEN);

    OS_printf("FW_DEMO_InitApp() Before TBLMGR calls\n");
    TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, XMLTBL_LoadCmd, XMLTBL_DumpCmd, FW_DEMO_XML_TBL_DEF_LOAD_FILE);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, SCANFTBL_LoadCmd, SCANFTBL_DumpCmd, FW_DEMO_SCANF_TBL_DEF_LOAD_FILE);
                         
                         
    CFE_SB_InitMsg(&FwDemoHkPkt, FW_DEMO_TLM_HK_MID, FW_DEMO_TLM_HK_LEN, TRUE);

    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(FW_DEMO_INIT_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "FW_DEMO Initialized. Version %d.%d.%d.%d",
                               FW_DEMO_MAJOR_VERSION,
                               FW_DEMO_MINOR_VERSION,
                               FW_DEMO_REVISION,
                               FW_DEMO_MISSION_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, FwDemo.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case FW_DEMO_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case FW_DEMO_SEND_HK_MID:
            FW_DEMO_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(FW_DEMO_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */

/******************************************************************************
** Function: LoadDataFromTbl
**
*/
static void LoadDataFromTbl(void)
{
   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);
  
   /* Kludgey since ID's are registration order dependent */
   
   switch (LastTbl->Id) {
      case 0:
         EXOBJ_LoadDataFromXmlTbl();
         break;
      case 1:
         EXOBJ_LoadDataFromScanfTbl();
         break;
      default:
         /* Do nothing. Don't flood events */
         break;
   }	

} /* End LoadDataFromTbl() */

/* end of file */
