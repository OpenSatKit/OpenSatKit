/* 
** Purpose: Define App class
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

#include "App.h"

/*
** Event Message IDs
*/

enum EID {

   EID_APP_NOOP_CMD = oskfw::Cfg::APP_BASE_EID,
   EID_APP_DEBUG_CMD,
   EID_APP_DEBUG_CMD_ERR
   
};

namespace oskfw {
   
static std::string App_BoolStr[2] = { "False", "True" };

static std::string App_FwStr[4] = { "UNDEF", "Cmd", "Tbl", "JSON" };


App::App(int major_ver, int minor_ver, int local_rev) : 
   m_noop_cmd(*this), 
   m_reset_cmd(*this),
   m_debug_cmd(*this, APP_DEBUG_CMD_DATA_LEN) {

   m_major_ver = major_ver;
   m_minor_ver = minor_ver;
   m_local_rev = local_rev;
      
   m_json = nullptr;
   
   m_cmd_mgr.RegisterCmd(m_noop_cmd,  oskfw::Cfg::CMD_FC_NOOP);
   m_cmd_mgr.RegisterCmd(m_reset_cmd, oskfw::Cfg::CMD_FC_RESET_APP);  
   
   m_tbl_mgr.RegisterCmds(m_cmd_mgr);
   
   m_cmd_mgr.RegisterCmd(m_debug_cmd, oskfw::Cfg::CMD_FC_DEBUG_APP);  

} // End App()

App::~App() {}


/******************************************************************************
** NoopCmd
*/

App::NoopCmd::NoopCmd(App& app) : m_app(app) {}

boolean App::NoopCmd::Process(const CFE_SB_MsgPtr_t msg_ptr) {
   

   CFE_EVS_SendEvent (EID_APP_NOOP_CMD,
                      CFE_EVS_INFORMATION,
                      "No operation command received for CPP version %d.%d.%d",
                      m_app.m_major_ver, m_app.m_minor_ver, m_app.m_local_rev);

   return TRUE;


} // End NoopCmd::Process()


/******************************************************************************
** ResetCmd
*/

App::ResetCmd::ResetCmd(App& app) : m_app(app) {}

boolean App::ResetCmd::Process(const CFE_SB_MsgPtr_t msg_ptr) {
   
   return m_app.ResetState();
   
} // End ResetCmd::Process()

/******************************************************************************
** DebugCmd
**
** An application may have 0..N JSON parsers so this command will configure
** debugging for which ever JSON parser is set by the application. 
**
*/

App::DebugCmd::DebugCmd(App& app, uint16 data_len) : CmdMsg(data_len), m_app(app) {}

boolean App::DebugCmd::Process(const CFE_SB_MsgPtr_t msg_ptr) {
   
   boolean ret_status = TRUE;
   const  APP_DebugCmdMsg *debug = (const APP_DebugCmdMsg *) msg_ptr;
   
   boolean new_state = debug->enabled == TRUE? TRUE : FALSE;
   
   switch (debug->app_comp_id) {
   case CMD_MGR:
      m_app.m_cmd_mgr.set_debug_enabled(new_state);
      break;

   case TBL_MGR:
      m_app.m_tbl_mgr.set_debug_enabled(new_state);
      break;

   case JSON:
      if (m_app.m_json != nullptr) {
         m_app.m_json->set_debug_enabled(new_state);
      }         
      else {
         CFE_EVS_SendEvent(EID_APP_DEBUG_CMD_ERR, CFE_EVS_ERROR,
                          "App does not have a JSON parser that can be configured for debugging");
         ret_status = FALSE;
      }
      break;

   default:
      ret_status = FALSE;
      CFE_EVS_SendEvent (EID_APP_DEBUG_CMD_ERR, CFE_EVS_ERROR,
                         "Invalid application framework ID %d received for config debug cmd", debug->app_comp_id);

   } // End switch app_comp_id

   if (ret_status) {
      CFE_EVS_SendEvent (EID_APP_DEBUG_CMD, CFE_EVS_INFORMATION,
                         "Set framework %s component debug enabled to %s",
                         App_FwStr[debug->app_comp_id].c_str(), App_BoolStr[new_state].c_str());
   }
   
   return ret_status;


} // End DebugCmd::Process()

} // End namespace oskfw