/* 
** Purpose: Implement a proto application. This app exercises the OpenSat 
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
#include "osk_c_proto_app.h"
#include "cfetbl.h"

/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

OSK_C_PROTO_Class  OskCProto;


/*
** Convenience Macros
*/

#define  CMDMGR_OBJ   (&(OskCProto.CmdMgr))
#define  TBLMGR_OBJ   (&(OskCProto.TblMgr))
#define  PROTOBJ_OBJ  (&(OskCProto.ProtObj))
#define  STATEREP_OBJ (&(OskCProto.StateRep))
#define  PROTOFR_OBJ  (&(OskCProto.ProtoFr))

/******************************************************************************
** Function: OSK_C_PROTO_Main
**
*/
void OSK_C_PROTO_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(OSK_C_PROTO_MAIN_PERF_ID);
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
   ** needed for this proto.
   */

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      /*
      ** This is just an example loop. There are many ways to control the
      ** main loop execution.
      */

      CFE_ES_PerfLogExit(OSK_C_PROTO_MAIN_PERF_ID);
      OS_TaskDelay(OSK_C_PROTO_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(OSK_C_PROTO_MAIN_PERF_ID);

      PROTOBJ_Execute();
      PROTOFR_SimStep();
      
      STATEREP_GenTlmMsg(STATEREP_OBJ, &OskCProto.SrPkt);
      CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &OskCProto.SrPkt);
      CFE_SB_SendMsg((CFE_SB_Msg_t *) &OskCProto.SrPkt);

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("OSK_C_PROTO Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(OSK_C_PROTO_EXIT_ERR_EID, CFE_EVS_CRITICAL, "OSK_C_PROTO Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(OSK_C_PROTO_MAIN_PERF_ID);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of OSK_C_PROTO_Main() */


/******************************************************************************
** Function: OSK_C_PROTO_NoOpCmd
**
*/

boolean OSK_C_PROTO_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (OSK_C_PROTO_CMD_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for OSK_C_PROTO version %d.%d.%d",
                      OSK_C_PROTO_MAJOR_VER,OSK_C_PROTO_MINOR_VER,OSK_C_PROTO_PLATFORM_REV);

   return TRUE;


} /* End OSK_C_PROTO_NoOpCmd() */


/******************************************************************************
** Function: OSK_C_PROTO_ResetAppCmd
**
*/

boolean OSK_C_PROTO_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   /* IBD - SHould StateRep include a reset? */
   
   PROTOBJ_ResetStatus();
   PROTOFR_ResetStatus();
  
   return TRUE;

} /* End OSK_C_PROTO_ResetAppCmd() */


