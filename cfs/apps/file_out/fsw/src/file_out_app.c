/* 
** Purpose: File Output application
**
** TODO
**   1. Add performance monitors
**   2. Verify applying the running CRC is equal to a one-time end CRC 
**   3. Add event filters
**   4. Can there be more than one OS_TaskInstallDeleteHandler?
**   5. Put limit on failed data segment attempts? 
**   6. Replace FOTP loop with telemetry output flow control
**   7. Verify Fotp->NextDataSegmentId managed correctly
**   8. The SendFileTransferTlm() function is inconsistent with whether
**      the caller loads the telemetry packet prior to calling the function. 
**
** Notes:
**   1. This app manages the File Output Transfer Protocol (FOTP) which is
**      a custom protocol that is similar to the CCSDS File Deliver
**      Protocol (CFDP) Class 1. See fotp.h for protocol details. 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide and the
**      osk_c_demo app that illustrates best practices with comments.  
**   2. cFS Application Developer's Guide.
**
**   Written by David McComas, licensed under the Apache License, Version 2.0
**   (the "License"); you may not use this file except in compliance with the
**   License. You may obtain a copy of the License at
**
**      http://www.apache.org/licenses/LICENSE-2.0
**
**   Unless required by applicable law or agreed to in writing, software
**   distributed under the License is distributed on an "AS IS" BASIS,
**   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**   See the License for the specific language governing permissions and
**   limitations under the License.
*/
/*
** Includes
*/

#include <string.h>
#include "file_out_app.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ  (&(FileOut.IniTbl))
#define  CMDMGR_OBJ  (&(FileOut.CmdMgr))  
#define  FOTP_OBJ    (&(FileOut.Fotp))


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(void);
static void ProcessCommands(void);
static void SendHousekeepingPkt(void);

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum_t IniCfgEnum"
*/
DEFINE_ENUM(Config,APP_CONFIG)


/**********************/
/** File Global Data **/
/**********************/

static CFE_EVS_BinFilter_t  EventFilters[] =
{  
   /* Event ID                   , Mask                 */
   {FOTP_SEND_FILE_TRANSFER_ERR_EID, CFE_EVS_FIRST_TWO_STOP}
};


/*****************/
/** Global Data **/
/*****************/

FILE_OUT_Class_t  FileOut;


/******************************************************************************
** Function: FILE_OUT_AppMain
**
*/
void FILE_OUT_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;

   Status = CFE_ES_RegisterApp();

   if (Status == CFE_SUCCESS)
   {  
      Status = InitApp();
   }

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus)) {
      
      ProcessCommands();
      
   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("FILE_OUT App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(FILE_OUT_APP_EXIT_EID, CFE_EVS_CRITICAL, "FILE_OUT App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of FILE_OUT_AppMain() */


/******************************************************************************
** Function: FILE_OUT_APP_NoOpCmd
**
*/

boolean FILE_OUT_APP_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (FILE_OUT_APP_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for FILE_OUT App version %d.%d.%d",
                      FILE_OUT_MAJOR_VER, FILE_OUT_MINOR_VER, FILE_OUT_PLATFORM_REV);

   return TRUE;


} /* End FILE_OUT_APP_NoOpCmd() */


/******************************************************************************
** Function: FILE_OUT_APP_ResetAppCmd
**
*/

boolean FILE_OUT_APP_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_ResetFilter (FOTP_SEND_FILE_TRANSFER_ERR_EID);
   
   FOTP_ResetStatus();
   CMDMGR_ResetStatus(CMDMGR_OBJ);

   return TRUE;

} /* End FILE_OUT_APP_ResetAppCmd() */


/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
static void SendHousekeepingPkt(void)
{

   /*
   ** FILE_OUT Application Data
   */

   FileOut.HkPkt.ValidCmdCnt   = FileOut.CmdMgr.ValidCmdCnt;
   FileOut.HkPkt.InvalidCmdCnt = FileOut.CmdMgr.InvalidCmdCnt;

   /*
   ** FOTP Data
   ** - At a minimum all FileIn variables effected by a reset must be included
   */
   
   FileOut.HkPkt.FileTransferCnt     = FileOut.Fotp.FileTransferCnt;             
   FileOut.HkPkt.FileTransferState   = FileOut.Fotp.FileTransferState; 
   FileOut.HkPkt.PausedTransferState = FileOut.Fotp.PausedFileTransferState;
   FileOut.HkPkt.PrevSegmentFailed   = FileOut.Fotp.PrevSendDataSegmentFailed;
   
   FileOut.HkPkt.FileTranferByteCnt  = FileOut.Fotp.FileTransferByteCnt;
   FileOut.HkPkt.FileRunningCrc      = FileOut.Fotp.FileRunningCrc;
   
   FileOut.HkPkt.DataTransferLen     = FileOut.Fotp.DataTransferLen;
   FileOut.HkPkt.FileLen             = FileOut.Fotp.FileLen;
   FileOut.HkPkt.FileByteOffset      = FileOut.Fotp.FileByteOffset;
   FileOut.HkPkt.DataSegmentLen      = FileOut.Fotp.DataSegmentLen;
   FileOut.HkPkt.DataSegmentOffset   = FileOut.Fotp.DataSegmentOffset;
   FileOut.HkPkt.NextDataSegmentId   = FileOut.Fotp.NextDataSegmentId;

   strncpy(FileOut.HkPkt.SrcFilename, FileOut.Fotp.SrcFilename, FOTP_FILENAME_LEN);

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FileOut.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &FileOut.HkPkt);

} /* End SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = CFE_SEVERITY_ERROR;

   CFE_PSP_MemSet((void*)&FileOut, 0, sizeof(FILE_OUT_Class_t));
   
   Status = CFE_EVS_Register(EventFilters,sizeof(EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                             CFE_EVS_BINARY_FILTER);

   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("Error registering for EVS services, status = 0x%08X", Status);
   }

   /*
   ** Initialize contained objects
   */
   
   if (INITBL_Constructor(INITBL_OBJ, FILE_OUT_INI_FILENAME, &IniCfgEnum))
   {
      
      FileOut.CmdMidValue     = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_CMD_MID);
      FileOut.SendHkMidValue  = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_SEND_HK_MID);
      FileOut.ExecuteMidValue = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_EXECUTE_MID);

      FOTP_Constructor(FOTP_OBJ, INITBL_OBJ);
      
      Status = CFE_SUCCESS;
   
   } /* End if INITBL Constructed */

   if (Status == CFE_SUCCESS)
   {

      Status = CFE_SB_CreatePipe(&FileOut.CmdPipe,
                                 INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_DEPTH),
                                 INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME)); 

      if (Status == CFE_SUCCESS) 
      {

         Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(FileOut.CmdMidValue), FileOut.CmdPipe);
         if (Status == CFE_SUCCESS) 
         {
            Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(FileOut.SendHkMidValue), FileOut.CmdPipe);
            if (Status == CFE_SUCCESS) 
            {
               Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(FileOut.ExecuteMidValue), FileOut.CmdPipe);
               if (Status != CFE_SUCCESS) 
               {
                  CFE_EVS_SendEvent(FILE_OUT_APP_INIT_ERR_EID, CFE_EVS_ERROR,
                                    "Error subscribing to Execute MID value %d on command pipe %s. SB Status = 0x%08X",
                                    CFE_SB_ValueToMsgId(FileOut.SendHkMidValue),
                                    INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME), Status);
               }
            }
            else 
            {
               CFE_EVS_SendEvent(FILE_OUT_APP_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Error subscribing to Send HK MID value %d on command pipe %s failed. SB Status = 0x%08X",
                                 CFE_SB_ValueToMsgId(FileOut.SendHkMidValue),
                                 INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME), Status);
            }
         }
         else
         {
            CFE_EVS_SendEvent(FILE_OUT_APP_INIT_ERR_EID, CFE_EVS_ERROR,
                              "Error subscribing to Command MID value %d on command pipe %s failed. SB Status = 0x%08X",
                              CFE_SB_ValueToMsgId(FileOut.CmdMidValue),
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME), Status);
         }
         
      } /* End if create pipe */
      else
      {

         CFE_EVS_SendEvent(FILE_OUT_APP_INIT_ERR_EID, CFE_EVS_ERROR,
                           "Error creating SB Command Pipe %s with depth %d. SB Status = 0x%08X",
                           INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME), 
                           INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_DEPTH), Status);
      
      } /* End if create pipe failed */

      if (Status == CFE_SUCCESS)
      {         
         CMDMGR_Constructor(CMDMGR_OBJ);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, FILE_OUT_APP_NoOpCmd,     0);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, FILE_OUT_APP_ResetAppCmd, 0);
         
         CMDMGR_RegisterFunc(CMDMGR_OBJ, FOTP_START_TRANSFER_CMD_FC,  FOTP_OBJ, FOTP_StartTransferCmd,  FOTP_START_TRANSFER_CMD_DATA_LEN);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, FOTP_CANCEL_TRANSFER_CMD_FC, FOTP_OBJ, FOTP_CancelTransferCmd, FOTP_CANCEL_TRANSFER_CMD_DATA_LEN);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, FOTP_PAUSE_TRANSFER_CMD_FC,  FOTP_OBJ, FOTP_PauseTransferCmd,  FOTP_PAUSE_TRANSFER_CMD_DATA_LEN);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, FOTP_RESUME_TRANSFER_CMD_FC, FOTP_OBJ, FOTP_ResumeTransferCmd, FOTP_RESUME_TRANSFER_CMD_DATA_LEN);

         CFE_SB_InitMsg(&FileOut.HkPkt, 
                        CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_HK_TLM_MID)),
                        FILE_OUT_TLM_HK_LEN, TRUE);

         /*
         ** Application startup event message
         */
         Status = CFE_EVS_SendEvent(FILE_OUT_APP_INIT_EID, CFE_EVS_INFORMATION,
                                    "FILE_OUT App Initialized. Version %d.%d.%d",
                                    FILE_OUT_MAJOR_VER, FILE_OUT_MINOR_VER, FILE_OUT_PLATFORM_REV);
      } /* End if CFE_SUCCESS */
      
   } /* End if init success */

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
   int             MsgId;
   
   Status = CFE_SB_RcvMsg(&CmdMsgPtr, FileOut.CmdPipe, CFE_SB_PEND_FOREVER);
        
   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_MsgIdToValue(CFE_SB_GetMsgId(CmdMsgPtr));

      if (MsgId == FileOut.CmdMidValue) 
      {
         CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
      }
      else if (MsgId == FileOut.ExecuteMidValue)
      {
         FOTP_Execute();
      }
      else if (MsgId == FileOut.SendHkMidValue)
      {
         SendHousekeepingPkt();
      }
      else
      {
         CFE_EVS_SendEvent(FILE_OUT_APP_INVALID_MID_EID, CFE_EVS_ERROR,
                           "Received invalid command packet,MID = 0x%04X",MsgId);
      }

   } /* End if SB received a packet */

} /* End ProcessCommands() */
