/*
** Purpose: Manage command dispatching for an application
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

#ifndef _cmd_mgr_
#define _cmd_mgr_

/*
** Includes
*/

#include "OskCppFwCfg.h"
#include "CmdMsg.h"

namespace oskfw {

class CmdMgr {
public:

   CmdMgr();
   boolean RegisterCmd(CmdMsg& cmd_msg, uint16 func_code);
   void ResetState();
   boolean ProcessCmd(const CFE_SB_MsgPtr_t  msg_ptr);
       
   uint16 valid_cmd_cnt()   const { return m_valid_cmd_cnt;   }
   uint16 invalid_cmd_cnt() const { return m_invalid_cmd_cnt; }

   class CmdStub : public CmdMsg {
   public:
   
      CmdStub() {}
      boolean Process(const CFE_SB_MsgPtr_t msg_ptr);

   };

   boolean debug_enabled() { return m_debug_enabled; }
   void set_debug_enabled(boolean debug_enabled) { m_debug_enabled = debug_enabled; }
   
private:
   uint16   m_valid_cmd_cnt;       // Number of valid messages received since init or reset
   uint16   m_invalid_cmd_cnt;     // Number of invalid messages received since init or reset
   CmdMsg*  m_cmd_msg[oskfw::Cfg::CMD_FC_ID_LAST];

   CmdStub  m_cmd_stub;
   
   boolean m_debug_enabled;
      
}; // End class CmdMgr

}  // End namespace oskfw

#endif // _cmd_mgr_
