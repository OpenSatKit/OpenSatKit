/*
** Purpose: Implement an example object.
**
** Notes:
**   1. This serves as an example object that uses a table. It does not perform
**      any realistic funcions.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>


#include "AppCfg.h"
#include "ExAppObj.h"

enum EID {

   EID_PROTO_CMD = AppCfg::EX_APP_OBJ_BASE_EID,
   EID_INVALID_TBL_ID,
   EID_INVALID_TBL_ENTRY_INDX
   
};


int JsonDom(void);


/******************************************************************************
** Constructor
*/
ExAppObj::ExAppObj(uint16 event_id_base) : AppObj(event_id_base),
   m_proto_cmd(this, EX_APP_OBJ_PROTO_CMD_DATA_LEN),
   m_ex_jsmn_tbl(OSK_CPP_PROTO_JSON_TBL_DEF_LOAD_FILE),
   m_ex_rapid_tbl(OSK_CPP_PROTO_JSON_TBL_DEF_LOAD_FILE),
   m_ex_cfe_tbl(OSK_CPP_PROTO_CFE_TBL_NAME,CFE_TBL_OPT_DEFAULT) {

   m_exec_cnt = 0;
   
}


void ExAppObj::ResetState() {

   m_exec_cnt = 0;
   
   m_ex_jsmn_tbl.ResetState();
   m_ex_rapid_tbl.ResetState();
   
}

uint8 ExAppObj::RegisterCmds (oskfw::CmdMgr& cmd_mgr) {

   cmd_mgr.RegisterCmd(m_proto_cmd, AppCfg::EX_APP_OBJ_PROTO_CMD_FC);
   
   return 1;

}

uint8 ExAppObj::RegisterTbls (oskfw::TblMgr& tbl_mgr) {

   uint8 num_tbls_reg = 0;
   
   m_ex_jsmn_tbl_id = tbl_mgr.RegisterTbl(&m_ex_jsmn_tbl);
   if (m_ex_jsmn_tbl_id != oskfw::Cfg::TBL_ID_UNDEF) num_tbls_reg++;
   
   m_ex_rapid_tbl_id = tbl_mgr.RegisterTbl(&m_ex_rapid_tbl);
   if (m_ex_rapid_tbl_id != oskfw::Cfg::TBL_ID_UNDEF) num_tbls_reg++;
   
   m_ex_cfe_tbl.Register(ValidateExCfeTbl,OSK_CPP_PROTO_CFE_TBL_DEF_LOAD_FILE);
   
   return num_tbls_reg;
 
}


/******************************************************************************
** Periodic Processing
*/

void ExAppObj::Execute() {

   m_exec_cnt++;

} 


void ExAppObj::Monitor() {

   m_ex_cfe_tbl.ManageService();

} 

int32 ExAppObj::ValidateExCfeTbl(void* TblPtr) {
  
   OS_printf("ExAppObj::ValidateExCfeTbl()\n");
   
   return  CFE_SUCCESS;
  
} 

/******************************************************************************
** ProtoCmd
*/

ExAppObj::ProtoCmd::ProtoCmd(ExAppObj* ex_app_obj, uint16 data_len) : oskfw::CmdMsg(data_len) , m_ex_app_obj(ex_app_obj) {}

boolean ExAppObj::ProtoCmd::Process(const CFE_SB_MsgPtr_t msg_ptr) {

   boolean ret_status  = FALSE;
   boolean valid_table = FALSE;
   boolean valid_entry = FALSE;
   ExTblEntry tbl_entry;
   std::string table_str[4] = {"Null", "ExJsmnTbl", "ExRapidTbl", "ExCfeTbl"};
   
   const ExAppObj_ProtoCmdMsg *cmd_msg = (const ExAppObj_ProtoCmdMsg *) msg_ptr;
   int table_id   = cmd_msg->table_id;
   int entry_indx = cmd_msg->entry_indx;
   ExTblParam* ex_cfe_tbl;
      
   switch (table_id) {   
   case 1:
      valid_entry = m_ex_app_obj->m_ex_jsmn_tbl.get_entry(entry_indx, &tbl_entry);
      valid_table = TRUE;
      break;
   case 2:
      valid_entry = m_ex_app_obj->m_ex_rapid_tbl.get_entry(entry_indx, &tbl_entry);
      valid_table = TRUE;
      break;
   case 3:
      ex_cfe_tbl = m_ex_app_obj->m_ex_cfe_tbl.get_data_ptr();
      if (ex_cfe_tbl != nullptr) {
         tbl_entry = ex_cfe_tbl->entry[entry_indx];
         valid_table = valid_entry = TRUE;
      }
      break;
   default:
      CFE_EVS_SendEvent(EID_INVALID_TBL_ID, CFE_EVS_ERROR, "Invalid table identifer %d. Index must be from 1 to 2", table_id);
   }
   
   if (valid_table) {

      if (valid_entry) {
         
         
         CFE_EVS_SendEvent (EID_PROTO_CMD, CFE_EVS_INFORMATION,
                            "%s data entry %d: %04d, %04d, %04d",
                            table_str[table_id].c_str(), entry_indx, tbl_entry.data1, tbl_entry.data2, tbl_entry.data3);
         
         ret_status = TRUE;
         
      }
      else {
         
          CFE_EVS_SendEvent(EID_INVALID_TBL_ENTRY_INDX, CFE_EVS_ERROR, "Invalid table data entry index %d. Index must be from 0 to %d", 
                           entry_indx, (AppCfg::EXTBL_MAX_ENTRY_ID-1));
      }

   } // End if valid table
   
   return ret_status;
   
} // End ProtoCmd()

