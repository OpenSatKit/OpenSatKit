/* 
** Purpose: Manage receiving commands over a UDP socket and sending them on the SB.
**
** Notes:
**   1. This is non-flight code so an attempt has been made to balance keeping
**      it simple while making it robust. Limiting the number of configuration
**      parameters and integration items (message IDs, perf IDs, etc) was
**      also taken into consideration.
**   2. Event message filters are not used since this is for test environments.
**      This may be reconsidered if event flooding ever becomes a problem.
**   3. Performance traces are not included.
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
#include "kit_ci_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

KIT_CI_Class  KitCi;

KIT_CI_HkPkt  KitCiHkPkt;

#define  CMDMGR_OBJ  (&(KitCi.CmdMgr))  /* Convenience macro */
#define  UPLINK_OBJ  (&(KitCi.Uplink))  /* Convenience macro */

/******************************************************************************
** Function: KIT_CI_AppMain
**
*/
void KIT_CI_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


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
   ** needed.
   */

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {


      OS_TaskDelay(KIT_CI_RUNLOOP_DELAY);

      UPLINK_Read(KIT_CI_RUNLOOP_MSG_READ);

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("KIT_CI App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(KIT_CI_APP_EXIT_EID, CFE_EVS_CRITICAL, "KIT_CI App: terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of KIT_CI_AppMain() */


/******************************************************************************
** Function: KIT_CI_NoOpCmd
**
*/

boolean KIT_CI_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (KIT_CI_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "Kit Command Ingest (KIT_CI) version %d.%d received a no operation command",
                      KIT_CI_MAJOR_VERSION,KIT_CI_MINOR_VERSION);

   return TRUE;


} /* End KIT_CI_NoOpCmd() */


/******************************************************************************
** Function: KIT_CI_ResetAppCmd
**
*/

boolean KIT_CI_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   UPLINK_ResetStatus();
   CMDMGR_ResetStatus(CMDMGR_OBJ);

   return TRUE;

} /* End KIT_CI_ResetAppCmd() */


/******************************************************************************
** Function: KIT_CI_SendHousekeepingPkt
**
*/
void KIT_CI_SendHousekeepingPkt(void)
{

   /*
   ** Application Data
   */

   KitCiHkPkt.ValidCmdCnt   = KitCi.CmdMgr.ValidCmdCnt;
   KitCiHkPkt.InvalidCmdCnt = KitCi.CmdMgr.InvalidCmdCnt;

   /*
   ** Uplink Data
   */

   KitCiHkPkt.SocketConnected      = KitCi.Uplink.Connected;
   KitCiHkPkt.MsgTunnelEnabled     = KitCi.Uplink.MsgTunnel.Enabled;
   KitCiHkPkt.SocketId             = KitCi.Uplink.SocketId;
   KitCiHkPkt.RecvMsgCnt           = KitCi.Uplink.RecvMsgCnt;
   KitCiHkPkt.RecvMsgErrCnt        = KitCi.Uplink.RecvMsgErrCnt;

   KitCiHkPkt.MappingsPerformed    = KitCi.Uplink.MsgTunnel.MappingsPerformed;
   KitCiHkPkt.LastMapping.Index    = KitCi.Uplink.MsgTunnel.LastMapping.Index;
   KitCiHkPkt.LastMapping.OrgMsgId = KitCi.Uplink.MsgTunnel.LastMapping.OrgMsgId;
   KitCiHkPkt.LastMapping.NewMsgId = KitCi.Uplink.MsgTunnel.LastMapping.NewMsgId;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &KitCiHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &KitCiHkPkt);

} /* End KIT_CI_SendHousekeepingPkt() */


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

    UPLINK_Constructor(&KitCi.Uplink,KIT_CI_PORT);

    /*
    ** Initialize application managers
    */

    CFE_SB_CreatePipe(&KitCi.CmdPipe, CMDMGR_PIPE_DEPTH, CMDMGR_PIPE_NAME);
    CFE_SB_Subscribe(KIT_CI_CMD_MID, KitCi.CmdPipe);
    CFE_SB_Subscribe(KIT_CI_SEND_HK_MID, KitCi.CmdPipe);

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,              NULL,       KIT_CI_NoOpCmd,            0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,             NULL,       KIT_CI_ResetAppCmd,        0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_CI_CONFIG_MSG_TUNNEL_CMD_FC, UPLINK_OBJ, UPLINK_ConfigMsgTunnelCmd, UPLINK_CONFIG_MSG_TUNNEL_CMD_DATA_LEN);

    CFE_SB_InitMsg(&KitCiHkPkt, KIT_CI_HK_TLM_MID, KIT_CI_TLM_HK_LEN, TRUE);

    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(KIT_CI_INIT_APP_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "KIT_CI Initialized. Version %d.%d.%d.%d",
                               KIT_CI_MAJOR_VERSION,
                               KIT_CI_MINOR_VERSION,
                               KIT_CI_REVISION,
                               KIT_CI_MISSION_REV);

    return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
*/
static void ProcessCommands(void)
{

   int32           Status = CFE_SEVERITY_ERROR;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, KitCi.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case KIT_CI_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case KIT_CI_SEND_HK_MID:
            KIT_CI_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(KIT_CI_APP_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */





