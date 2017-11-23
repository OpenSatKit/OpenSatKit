/* 
** Purpose: cFS local file transfer application based on Trivial File Transfer Protocol (TFTP).
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
#include "tftp_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

TFTP_APP_Class  TftpApp;

TFTP_APP_HkPkt  TftpAppHkPkt;

/* Convenience macros */
#define  CMDMGR_OBJ  (&(TftpApp.CmdMgr))  
#define  TFTP_OBJ    (&(TftpApp.Tftp))
#define  NETIF_OBJ   (&(TftpApp.Tftp.NetIf))

/******************************************************************************
** Function: TFTP_AppMain
**
*/
void TFTP_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;
   uint16 HkRunLoopCtr = 0;

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

   if (Status == CFE_SUCCESS) 
      RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
    while (CFE_ES_RunLoop(&RunStatus))
   {

      OS_TaskDelay(TFTP_APP_RUNLOOP_DELAY);

      TFTP_Read(TFTP_APP_RUNLOOP_PKTS);
      ProcessCommands();
      
      if (++HkRunLoopCtr >= TFTP_APP_HK_RUNLOOP_PERIOD)
      {
         TFTP_APP_SendHousekeepingPkt();
         HkRunLoopCtr = 0;
      }
       

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("TFTP App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(TFTP_APP_EXIT_ERR_EID, CFE_EVS_CRITICAL, "TTFP App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of TFTP_AppMain() */


/******************************************************************************
** Function: TFTP_APP_NoOpCmd
**
*/

boolean TFTP_APP_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (TFTP_APP_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for TFTP App version %d.%d",
                      TFTP_APP_MAJOR_VERSION,TFTP_APP_MINOR_VERSION);

   return TRUE;


} /* End TFTP_APP_NoOpCmd() */


/******************************************************************************
** Function: TFTP_APP_ResetAppCmd
**
*/

boolean TFTP_APP_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   TFTP_ResetStatus();
   CMDMGR_ResetStatus(CMDMGR_OBJ);

   return TRUE;

} /* End TFTP_APP_ResetAppCmd() */


/******************************************************************************
** Function: TFTP_APP_SendHousekeepingPkt
**
*/
void TFTP_APP_SendHousekeepingPkt(void)
{

   /*
   ** TFTP Application Data
   */

   TftpAppHkPkt.ValidCmdCnt   = TftpApp.CmdMgr.ValidCmdCnt;
   TftpAppHkPkt.InvalidCmdCnt = TftpApp.CmdMgr.InvalidCmdCnt;

   /*
   ** TFTP Data
   ** - At a minimum all TFTP variables effected by a reset must be included
   ** - Some of these may be more diagnostic but not enough to warrant a
   **   separate diagnostic packet. Also easier for the user not to have to command it.
   */

   TftpAppHkPkt.GetFileCnt = TftpApp.Tftp.GetFileCnt;
   TftpAppHkPkt.PutFileCnt = TftpApp.Tftp.PutFileCnt;

   TftpAppHkPkt.State      = TftpApp.Tftp.State;
   TftpAppHkPkt.BlockNum   = TftpApp.Tftp.BlockNum;

   strncpy(TftpAppHkPkt.SrcFileName,  TftpApp.Tftp.SrcFileName,  TFTP_FILE_NAME_LEN);
   strncpy(TftpAppHkPkt.DestFileName, TftpApp.Tftp.DestFileName, TFTP_FILE_NAME_LEN);

   /*
   ** Connection Data
   */

   TftpAppHkPkt.RecvMsgCnt      = TftpApp.Tftp.MsgCnt;
   TftpAppHkPkt.RecvMsgErrCnt   = TftpApp.Tftp.MsgErrCnt;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &TftpAppHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &TftpAppHkPkt);

} /* End TFTP_APP_SendHousekeepingPkt() */


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

    TFTP_Constructor(&TftpApp.Tftp);

    /*
    ** Initialize application managers
    */

    CFE_SB_CreatePipe(&TftpApp.CmdPipe, TFTP_CMD_PIPE_DEPTH, TFTP_CMD_PIPE_NAME);
    CFE_SB_Subscribe(TFTP_CMD_MID, TftpApp.CmdPipe);
    CFE_SB_Subscribe(TFTP_SEND_HK_MID, TftpApp.CmdPipe);

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, TFTP_APP_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, TFTP_APP_ResetAppCmd, 0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, TFTP_GET_FILE_CMD_FC, TFTP_OBJ, TFTP_GetFileCmd, TFTP_GET_FILE_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, TFTP_PUT_FILE_CMD_FC, TFTP_OBJ, TFTP_PutFileCmd, TFTP_PUT_FILE_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, NETIF_INIT_SOCKET_CMD_FC, NETIF_OBJ, NETIF_InitSocketCmd, NETIF_INIT_SOCKET_CMD_DATA_LEN);

    CFE_SB_InitMsg(&TftpAppHkPkt, TFTP_HK_TLM_MID, TFTP_APP_TLM_HK_LEN, TRUE);

    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(TFTP_APP_INIT_APP_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "TFTP App Initialized. Version %d.%d.%d.%d",
                               TFTP_APP_MAJOR_VERSION,
                               TFTP_APP_MINOR_VERSION,
                               TFTP_APP_REVISION,
                               TFTP_APP_MISSION_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, TftpApp.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case TFTP_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case TFTP_SEND_HK_MID:
            TFTP_APP_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(TFTP_APP_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */
