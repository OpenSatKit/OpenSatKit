/* 
** Purpose: Implement a demo application. This app exercises the OpenSat 
**          application framework and serves as a good starting point for
**          trying new ideas.
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Includes
*/

#include <string.h>
#include "osk_demo_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);
static void LoadDataFromTbl(void);

/*
** Global Data
*/

OSK_DEMO_Class  OskDemo;
OSK_DEMO_HkPkt  OskDemoHkPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ   (&(OskDemo.CmdMgr))
#define  TBLMGR_OBJ   (&(OskDemo.TblMgr))
#define  DEMOBJ_OBJ   (&(OskDemo.DemObj))
#define  XMLTBL_OBJ   (&(OskDemo.XmlTbl))
#define  SCANFTBL_OBJ (&(OskDemo.ScanfTbl))
#define  JSONTBL_OBJ  (&(OskDemo.JsonTbl))

/******************************************************************************
** Function: OSK_DEMO_Main
**
*/
void OSK_DEMO_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(OSK_DEMO_MAIN_PERF_ID);
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

	  CFE_ES_PerfLogExit(OSK_DEMO_MAIN_PERF_ID);
      OS_TaskDelay(OSK_DEMO_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(OSK_DEMO_MAIN_PERF_ID);

      LoadDataFromTbl();

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("OSK_DEMO Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(OSK_DEMO_EXIT_ERR_EID, CFE_EVS_CRITICAL, "OSK_DEMO Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(OSK_DEMO_MAIN_PERF_ID);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of OSK_DEMO_Main() */


/******************************************************************************
** Function: OSK_DEMO_NoOpCmd
**
*/

boolean OSK_DEMO_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (OSK_DEMO_CMD_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for OSK_DEMO version %d.%d",
                      OSK_DEMO_MAJOR_VERSION,OSK_DEMO_MINOR_VERSION);

   return TRUE;


} /* End OSK_DEMO_NoOpCmd() */


/******************************************************************************
** Function: OSK_DEMO_ResetAppCmd
**
*/

boolean OSK_DEMO_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   DEMOBJ_ResetStatus();
   XMLTBL_ResetStatus();
   SCANFTBL_ResetStatus();
   JSONTBL_ResetStatus();
   
   return TRUE;

} /* End OSK_DEMO_ResetAppCmd() */


/******************************************************************************
** Function: OSK_DEMO_SendHousekeepingPkt
**
*/
void OSK_DEMO_SendHousekeepingPkt(void)
{

   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);
   
   /*
   ** CMDMGR Data
   */

   OskDemoHkPkt.ValidCmdCnt   = OskDemo.CmdMgr.ValidCmdCnt;
   OskDemoHkPkt.InvalidCmdCnt = OskDemo.CmdMgr.InvalidCmdCnt;

   
   /*
   ** EXTBL Data
   */
 
   OskDemoHkPkt.LastAction       = LastTbl->LastAction;
   OskDemoHkPkt.LastActionStatus = LastTbl->LastActionStatus;

   /*
   ** DEMOBJ Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */

   OskDemoHkPkt.EnableDataLoad = OskDemo.DemObj.EnableDataLoad;
   OskDemoHkPkt.TblIndex       = OskDemo.DemObj.TblIndex;
   OskDemoHkPkt.Data1          = OskDemo.DemObj.Data1;
   OskDemoHkPkt.Data2          = OskDemo.DemObj.Data2;
   OskDemoHkPkt.Data3          = OskDemo.DemObj.Data3;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &OskDemoHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &OskDemoHkPkt);

} /* End OSK_DEMO_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
    int32 Status = CFE_SUCCESS;

    CFE_EVS_SendEvent(OSK_DEMO_INIT_DEBUG_EID, OSK_DEMO_INIT_EVS_TYPE, "OSK_DEMO_InitApp() Entry\n");
    
    /*
    ** Initialize 'entity' objects
    */

    XMLTBL_Constructor(XMLTBL_OBJ, DEMOBJ_GetXmlTblPtr, DEMOBJ_LoadXmlTbl, DEMOBJ_LoadXmlTblEntry);
    SCANFTBL_Constructor(SCANFTBL_OBJ, DEMOBJ_GetScanfTblPtr, DEMOBJ_LoadScanfTbl, DEMOBJ_LoadScanfTblEntry);
    JSONTBL_Constructor(JSONTBL_OBJ, DEMOBJ_GetJsonTblPtr, DEMOBJ_LoadJsonTbl, DEMOBJ_LoadJsonTblEntry);
    
    DEMOBJ_Constructor(DEMOBJ_OBJ);

    /*
    ** Initialize application managers
    */

    CFE_SB_CreatePipe(&OskDemo.CmdPipe, OSK_DEMO_CMD_PIPE_DEPTH, OSK_DEMO_CMD_PIPE_NAME);
    CFE_SB_Subscribe(OSK_DEMO_CMD_MID, OskDemo.CmdPipe);
    CFE_SB_Subscribe(OSK_DEMO_SEND_HK_MID, OskDemo.CmdPipe);

    CFE_EVS_SendEvent(OSK_DEMO_INIT_DEBUG_EID, OSK_DEMO_INIT_EVS_TYPE, "OSK_DEMO_InitApp() Before CMDMGR calls\n");
    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, OSK_DEMO_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, OSK_DEMO_ResetAppCmd, 0);
    
    CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_DEMO_DEMOBJ_TBL_LOAD_CMD_FC, TBLMGR_OBJ, TBLMGR_LoadTblCmd,        TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_DEMO_DEMOBJ_TBL_DUMP_CMD_FC, TBLMGR_OBJ, TBLMGR_DumpTblCmd,        TBLMGR_DUMP_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_DEMO_ENA_DATA_LOAD_CMD_FC,   DEMOBJ_OBJ, DEMOBJ_EnableDataLoadCmd, DEMOBJ_ENABLE_DATA_LOAD_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_DEMO_SET_TBL_INDEX_CMD_FC,   DEMOBJ_OBJ, DEMOBJ_SetTblIndexCmd,    DEMOBJ_SET_TBL_INDEX_CMD_DATA_LEN);

    CFE_EVS_SendEvent(OSK_DEMO_INIT_DEBUG_EID, OSK_DEMO_INIT_EVS_TYPE, "OSK_DEMO_InitApp() Before TBLMGR calls\n");
    TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, XMLTBL_LoadCmd,   XMLTBL_DumpCmd,   OSK_DEMO_XML_TBL_DEF_LOAD_FILE);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, SCANFTBL_LoadCmd, SCANFTBL_DumpCmd, OSK_DEMO_SCANF_TBL_DEF_LOAD_FILE);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, JSONTBL_LoadCmd,  JSONTBL_DumpCmd,  OSK_DEMO_JSON_TBL_DEF_LOAD_FILE);
                         
                         
    CFE_SB_InitMsg(&OskDemoHkPkt, OSK_DEMO_TLM_HK_MID, OSK_DEMO_TLM_HK_LEN, TRUE);

    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(OSK_DEMO_INIT_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "OSK_DEMO Initialized. Version %d.%d.%d.%d",
                               OSK_DEMO_MAJOR_VERSION,
                               OSK_DEMO_MINOR_VERSION,
                               OSK_DEMO_REVISION,
                               OSK_DEMO_MISSION_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, OskDemo.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case OSK_DEMO_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case OSK_DEMO_SEND_HK_MID:
            OSK_DEMO_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(OSK_DEMO_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
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
         DEMOBJ_LoadDataFromXmlTbl();
         break;
      case 1:
         DEMOBJ_LoadDataFromScanfTbl();
         break;
      case 2:
         DEMOBJ_LoadDataFromJsonTbl();
         break;
      default:
         /* Do nothing. Don't flood events */
         break;
   }	

} /* End LoadDataFromTbl() */

/* end of file */
