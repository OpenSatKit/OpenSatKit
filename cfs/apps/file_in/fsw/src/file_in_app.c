/* 
** Purpose: File Input application
**
** TODO
**   1. Add performance monitors
**   2. Verify applying the running CRC is equal to a one-time end CRC 
**   3. Add event filters
**
** Notes:
**   1. This app manages the File Input Transfer Protocol (FITP) which is
**      a custom protocol that is similar to the CCSDS File Deliver
**      Protocol (CFDP) Class 1. See fitp.h for protocol details. 
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
#include "file_in_app.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ  (&(FileIn.IniTbl))
#define  CMDMGR_OBJ  (&(FileIn.CmdMgr))  
#define  FITP_OBJ    (&(FileIn.Fitp))


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
   {FITP_DATA_SEGMENT_CMD_ERR_EID, CFE_EVS_FIRST_TWO_STOP}
};

/*****************/
/** Global Data **/
/*****************/

FILE_IN_Class_t  FileIn;


/******************************************************************************
** Function: FILE_IN_AppMain
**
*/
void FILE_IN_AppMain(void)
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

   CFE_ES_WriteToSysLog("FILE_IN App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(FILE_IN_APP_EXIT_EID, CFE_EVS_CRITICAL, "FILE_IN App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of FILE_IN_AppMain() */


/******************************************************************************
** Function: FILE_IN_APP_NoOpCmd
**
*/

boolean FILE_IN_APP_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (FILE_IN_APP_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for FILE_IN App version %d.%d.%d",
                      FILE_IN_MAJOR_VER, FILE_IN_MINOR_VER, FILE_IN_PLATFORM_REV);

   return TRUE;


} /* End FILE_IN_APP_NoOpCmd() */


/******************************************************************************
** Function: FILE_IN_APP_ResetAppCmd
**
*/

boolean FILE_IN_APP_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_ResetFilter (FITP_DATA_SEGMENT_CMD_ERR_EID);
   
   FITP_ResetStatus();
   CMDMGR_ResetStatus(CMDMGR_OBJ);

   return TRUE;

} /* End FILE_IN_APP_ResetAppCmd() */


/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
static void SendHousekeepingPkt(void)
{

   /*
   ** FILE_IN Application Data
   */

   FileIn.HkPkt.ValidCmdCnt   = FileIn.CmdMgr.ValidCmdCnt;
   FileIn.HkPkt.InvalidCmdCnt = FileIn.CmdMgr.InvalidCmdCnt;

   /*
   ** FITP Data
   ** - At a minimum all FileIn variables effected by a reset must be included
   */
   
   FileIn.HkPkt.FileTransferCnt     = FileIn.Fitp.FileTransferCnt;
   FileIn.HkPkt.FileTransferActive  = FileIn.Fitp.FileTransferActive; 
   FileIn.HkPkt.LastDataSegmentId   = FileIn.Fitp.LastDataSegmentId;
   FileIn.HkPkt.DataSegmentErrCnt   = FileIn.Fitp.DataSegmentErrCnt;             
   FileIn.HkPkt.FileTransferByteCnt = FileIn.Fitp.FileTransferByteCnt;
   FileIn.HkPkt.FileRunningCrc      = FileIn.Fitp.FileRunningCrc;
   strncpy(FileIn.HkPkt.DestFilename, FileIn.Fitp.DestFilename, FITP_FILENAME_LEN);

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FileIn.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &FileIn.HkPkt);

} /* End SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = CFE_SEVERITY_ERROR;

   CFE_PSP_MemSet((void*)&FileIn, 0, sizeof(FILE_IN_Class_t));

   Status = CFE_EVS_Register(EventFilters,sizeof(EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                             CFE_EVS_BINARY_FILTER);

   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("Error registering for EVS services, status = 0x%08X", Status);
   }
 
   /*
   ** Initialize contained objects
   */
   
   if (INITBL_Constructor(INITBL_OBJ, FILE_IN_INI_FILENAME, &IniCfgEnum))
   {
      
      FileIn.CmdMidValue     = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_CMD_MID);
      FileIn.SendHkMidValue  = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_SEND_HK_MID);

      FITP_Constructor(FITP_OBJ);
      
      Status = CFE_SUCCESS;
      
   } /* End if INITBL Constructed */
  
   if (Status == CFE_SUCCESS)
   {

      Status = CFE_SB_CreatePipe(&FileIn.CmdPipe,
                                 INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_DEPTH),
                                 INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME)); 

      if (Status == CFE_SUCCESS) 
      {

         Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(FileIn.CmdMidValue), FileIn.CmdPipe);
         if (Status == CFE_SUCCESS) 
         {
            Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(FileIn.SendHkMidValue), FileIn.CmdPipe);
            if (Status != CFE_SUCCESS) 
            {
               CFE_EVS_SendEvent(FILE_IN_APP_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Error subscribing to Send HK MID value %d on command pipe %s failed. SB Status = 0x%08X",
                                 CFE_SB_ValueToMsgId(FileIn.SendHkMidValue),
                                 INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME), Status);
            }
         }
         else
         {
            CFE_EVS_SendEvent(FILE_IN_APP_INIT_ERR_EID, CFE_EVS_ERROR,
                              "Error subscribing to Command MID value %d on command pipe %s failed. SB Status = 0x%08X",
                              CFE_SB_ValueToMsgId(FileIn.CmdMidValue),
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME), Status);
         }
         
      } /* End if create pipe */
      else
      {

         CFE_EVS_SendEvent(FILE_IN_APP_INIT_ERR_EID, CFE_EVS_ERROR,
                           "Error creating SB Command Pipe %s with depth %d. SB Status = 0x%08X",
                           INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME), 
                           INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_DEPTH), Status);
      
      } /* End if create pipe failed */

      if (Status == CFE_SUCCESS)
      {         
         CMDMGR_Constructor(CMDMGR_OBJ);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, FILE_IN_APP_NoOpCmd,     0);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, FILE_IN_APP_ResetAppCmd, 0);
         
         CMDMGR_RegisterFunc(CMDMGR_OBJ, FITP_START_TRANSFER_CMD_FC,  FITP_OBJ, FITP_StartTransferCmd,  FITP_START_TRANSFER_CMD_DATA_LEN);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, FITP_DATA_SEGMENT_CMD_FC,    FITP_OBJ, FITP_DataSegmentCmd,    FITP_DATA_SEGMENT_CMD_DATA_LEN);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, FITP_FINISH_TRANSFER_CMD_FC, FITP_OBJ, FITP_FinishTransferCmd, FITP_FINISH_TRANSFER_CMD_DATA_LEN);
         CMDMGR_RegisterFunc(CMDMGR_OBJ, FITP_CANCEL_TRANSFER_CMD_FC, FITP_OBJ, FITP_CancelTransferCmd, FITP_CANCEL_TRANSFER_CMD_DATA_LEN);

         CFE_SB_InitMsg(&FileIn.HkPkt, 
                        CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_HK_TLM_MID)),
                        FILE_IN_TLM_HK_LEN, TRUE);

         /*
         ** Application startup event message
         */
         Status = CFE_EVS_SendEvent(FILE_IN_APP_INIT_EID, CFE_EVS_INFORMATION,
                                    "FILE_IN App Initialized. Version %d.%d.%d",
                                    FILE_IN_MAJOR_VER, FILE_IN_MINOR_VER, FILE_IN_PLATFORM_REV);
      } /* End if CFE_SUCCESS */
      
   } /* End if init success */

   return Status;

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
   
   Status = CFE_SB_RcvMsg(&CmdMsgPtr, FileIn.CmdPipe, CFE_SB_PEND_FOREVER);
        
   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_MsgIdToValue(CFE_SB_GetMsgId(CmdMsgPtr));

      if (MsgId == FileIn.CmdMidValue) 
      {
         CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
      }
      else if (MsgId == FileIn.SendHkMidValue)
      {
         SendHousekeepingPkt();
      }
      else
      {
         CFE_EVS_SendEvent(FILE_IN_APP_INVALID_MID_EID, CFE_EVS_ERROR,
                           "Received invalid command packet,MID = 0x%4X",MsgId);
      }

   } /* End if SB received a packet */

} /* End ProcessCommands() */