/******************************************************************************
** Function: OSK_C_PROTO_SendHousekeepingPkt
**
*/
void OSK_C_PROTO_SendHousekeepingPkt(void)
{

   const TBLMGR_Tbl* Tbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);
      
   /*
   ** CMDMGR Data
   */

   OskCProto.HkPkt.ValidCmdCnt   = OskCProto.CmdMgr.ValidCmdCnt;
   OskCProto.HkPkt.InvalidCmdCnt = OskCProto.CmdMgr.InvalidCmdCnt;

   
   /*
   ** EXTBL Data
   */
 
   OskCProto.HkPkt.LastTblId           = Tbl->Id;
   OskCProto.HkPkt.LastTblAction       = Tbl->LastAction;
   OskCProto.HkPkt.LastTblActionStatus = Tbl->LastActionStatus;

   /*
   ** PROTOFR & STATEREP Data
   */
   
   OskCProto.HkPkt.StateRepTlmMode  = OskCProto.StateRep.TlmMode;
   OskCProto.HkPkt.SimEnabled       = OskCProto.ProtoFr.SimEnabled;
   OskCProto.HkPkt.SimMode          = OskCProto.ProtoFr.SimMode;
   
   OskCProto.HkPkt.StateRepEnabled  = OskCProto.StateRep.BitConfig.Enabled[0];
   OskCProto.HkPkt.StateRepLatched  = OskCProto.StateRep.BitConfig.Latched[0];
  

   /*
   ** PROTOBJ Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */

   OskCProto.HkPkt.TblDataEnabled = PROTOBJ_GetTblData(&OskCProto.HkPkt.TblData);
   OskCProto.HkPkt.TblId    = OskCProto.ProtObj.TblId;
   OskCProto.HkPkt.TblIndex = OskCProto.ProtObj.TblIndex;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &OskCProto.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &OskCProto.HkPkt);

} /* End OSK_C_PROTO_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = CFE_SUCCESS;

   CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "OSK_C_PROTO_InitApp() Entry\n");
    
   /*
   ** Initialize 'entity' objects
   */
 
   PROTOBJ_Constructor(PROTOBJ_OBJ);
   PROTOFR_Constructor(PROTOFR_OBJ, STATEREP_OBJ);

   /*
   ** Initialize application managers
   */

   CFE_SB_CreatePipe(&OskCProto.CmdPipe, OSK_C_PROTO_CMD_PIPE_DEPTH, "C_PROTO"); //OSK_C_PROTO_CMD_PIPE_NAME);
   CFE_SB_Subscribe(OSK_C_PROTO_CMD_MID, OskCProto.CmdPipe);
   CFE_SB_Subscribe(OSK_C_PROTO_SEND_HK_MID, OskCProto.CmdPipe);

   CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "OSK_C_PROTO_InitApp() Before CMDMGR calls\n");
   CMDMGR_Constructor(CMDMGR_OBJ);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, OSK_C_PROTO_NoOpCmd,     0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, OSK_C_PROTO_ResetAppCmd, 0);
    
   CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_PROTO_PROTOBJ_TBL_LOAD_CMD_FC,     TBLMGR_OBJ,   TBLMGR_LoadTblCmd,        TBLMGR_LOAD_TBL_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_PROTO_PROTOBJ_TBL_DUMP_CMD_FC,     TBLMGR_OBJ,   TBLMGR_DumpTblCmd,        TBLMGR_DUMP_TBL_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_PROTO_ENA_TBL_DATA_CMD_FC,         PROTOBJ_OBJ,  PROTOBJ_EnableTblDataCmd, PROTOBJ_ENABLE_TBL_DATA_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_PROTO_SET_ACTIVE_TBL_CMD_FC,       PROTOBJ_OBJ,  PROTOBJ_SetActiveTblCmd,  PROTOBJ_SET_ACTIVE_TBL_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_PROTO_STATEREP_CFG_CMD_FC,         STATEREP_OBJ, STATEREP_ConfigBitCmd,    STATEREP_CONFIG_BIT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_PROTO_STATEREP_CLR_CMD_FC,         STATEREP_OBJ, STATEREP_ClearBitCmd,     STATEREP_CLEAR_BIT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_PROTO_PROTOFR_SET_TLM_MODE_CMD_FC, PROTOFR_OBJ,  PROTOFR_SetTlmModeCmd,    PROTOFR_SET_TLM_MODE_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_PROTO_PROTOFR_SIM_FAULT_CMD_FC,    PROTOFR_OBJ,  PROTOFR_SimFaultCmd,      PROTOFR_SIM_FAULT_CMD_DATA_LEN);

   CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "OSK_C_PROTO_InitApp() Before TBLMGR calls\n");
    
   /*
   ** Table IDs are assigned during registration so the order is important. COSMOS cmd/tlm definitions assume
   ** 0=XML, 1=SCANF, 2=JSON.
   */
   TBLMGR_Constructor(TBLMGR_OBJ);
   TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, XMLTBL_LoadCmd,   XMLTBL_DumpCmd,   OSK_C_PROTO_XML_TBL_DEF_LOAD_FILE);
   TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, SCANFTBL_LoadCmd, SCANFTBL_DumpCmd, OSK_C_PROTO_SCANF_TBL_DEF_LOAD_FILE);
   TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, JSONTBL_LoadCmd,  JSONTBL_DumpCmd,  OSK_C_PROTO_JSON_TBL_DEF_LOAD_FILE);

   STATEREP_Constructor(STATEREP_OBJ, OSK_C_PROTO_STATEREP_BIT_ID_MAX);
    
   CFE_SB_InitMsg(&OskCProto.SrPkt, OSK_C_PROTO_TLM_SR_MID, STATEREP_TLM_PKT_LEN, TRUE);
                                          
   CFE_SB_InitMsg(&OskCProto.HkPkt, OSK_C_PROTO_TLM_HK_MID, OSK_C_PROTO_TLM_HK_LEN, TRUE);

   /*
   ** Application startup event message
   */
   Status = CFE_EVS_SendEvent(OSK_C_PROTO_INIT_INFO_EID, CFE_EVS_INFORMATION,
                              "OSK_C_PROTO Initialized. Version %d.%d.%d",
                              OSK_C_PROTO_MAJOR_VER, OSK_C_PROTO_MINOR_VER, OSK_C_PROTO_PLATFORM_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, OskCProto.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case OSK_C_PROTO_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case OSK_C_PROTO_SEND_HK_MID:
            // DR-142: CFE_TBL_Manage();
            OSK_C_PROTO_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(OSK_C_PROTO_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */
