/*
** Purpose: Define the OpenSatKit Telemetry Output application. This app
**          receives telemetry packets from the software bus and uses its
**          packet table to determine whether packets should be sent over
**          a UDP socket.
**
** Notes:
**   1. This is non-flight code so an attempt has been made to balance keeping
**      it simple while making it robust. Limiting the number of configuration
**      parameters and integration items (message IDs, perf IDs, etc) was
**      also taken into consideration.
**   2. Performance traces are not included.
**   3. Most functions are global to assist in unit testing
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
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
#include <math.h>
#include "kit_to_app.h"
#include "cfs_utils.h"

/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void InitDataTypePkt(KIT_TO_DataTypePkt *DataTypePkt);
static void ProcessCommands(void);
static void SendHousekeepingPkt(KIT_TO_HkPkt *HkPkt);

/*
** Global Data
*/

KIT_TO_Class  KitTo;


/* Convenience macros */
#define  CMDMGR_OBJ   (&(KitTo.CmdMgr))
#define  TBLMGR_OBJ   (&(KitTo.TblMgr))
#define  PKTTBL_OBJ   (&(KitTo.PktTbl))
#define  PKTMGR_OBJ   (&(KitTo.PktMgr))
#define  EVTPLBK_OBJ  (&(KitTo.EvtPlbk))

/******************************************************************************
** Function: KIT_TO_AppMain
**
*/
void KIT_TO_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;
   uint32 StartupCnt;
   uint16 NumPktsOutput;

   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);

   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS) {
      
      CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "KIT_TO: About to call init\n");
      Status = InitApp();
   
   }

   if (Status == CFE_SUCCESS) {
      
      RunStatus = CFE_ES_APP_RUN;
   
   }
   
   /*
   ** Main process loop
   */
   
   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "KIT_TO: About to enter loop\n");
   StartupCnt = 0;
   while (CFE_ES_RunLoop(&RunStatus)) {
   
      /* Use a short delay during startup to avoid event message pipe overflow */
      if (StartupCnt < 200) { 
         OS_TaskDelay(20);
         ++StartupCnt;
      }
      else {
         OS_TaskDelay(KitTo.RunLoopDelay);
      }

      NumPktsOutput = PKTMGR_OutputTelemetry();
      
      CFE_EVS_SendEvent(KIT_TO_DEMO_EID, CFE_EVS_DEBUG, "Output %d telemetry packets", NumPktsOutput);

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("KIT_TO App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(KIT_TO_APP_EXIT_EID, CFE_EVS_CRITICAL, "KIT_TO App: terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of KIT_TO_AppMain() */


/******************************************************************************
** Function: KIT_TO_NoOpCmd
**
*/

boolean KIT_TO_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (KIT_TO_APP_NOOP_EID, CFE_EVS_INFORMATION,
                      "Kit Telemetry Output (KIT_TO) version %d.%d.%d received a no operation command",
                      KIT_TO_MAJOR_VER,KIT_TO_MINOR_VER,KIT_TO_PLATFORM_REV);

   return TRUE;


} /* End KIT_TO_NoOpCmd() */


/******************************************************************************
** Function: KIT_TO_ResetAppCmd
**
*/

boolean KIT_TO_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);

   PKTMGR_ResetStatus();
   EVT_PLBK_ResetStatus();
   
   return TRUE;

} /* End KIT_TO_ResetAppCmd() */


/******************************************************************************
** Function: KIT_TO_SendDataTypeTlmCmd
**
*/

boolean KIT_TO_SendDataTypeTlmCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   int32 Status;

   CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &KitTo.DataTypePkt);
   Status = CFE_SB_SendMsg((CFE_SB_Msg_t *)&KitTo.DataTypePkt);

   return (Status == CFE_SUCCESS);

} /* End KIT_TO_SendDataTypeTlmCmd() */


/******************************************************************************
** Function: KIT_TO_SetRunLoopDelayCmd
**
*/
boolean KIT_TO_SetRunLoopDelayCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const KIT_TO_SetRunLoopDelayCmdMsg *CmdMsg = (const KIT_TO_SetRunLoopDelayCmdMsg *) MsgPtr;
   KIT_TO_Class *KitToPtr = (KIT_TO_Class *)ObjDataPtr;
   boolean RetStatus = FALSE;
   
   if ((CmdMsg->RunLoopDelay >= KIT_TO_MIN_RUN_LOOP_DELAY_MS) &&
       (CmdMsg->RunLoopDelay <= KIT_TO_MAX_RUN_LOOP_DELAY_MS)) {
   
      CFE_EVS_SendEvent(KIT_TO_SET_RUN_LOOP_DELAY_EID, CFE_EVS_INFORMATION,
                        "Run loop delay changed from %d to %d", 
                        KitToPtr->RunLoopDelay, CmdMsg->RunLoopDelay);
   
      KitToPtr->RunLoopDelay = CmdMsg->RunLoopDelay;
      
      PKTMGR_InitStats(KitToPtr->RunLoopDelay,PKTMGR_STATS_RECONFIG_INIT_MS);

      RetStatus = TRUE;
   
   }   
   else {
      
      CFE_EVS_SendEvent(KIT_TO_INVALID_RUN_LOOP_DELAY_EID, CFE_EVS_ERROR,
                        "Invalid commanded run loop delay of %d ms. Valid inclusive range: [%d,%d] ms", 
                        CmdMsg->RunLoopDelay,KIT_TO_MIN_RUN_LOOP_DELAY_MS,KIT_TO_MAX_RUN_LOOP_DELAY_MS);
      
   }
   
   return RetStatus;
   
} /* End KIT_TO_SetRunLoopDelayCmd() */


/******************************************************************************
** Function: KIT_TO_TestFilterCmd
**
*/
boolean KIT_TO_TestFilterCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const KIT_TO_TestFilterCmdMsg *CmdPtr = (const KIT_TO_TestFilterCmdMsg *) MsgPtr;
      
   uint16 SeqCnt;
   uint32 Seconds, Subseconds;
   uint32 SubSecDelta;
   char   FormatStr[132];
   CCSDS_TelemetryPacket_t TestPkt;
   CFE_TIME_SysTime_t      PktTime;
   PktUtil_Filter Filter;
 
   
   Filter.Type  = PKTUTIL_FILTER_BY_SEQ_CNT;
   Filter.Param = CmdPtr->FilterParam;

   CFE_EVS_SendEvent(KIT_TO_TEST_FILTER_EID, CFE_EVS_INFORMATION,
                     "Filter by sequence counter: N=%d, X=%d, O=%d",
                     Filter.Param.N, Filter.Param.X, Filter.Param.O);

   for (SeqCnt=0; SeqCnt < 20; SeqCnt++) {
      CCSDS_WR_SEQ(TestPkt.SpacePacket.Hdr, SeqCnt);
      CFE_EVS_SendEvent(KIT_TO_TEST_FILTER_EID, CFE_EVS_INFORMATION,
                        ">>>SeqCnt=%2d: Filtered=%d\n", 
                        SeqCnt, PktUtil_IsPacketFiltered((const CFE_SB_MsgPtr_t)&TestPkt, &Filter));
   }


   Filter.Type  = PKTUTIL_FILTER_BY_TIME;

   CFE_EVS_SendEvent(KIT_TO_TEST_FILTER_EID, CFE_EVS_INFORMATION,
                     "Filter by time: N=%d, X=%d, O=%d. CCSDS_TIME_SIZE=%d bytes",
                     Filter.Param.N, Filter.Param.X, Filter.Param.O, CCSDS_TIME_SIZE); 

   if (CCSDS_TIME_SIZE == 6) {
      SubSecDelta = 0x0100;
      strcpy(FormatStr,">>>Time=0x%08X:%06X OSK Filtered=%d, cFS Filtered=%d\n");
      CFE_SB_InitMsg(&TestPkt, KIT_TO_HK_TLM_MID, 12, TRUE);
   }
   else {
      SubSecDelta = 0x01000000;
      strcpy(FormatStr,">>>Time=0x%08X:%08X OSK Filtered=%d, cFS Filtered=%d\n");
      CFE_SB_InitMsg(&TestPkt, KIT_TO_HK_TLM_MID, 14, TRUE);
   }
   
   Seconds = 0;
   Subseconds = 0;
   for (SeqCnt=0; SeqCnt < 20; SeqCnt++) {
      CCSDS_WR_SEC_HDR_SEC(TestPkt.Sec,Seconds);
      CCSDS_WR_SEC_HDR_SUBSEC(TestPkt.Sec, Subseconds);
      PktTime = CFE_SB_GetMsgTime((CFE_SB_MsgPtr_t)&TestPkt);
      CFE_EVS_SendEvent(KIT_TO_TEST_FILTER_EID, CFE_EVS_INFORMATION, FormatStr,
                        PktTime.Seconds, PktTime.Subseconds, 
                        PktUtil_IsPacketFiltered((const CFE_SB_MsgPtr_t)&TestPkt, &Filter),
                        CFS_IsPacketFiltered((CFE_SB_MsgPtr_t)&TestPkt,2,Filter.Param.N,Filter.Param.X,Filter.Param.O));
      
      Subseconds += SubSecDelta;
      ++Seconds;
   }

   return TRUE;

} /* End KIT_TO_TestFilterCmd() */


/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
static void SendHousekeepingPkt(KIT_TO_HkPkt *HkPkt)
{

   /*
   ** KIT_TO Data
   */

   HkPkt->ValidCmdCnt   = KitTo.CmdMgr.ValidCmdCnt;
   HkPkt->InvalidCmdCnt = KitTo.CmdMgr.InvalidCmdCnt;

   HkPkt->RunLoopDelay  = KitTo.RunLoopDelay;

   /*
   ** TBLMGR Data
   */

   HkPkt->PktTblLastLoadStatus  = KitTo.PktTbl.LastLoadStatus;
   HkPkt->PktTblAttrErrCnt      = KitTo.PktTbl.AttrErrCnt;

   /*
   ** PKTMGR Data
   ** - At a minimum all pktmgr variables effected by a reset must be included
   ** - Some of these may be more diagnostic but not enough to warrant a
   **   separate diagnostic. Also easier for the user not to have to command it.
   */

   HkPkt->StatsValid  = (KitTo.PktMgr.Stats.State == PKTMGR_STATS_VALID);
   HkPkt->PktsPerSec  = round(KitTo.PktMgr.Stats.AvgPktsPerSec);
   HkPkt->BytesPerSec = round(KitTo.PktMgr.Stats.AvgBytesPerSec);

   HkPkt->TlmSockId = (uint16)KitTo.PktMgr.TlmSockId;
   strncpy(HkPkt->TlmDestIp, KitTo.PktMgr.TlmDestIp, PKTMGR_IP_STR_LEN);

   HkPkt->EvtPlbkEna      = KitTo.EvtPlbk.Enabled;
   HkPkt->EvtPlbkHkPeriod = (uint8)KitTo.EvtPlbk.HkCyclePeriod;
   
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) HkPkt);

} /* End SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
   int32 Status = CFE_SUCCESS;

   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "KIT_TO_InitApp() Entry\n");

   KitTo.RunLoopDelay = KIT_TO_RUN_LOOP_DELAY_MS;

   /*
   ** Initialize 'entity' objects
   */

   PKTTBL_Constructor(PKTTBL_OBJ, PKTMGR_GetTblPtr, PKTMGR_LoadTbl, PKTMGR_LoadTblEntry);
   PKTMGR_Constructor(PKTMGR_OBJ, PKTMGR_PIPE_NAME, PKTMGR_PIPE_DEPTH);

   EVT_PLBK_Constructor(EVTPLBK_OBJ, KIT_TO_EVS_LOG_TLM_MID, 
                        EVT_PLBK_HK_CYCLE_PERIOD, EVT_PLBK_EVT_LOG_FILENAME,
                        CFE_EVS_CMD_MID);
      
   /*
   ** Initialize application managers
   */

   CFE_SB_CreatePipe(&KitTo.CmdPipe, KIT_TO_CMD_PIPE_DEPTH, KIT_TO_CMD_PIPE_NAME);
   CFE_SB_Subscribe(KIT_TO_CMD_MID, KitTo.CmdPipe);
   CFE_SB_Subscribe(KIT_TO_SEND_HK_MID, KitTo.CmdPipe);

   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "KIT_TO_InitApp() Before CMDMGR calls\n");
   CMDMGR_Constructor(CMDMGR_OBJ);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,            NULL,       KIT_TO_NoOpCmd,            0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,           NULL,       KIT_TO_ResetAppCmd,        0);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_PKT_TBL_LOAD_CMD_FC,    TBLMGR_OBJ, TBLMGR_LoadTblCmd,         TBLMGR_LOAD_TBL_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_PKT_TBL_DUMP_CMD_FC,    TBLMGR_OBJ, TBLMGR_DumpTblCmd,         TBLMGR_DUMP_TBL_CMD_DATA_LEN);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_ADD_PKT_CMD_FC,          PKTMGR_OBJ, PKTMGR_AddPktCmd,          PKKTMGR_ADD_PKT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_ENABLE_OUTPUT_CMD_FC,    PKTMGR_OBJ, PKTMGR_EnableOutputCmd,    PKKTMGR_ENABLE_OUTPUT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_REMOVE_ALL_PKTS_CMD_FC,  PKTMGR_OBJ, PKTMGR_RemoveAllPktsCmd,   0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_REMOVE_PKT_CMD_FC,       PKTMGR_OBJ, PKTMGR_RemovePktCmd,       PKKTMGR_REMOVE_PKT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_SEND_PKT_TBL_TLM_CMD_FC, PKTMGR_OBJ, PKTMGR_SendPktTblTlmCmd,   PKKTMGR_SEND_PKT_TBL_TLM_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_UPDATE_FILTER_CMD_FC,    PKTMGR_OBJ, PKTMGR_UpdateFilterCmd,    PKKTMGR_UPDATE_FILTER_CMD_DATA_LEN);
   
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_SEND_DATA_TYPES_CMD_FC,    &KitTo, KIT_TO_SendDataTypeTlmCmd, 0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_SET_RUN_LOOP_DELAY_CMD_FC, &KitTo, KIT_TO_SetRunLoopDelayCmd, KIT_TO_SET_RUN_LOOP_DELAY_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_TEST_FILTER_CMD_FC,        &KitTo, KIT_TO_TestFilterCmd,      KIT_TO_TEST_FILTER_CMD_DATA_LEN);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_EVT_PLBK_CONFIG_CMD_FC,  EVTPLBK_OBJ, EVT_PLBK_ConfigCmd, EVT_PLBK_CONFIG_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_EVT_PLBK_START_CMD_FC,   EVTPLBK_OBJ, EVT_PLBK_StartCmd,  EVT_PLBK_START_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_EVT_PLBK_STOP_CMD_FC,    EVTPLBK_OBJ, EVT_PLBK_StopCmd,   EVT_PLBK_STOP_CMD_DATA_LEN);

   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "KIT_TO_InitApp() Before TBLMGR calls\n");
   TBLMGR_Constructor(TBLMGR_OBJ);
   TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, PKTTBL_LoadCmd, PKTTBL_DumpCmd, KIT_TO_DEF_PKTTBL_FILENAME);

   CFE_SB_InitMsg(&KitTo.HkPkt, KIT_TO_HK_TLM_MID, KIT_TO_TLM_HK_LEN, TRUE);
   InitDataTypePkt(&KitTo.DataTypePkt);

   /*
   ** Application startup event message
   */

   Status = CFE_EVS_SendEvent(KIT_TO_APP_INIT_EID, CFE_EVS_INFORMATION,
                              "KIT_TO Initialized. Version %d.%d.%d",
                              KIT_TO_MAJOR_VER, KIT_TO_MINOR_VER, KIT_TO_PLATFORM_REV);

   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: InitDataTypePkt
**
*/
static void InitDataTypePkt(KIT_TO_DataTypePkt *DataTypePkt)
{

   int16  i;
   char   StringVariable[10] = "ABCDEFGHIJ";

   CFE_SB_InitMsg(DataTypePkt, KIT_TO_DATA_TYPE_TLM_MID, KIT_TO_TLM_DATA_TYPE_LEN, TRUE);

   DataTypePkt->synch = 0x6969;
   #if 0
      DataTypePkt->bit1    = 1;
      DataTypePkt->bit2    = 0;
      DataTypePkt->bit34   = 2;
      DataTypePkt->bit56   = 3;
      DataTypePkt->bit78   = 1;
      DataTypePkt->nibble1 = 0xA;
      DataTypePkt->nibble2 = 0x4;
   #endif
      DataTypePkt->bl1 = FALSE;
      DataTypePkt->bl2 = TRUE;
      DataTypePkt->b1  = 16;
      DataTypePkt->b2  = 127;
      DataTypePkt->b3  = 0x7F;
      DataTypePkt->b4  = 0x45;
      DataTypePkt->w1  = 0x2468;
      DataTypePkt->w2  = 0x7FFF;
      DataTypePkt->dw1 = 0x12345678;
      DataTypePkt->dw2 = 0x87654321;
      DataTypePkt->f1  = 90.01;
      DataTypePkt->f2  = .0000045;
      DataTypePkt->df1 = 99.9;
      DataTypePkt->df2 = .4444;

   for (i=0; i < 10; i++) DataTypePkt->str[i] = StringVariable[i];

} /* End InitDataTypePkt() */

/******************************************************************************
** Function: ProcessCommands
**
*/
static void ProcessCommands(void)
{

   int32           Status;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, KitTo.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case KIT_TO_CMD_MID:
            CFE_EVS_SendEvent(KIT_TO_DEMO_EID, CFE_EVS_DEBUG, "Processing command function code %d", CFE_SB_GetCmdCode(CmdMsgPtr));
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case KIT_TO_SEND_HK_MID:
            CFE_EVS_SendEvent(KIT_TO_DEMO_EID, CFE_EVS_DEBUG, "Sending housekeeping packet");
            EVT_PLBK_Execute();
            SendHousekeepingPkt(&KitTo.HkPkt);
            break;

         default:
            CFE_EVS_SendEvent(KIT_TO_APP_INVALID_MID_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


