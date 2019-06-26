/*
** Purpose: Define the OpenSat Kit Telemetry Output application. This app
**          receives telemetry packets from the software bus and uses its
**          packet table to determine whether packets should be sent over
**          a UDP socket.
**
** Notes:
**   1. This is non-flight code so an attempt has been made to balance keeping
**      it simple while making it robust. Limiting the number of configuration
**      parameters and integration items (message IDs, perf IDs, etc) was
**      also taken into consideration.
**   2. Event message filters are not used since this is for test environments.
**      This may be reconsidered if event flooding ever becomes a problem.
**   3. Performance traces are not included.
**   4. Most functions are global to assist in unit testing
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
#include "kit_to_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void InitDataTypePkt(void);
static void ProcessCommands(void);

/*
** Global Data
*/

KIT_TO_Class  KitTo;

KIT_TO_HkPkt        KitToHkPkt;
KIT_TO_DataTypePkt  ToDataTypePkt;

#define  CMDMGR_OBJ  (&(KitTo.CmdMgr))  /* Convenience macro */
#define  TBLMGR_OBJ  (&(KitTo.TblMgr))  /* Convenience macro */
#define  PKTTBL_OBJ  (&(KitTo.PktTbl))  /* Convenience macro */
#define  PKTMGR_OBJ  (&(KitTo.PktMgr))  /* Convenience macro */

/******************************************************************************
** Function: KIT_TO_AppMain
**
*/
void KIT_TO_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);

   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS) {
      
      CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "KIT_TO: About to call init\n");
      Status = InitApp();
   
   }

   /*
   ** At this point many flight apps use CFE_ES_WaitForStartupSync() to
   ** synchronize their startup timing with other apps. This is not
   ** needed.
   */

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   
   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "KIT_TO: About to enter loop\n");
   while (CFE_ES_RunLoop(&RunStatus))
   {

      OS_TaskDelay(KIT_TO_RUNLOOP_DELAY);

      PKTMGR_OutputTelemetry();

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("KIT_TO App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(KIT_TO_EXIT_ERR_EID, CFE_EVS_CRITICAL, "KIT_TO App: terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of KIT_TO_AppMain() */


/******************************************************************************
** Function: KIT_TO_NoOpCmd
**
*/

boolean KIT_TO_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (KIT_TO_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "Kit Telemetry Output (KIT_TO) version %d.%d received a no operation command",
                      KIT_TO_MAJOR_VERSION,KIT_TO_MINOR_VERSION);

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

   return TRUE;

} /* End KIT_TO_ResetAppCmd() */


/******************************************************************************
** Function: KIT_TO_SendDataTypeTlmCmd
**
*/

boolean KIT_TO_SendDataTypeTlmCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   int32 Status;

   CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &ToDataTypePkt);
   Status = CFE_SB_SendMsg((CFE_SB_Msg_t *)&ToDataTypePkt);

   return (Status == CFE_SUCCESS);

} /* End KIT_TO_SendDataTypeTlmCmd() */

/******************************************************************************
** Function: KIT_TO_SendHousekeepingPkt
**
*/
void KIT_TO_SendHousekeepingPkt(void)
{

   /*
   ** KIT_TO Data
   */

   KitToHkPkt.ValidCmdCnt   = KitTo.CmdMgr.ValidCmdCnt;
   KitToHkPkt.InvalidCmdCnt = KitTo.CmdMgr.InvalidCmdCnt;

   /*
   ** TBLMGR Data
   */

   KitToHkPkt.PktTblLastLoadStatus  = KitTo.PktTbl.LastLoadStatus;
   KitToHkPkt.PktTblAttrErrCnt      = KitTo.PktTbl.AttrErrCnt;

   /*
   ** PKTMGR Data
   ** - At a minimum all pktmgr variables effected by a reset must be included
   ** - Some of these may be more diagnostic but not enough to warrant a
   **   separate diagnostic. Also easier for the user not to hhave to command it.
   */

   KitToHkPkt.TlmSockId = (uint16)KitTo.PktMgr.TlmSockId;
   strncpy(KitToHkPkt.TlmDestIp, KitTo.PktMgr.TlmDestIp, PKTMGR_IP_STR_LEN);

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &KitToHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &KitToHkPkt);

} /* End KIT_TO_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
   int32 Status = CFE_SUCCESS;

   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "KIT_TO_InitApp() Entry\n");

   /*
   ** Initialize 'entity' objects
   */

   PKTTBL_Constructor(PKTTBL_OBJ, PKTMGR_GetTblPtr, PKTMGR_LoadTbl, PKTMGR_LoadTblEntry);
   PKTMGR_Constructor(PKTMGR_OBJ, PKTMGR_PIPE_NAME, PKTMGR_PIPE_DEPTH);

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

   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_ADD_PKT_CMD_FC,         PKTMGR_OBJ, PKTMGR_AddPktCmd,          PKKTMGR_ADD_PKT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_REMOVE_PKT_CMD_FC,      PKTMGR_OBJ, PKTMGR_RemovePktCmd,       PKKTMGR_REMOVE_PKT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_REMOVE_ALL_PKTS_CMD_FC, PKTMGR_OBJ, PKTMGR_RemoveAllPktsCmd,   0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_ENABLE_OUTPUT_CMD_FC,   PKTMGR_OBJ, PKTMGR_EnableOutputCmd,    PKKTMGR_ENABLE_OUTPUT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_TO_SEND_DATA_TYPES_CMD_FC, &KitTo,     KIT_TO_SendDataTypeTlmCmd, 0);

   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "KIT_TO_InitApp() Before TBLMGR calls\n");
   TBLMGR_Constructor(TBLMGR_OBJ);
   TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, PKTTBL_LoadCmd, PKTTBL_DumpCmd, KIT_TO_DEF_PKTTBL_FILE_NAME);

   CFE_SB_InitMsg(&KitToHkPkt, KIT_TO_HK_TLM_MID, KIT_TO_TLM_HK_LEN, TRUE);
   InitDataTypePkt();

   /*
   ** Application startup event message
   */

   Status = CFE_EVS_SendEvent(KIT_TO_INIT_APP_INFO_EID,
                              CFE_EVS_INFORMATION,
                              "KIT_TO Initialized. Version %d.%d.%d.%d",
                              KIT_TO_MAJOR_VERSION,
                              KIT_TO_MINOR_VERSION,
                              KIT_TO_REVISION,
                              KIT_TO_MISSION_REV);

   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: InitDataTypePkt
**
*/
static void InitDataTypePkt(void)
{

   int16  i;
   char   StringVariable[10] = "ABCDEFGHIJ";

   CFE_SB_InitMsg(&ToDataTypePkt, KIT_TO_DATA_TYPE_TLM_MID, KIT_TO_TLM_DATA_TYPE_LEN, TRUE);

   ToDataTypePkt.synch = 0x6969;
   #if 0
      ToDataTypePkt.bit1    = 1;
      ToDataTypePkt.bit2    = 0;
      ToDataTypePkt.bit34   = 2;
      ToDataTypePkt.bit56   = 3;
      ToDataTypePkt.bit78   = 1;
      ToDataTypePkt.nibble1 = 0xA;
      ToDataTypePkt.nibble2 = 0x4;
   #endif
      ToDataTypePkt.bl1 = FALSE;
      ToDataTypePkt.bl2 = TRUE;
      ToDataTypePkt.b1  = 16;
      ToDataTypePkt.b2  = 127;
      ToDataTypePkt.b3  = 0x7F;
      ToDataTypePkt.b4  = 0x45;
      ToDataTypePkt.w1  = 0x2468;
      ToDataTypePkt.w2  = 0x7FFF;
      ToDataTypePkt.dw1 = 0x12345678;
      ToDataTypePkt.dw2 = 0x87654321;
      ToDataTypePkt.f1  = 90.01;
      ToDataTypePkt.f2  = .0000045;
      ToDataTypePkt.df1 = 99.9;
      ToDataTypePkt.df2 = .4444;

   for (i=0; i < 10; i++) ToDataTypePkt.str[i] = StringVariable[i];

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
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case KIT_TO_SEND_HK_MID:
            KIT_TO_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(KIT_TO_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */
