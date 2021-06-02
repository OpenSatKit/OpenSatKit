/* 
** Purpose: C++ proto app using the OSK C++ App Framework
**
** Notes:
**   None
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
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
#include "OskCppProtoApp.h"

/*
** Macro Definitions
*/

enum EID {
   
   EID_APP_INIT = AppCfg::OSK_CPP_PROTO_APP_BASE_EID,
   EID_APP_EXIT,
   EID_INVALID_CMD_MID

};


/******************************************************************************
** Function: OskCppProto_AppMain
**
** Notes:
**   1. C function used as cFE entry point.
*/

static OskCppProtoApp g_osk_cpp_proto_app (AppCfg::MAJOR_VER, AppCfg::MINOR_VER, AppCfg::LOCAL_REV);


// Naming convention would dictate OSK_CPP_PROTO_AppMain but the name gets too long
extern "C" void OSK_CPP_PROTO_Main(void);
void OSK_CPP_PROTO_Main(void) {
   
   g_osk_cpp_proto_app.Execute();

}

OskCppProtoApp::OskCppProtoApp(int major_ver, int minor_ver, int local_rev) : App(major_ver, minor_ver, local_rev) , 
   m_ex_app_obj(AppCfg::OSK_CPP_PROTO_APP_BASE_EID) {
      
      
   m_json =  m_ex_app_obj.json();
   
}

OskCppProtoApp::~OskCppProtoApp() {}

void OskCppProtoApp::Execute() {
   
   int32  status        = CFE_SEVERITY_ERROR;
   uint32 es_app_status = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(OSK_CPP_PROTO_MAIN_PERF_ID);
   status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);


   /*
   ** Perform application specific initialization
   */
   if (status == CFE_SUCCESS) {
      
       status = Init();
   }

   /*
   ** At this point many flight apps use CFE_ES_WaitForStartupSync() to
   ** synchronize their startup timing with other apps. This is not
   ** needed for this simple app.
   */

   if (status == CFE_SUCCESS) es_app_status = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&es_app_status)) {

      /*
      ** This is just a an example loop. There are many ways to control the
      ** main loop execution flow.
      */
	  
	   CFE_ES_PerfLogExit(OSK_CPP_PROTO_MAIN_PERF_ID);
      OS_TaskDelay(OSK_CPP_PROTO_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(OSK_CPP_PROTO_MAIN_PERF_ID);

      m_ex_app_obj.Execute();

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   // Write to system log in case events not working

   CFE_ES_WriteToSysLog("CPP Terminating, RunLoop status = 0x%08X\n", es_app_status);

   CFE_EVS_SendEvent(EID_APP_EXIT, CFE_EVS_CRITICAL, "CPP Terminating,  RunLoop status = 0x%08X", es_app_status);

   CFE_ES_PerfLogExit(OSK_CPP_PROTO_MAIN_PERF_ID);
   CFE_ES_ExitApp(es_app_status);             // Let cFE kill the task (and any child tasks)

} // End Execute()



/******************************************************************************
** Function: ResetState
**
** Put the app in a known default state. This is called in rsponse to a ground
** reset command
*/

boolean OskCppProtoApp::ResetState() {

   m_cmd_mgr.ResetState();
   m_tbl_mgr.ResetState();

   m_ex_app_obj.ResetState();

   return TRUE;

} // End ResetState()


/******************************************************************************
** Function: Init
**
*/
int32 OskCppProtoApp::Init() {
   
  
   int32 status = CFE_SUCCESS;

    
   /*
   ** Initialize 'entity' objects
   */

   // All example objects initialized during declaration

   /*
   ** Initialize cFE interfaces 
   */

   CFE_SB_CreatePipe(&m_cmd_pipe, OSK_CPP_PROTO_CMD_PIPE_DEPTH, OSK_CPP_PROTO_CMD_PIPE_NAME);
   CFE_SB_Subscribe(OSK_CPP_PROTO_CMD_MID, m_cmd_pipe);
   CFE_SB_Subscribe(OSK_CPP_PROTO_SEND_HK_MID, m_cmd_pipe);

   /*
   ** Initialize App Framework Components 
   */

   m_ex_app_obj.RegisterCmds(m_cmd_mgr);
   m_ex_app_obj.RegisterTbls(m_tbl_mgr);
   
                         
   CFE_SB_InitMsg(&m_hk_pkt, OSK_CPP_PROTO_APP_TLM_HK_MID, OSK_CPP_PROTO_APP_TLM_HK_LEN, TRUE);

                        
   /*
   ** Application startup event message
   */
   status = CFE_EVS_SendEvent(EID_APP_INIT,
                              CFE_EVS_INFORMATION,
                              "OSK C++ Proto App Initialized. Version %d.%d.%d",
                              m_major_ver, m_minor_ver, m_local_rev);

   return(status);

} // End of Init()



/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
void OskCppProtoApp::SendHousekeepingPkt() {
   

   // Good design practice in case app expands to more than one table
   const oskfw::Tbl* last_tbl = m_tbl_mgr.GetLastActionTbl();

   /*
   ** CMDMGR Data
   */

   m_hk_pkt.valid_cmd_cnt   = m_cmd_mgr.valid_cmd_cnt();
   m_hk_pkt.invalid_cmd_cnt = m_cmd_mgr.invalid_cmd_cnt();

   /*
   ** ExTbl, ExAppObj Data
   ** - At a minimum all object variables affected by a reset should be included
   */

   m_hk_pkt.last_tbl_action        = last_tbl->last_action();
   m_hk_pkt.last_tbl_action_valid  = last_tbl->last_action_valid();

   m_hk_pkt.ex_app_obj_exec_cnt    = m_ex_app_obj.exec_cnt();

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &m_hk_pkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &m_hk_pkt);

   return;  
  
} // End SendHousekeepingPkt()



/******************************************************************************
** Function: ProcessCommands
**
*/
void OskCppProtoApp::ProcessCommands() {


   int32           status;
   CFE_SB_Msg_t*   cmd_msg_ptr;
   CFE_SB_MsgId_t  msg_id;

   status = CFE_SB_RcvMsg(&cmd_msg_ptr, m_cmd_pipe, CFE_SB_POLL);

   if (status == CFE_SUCCESS) {

      msg_id = CFE_SB_GetMsgId(cmd_msg_ptr);

      switch (msg_id) {
         
         case OSK_CPP_PROTO_CMD_MID:
            m_cmd_mgr.ProcessCmd(cmd_msg_ptr);
            break;

         case OSK_CPP_PROTO_SEND_HK_MID:
            m_ex_app_obj.Monitor(); // This would be multiple calls for each AppObj
            SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(EID_INVALID_CMD_MID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",msg_id);

            break;

      } /* End Msgid switch */

   } // End if SB received a packet

} // ProcessCommands()