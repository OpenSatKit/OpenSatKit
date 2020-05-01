/* 
** Purpose: Manage command dispatching for an application.
**
** Notes:
**   1. Zero is an unused ID. Commands have minimal storage so [0] is
**      initialized but invalid. Keeps indexing simple.
**
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
** Include Files:
*/

#include "CmdMgr.h"

/*
** Event Message IDs
*/

enum EID {
   
   EID_REG_INVALID_FUNC_CODE = oskfw::Cfg::CMD_MGR_BASE_EID,
   EID_DISPATCH_INVALID_CHECKSUM,
   EID_DISPATCH_INVALID_LEN,
   EID_DISPATCH_INVALID_FUNC_CODE,
   EID_NONEXISTENT_CMD

};

namespace oskfw {
   
/******************************************************************************
** Function: CmdMgr
**
*/

CmdMgr::CmdMgr() {

   int i;

   m_valid_cmd_cnt   = 0;
   m_invalid_cmd_cnt = 0;

   // See prologue
   for (i=0; i <= oskfw::Cfg::CMD_FC_ID_LAST; i++) {
      
      m_cmd_msg[i] = &m_cmd_stub;
      
   }

   m_debug_enabled = oskfw::Cfg::CMD_DEBUG;
   
} // End CmdMgr()


/******************************************************************************
** Function: RegisterFunc
**
*/
boolean CmdMgr::RegisterCmd(CmdMsg& cmd_msg, uint16 func_code) {
   
   boolean ret_status = FALSE;
   
   if (func_code >= oskfw::Cfg::CMD_FC_ID_FIRST && func_code <= oskfw::Cfg::CMD_FC_ID_LAST) {

      if (m_debug_enabled) OS_printf("CmdMgr::RegisterCmd(): FuncCode %d, DataLen %d\n", func_code, cmd_msg.data_len());
      
      m_cmd_msg[func_code] = &cmd_msg;
      
      ret_status = TRUE;

   }
   else {
      
      CFE_EVS_SendEvent (EID_REG_INVALID_FUNC_CODE, CFE_EVS_ERROR,
         "Attempt to register function code %d which is greater than maximum ID %d",
         func_code, oskfw::Cfg::CMD_FC_ID_LAST);
   }

} // End RegisterCmd()


/******************************************************************************
** Function: ResetState
**
*/
void CmdMgr::ResetState() {

   m_valid_cmd_cnt   = 0;
   m_invalid_cmd_cnt = 0;

} // End ResetState()


/******************************************************************************
** Function: ProcessCmd
**
*/
boolean CmdMgr::ProcessCmd(const CFE_SB_MsgPtr_t  msg_ptr) {

   boolean  valid_cmd = FALSE;

   uint16  user_data_len = CFE_SB_GetUserDataLength(msg_ptr);
   uint16  func_code     = CFE_SB_GetCmdCode(msg_ptr);
   uint32  checksum      = CFE_SB_GetChecksum(msg_ptr);
   CmdMsg* cmd_msg;
   
   if (oskfw::Cfg::CMD_DEBUG) OS_printf("CmdMgr::ProcessCmd(): FuncCode %d, DataLen %d\n", func_code, user_data_len);
   if (oskfw::Cfg::CMD_DEBUG) OS_printf("CmdMgr::ProcessCmd(): [0]=0x%X, [0]=0x%X, [0]=0x%X, [0]=0x%X\n",
                             ((uint16*)msg_ptr)[0],((uint16*)msg_ptr)[1],((uint16*)msg_ptr)[2],((uint16*)msg_ptr)[3]);

   if (func_code >= oskfw::Cfg::CMD_FC_ID_FIRST && func_code <= oskfw::Cfg::CMD_FC_ID_LAST) {

      cmd_msg = m_cmd_msg[func_code];
      
      if (cmd_msg->ValidDataLen(user_data_len)) {

         if (CFE_SB_ValidateChecksum(msg_ptr) == TRUE) {

            valid_cmd = cmd_msg->Process(msg_ptr);

         } /* End if valid checksum */
         else {

            CFE_EVS_SendEvent (EID_DISPATCH_INVALID_CHECKSUM, CFE_EVS_ERROR,
                               "Invalid command checksum %d", checksum);
         }

      } /* End if valid length */
      else {

         CFE_EVS_SendEvent (EID_DISPATCH_INVALID_LEN, CFE_EVS_ERROR,
                            "Invalid command user data length %d, expected %d",
                            user_data_len, cmd_msg->data_len());

      }

   } /* End if valid function code */
   else {
      
      CFE_EVS_SendEvent (EID_DISPATCH_INVALID_FUNC_CODE, CFE_EVS_ERROR,
                         "Invalid command function code %d. Valid ID range is %d to %d",
                         func_code, oskfw::Cfg::CMD_FC_ID_FIRST, oskfw::Cfg::CMD_FC_ID_LAST);

   } // End if invalid function code

   valid_cmd ? m_valid_cmd_cnt++ : m_invalid_cmd_cnt++;
   
   return valid_cmd;

} // End ProcessCmd()


/******************************************************************************
** CmdStub
**
*/
boolean CmdMgr::CmdStub::Process(const CFE_SB_MsgPtr_t msg_ptr) {
   

   CFE_EVS_SendEvent (EID_NONEXISTENT_CMD, CFE_EVS_ERROR, "Attempt to execute a nonexistent command");

   return FALSE;

} // End CmdStub::Process()
 
} // End namespace oskfw
