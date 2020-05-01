/* 
** Purpose: Define CmdMsg class methods
**
** Notes:
**   None
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

#include "CmdMsg.h"

/*
** Event Message IDs
*/

enum EID {
   
   EID_UNUSED_FUNC_CODE = oskfw::Cfg::CMD_MSG_BASE_EID

};

namespace oskfw {
   
   
CmdMsg::CmdMsg() {

   m_data_len = 0;
   
} // End CmdMsg()


CmdMsg::CmdMsg(uint16 data_len) {

   m_data_len = data_len;
   
} // End CmdMsg()


CmdMsg::~CmdMsg() {}

   
boolean CmdMsg::Process(const CFE_SB_MsgPtr_t msg_ptr) {
   
   CFE_EVS_SendEvent (EID_UNUSED_FUNC_CODE, CFE_EVS_ERROR,
                      "Unused command function code %d received",CFE_SB_GetCmdCode(msg_ptr));

   return FALSE;

} // End Process()

} // End namespace oskfw