/* 
** Purpose: Manage an application's tables
**
** Notes:
**   1. Zero is an unused ID. Tables have minimal storage so [0] is
**      initialized but invalid. Keeps indexing simple.
**   2. Ids range from oskfw::Cfg::TBL_ID_FIRST..oskfw::Cfg::TBL_ID_LAST 
**   3. TBL_ID_UNDEF is returned as an ID when an operation fails.
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

#include <string.h>
#include "TblMgr.h"
#include "FileUtil.h"

/*
** Event Message IDs
*/

enum EID {
   EID_REG_EXCEEDED_MAX = oskfw::Cfg::TBL_MGR_BASE_EID,
   EID_NO_TBL_DEFINED,
   EID_INVALID_TBL_ID,
   EID_LOAD_SUCCESS,
   EID_DUMP_SUCCESS,
   EID_DEBUG
};

namespace oskfw {
   
/******************************************************************************
** Function: TblMgr 
**
*/
TblMgr::TblMgr() : m_load_tbl_cmd(this,TBLMGR_LOAD_TBL_CMD_DATA_LEN), m_dump_tbl_cmd(this,TBLMGR_DUMP_TBL_CMD_DATA_LEN) {

   TblId_t i;

   m_next_available_id  = oskfw::Cfg::TBL_ID_FIRST;
   m_last_action_tbl_id = oskfw::Cfg::TBL_ID_UNDEF;
   
   // See prologue
   for (i=0; i <= oskfw::Cfg::TBL_ID_LAST; i++) {
      
      m_tbl[i] = &m_tbl_stub;
   
   }
   
   m_debug_enabled = oskfw::Cfg::TBL_DEBUG;
   
} // End TblMgr()



/******************************************************************************
** Function: RegisterTbl
**
** Register a table without loading a default table.
**
** Returns table ID
**    
*/
TblId_t TblMgr::RegisterTbl(Tbl* p_tbl) {
  
   m_last_action_tbl_id = oskfw::Cfg::TBL_ID_UNDEF;
   
   if (m_debug_enabled) OS_printf("TblMgr::RegisterTbl() Entry - m_next_available_id = %d\n",m_next_available_id);
   
   if (m_next_available_id <= oskfw::Cfg::TBL_ID_LAST) {
      
      m_tbl[m_next_available_id] = p_tbl;
              
      m_last_action_tbl_id = m_next_available_id;
      m_next_available_id++;

   }
   else {
      
      CFE_EVS_SendEvent (EID_REG_EXCEEDED_MAX, CFE_EVS_ERROR,
         "Attempt to register a table that would exceed the maximum %d tables defined per app",
         oskfw::Cfg::TBL_ID_LAST);
   }
  
   return m_last_action_tbl_id;
   
} // End RegisterTbl()


/******************************************************************************
** Function: RegisterTblWithDef
**
** Register a table and load a default table
**
** Returns table ID.
*/
TblId_t TblMgr::RegisterTbl(Tbl* p_tbl, const std::string tbl_filename) {

   TblId_t tbl_id = RegisterTbl(p_tbl);
   
   TBLMGR_LoadTblCmdMsg load_tbl_cmd_msg;

   if (m_debug_enabled) OS_printf("TblMgr::RegisterTblWithDef() Entry - Assigned ID %d\n", tbl_id);

   if (tbl_id != oskfw::Cfg::TBL_ID_UNDEF) {
      
      // Use table load command function
      load_tbl_cmd_msg.id = tbl_id;
      load_tbl_cmd_msg.load_option = Tbl::LOAD_REPLACE;
      strncpy (load_tbl_cmd_msg.filename,tbl_filename.c_str(),OS_MAX_PATH_LEN);
      load_tbl_cmd_msg.filename[OS_MAX_PATH_LEN-1] = '\0';
      
      m_load_tbl_cmd.Process((CFE_SB_MsgPtr_t)&load_tbl_cmd_msg);
      
   } /* End if tbl_id valid */
   
   return tbl_id;
  
} // End RegisterTblWithDef()


/******************************************************************************
** Function: ResetState
**
*/
void TblMgr::ResetState() {

   /* Nothing to do - Preserve status of most recent action */ 

} // End ResetState()


/******************************************************************************
** Function: RegisterCmds
**
*/
uint8  TblMgr::RegisterCmds (CmdMgr& cmd_mgr) {

   cmd_mgr.RegisterCmd(m_load_tbl_cmd, oskfw::Cfg::CMD_FC_LOAD_TBL);
   cmd_mgr.RegisterCmd(m_dump_tbl_cmd, oskfw::Cfg::CMD_FC_DUMP_TBL);
   
   return 2;
   
}

/******************************************************************************
** Retrieve table info functions
**
*/

const Tbl* TblMgr::GetTbl(TblId_t tbl_id) const {
   
   Tbl* tbl_ptr = NULL;
   
   if ( ValidTblId(tbl_id) ) {
   
      tbl_ptr = m_tbl[tbl_id];
      
   }
   
   return tbl_ptr;
   
} // End GetTbl()


const Tbl* TblMgr::GetLastActionTbl() const {

   return m_tbl[m_last_action_tbl_id];  // Stub table returned if no tables registered

} // GetLastActionTbl()


/******************************************************************************
** Function: ValidTblId
**
** Returns TRUE if valid table ID, else returns false
*/
boolean TblMgr::ValidTblId(TblId_t tbl_id) const {
   
   boolean ret_status = FALSE;
   
   if (m_next_available_id == oskfw::Cfg::TBL_ID_FIRST) {
      
      CFE_EVS_SendEvent(EID_NO_TBL_DEFINED, CFE_EVS_ERROR, 
                        "Table %d failed. No tables defined for this app.",
                        tbl_id);
   
   }
   else {

      if (tbl_id >= oskfw::Cfg::TBL_ID_FIRST && tbl_id < m_next_available_id) {

         ret_status = TRUE;
         
      }
      else {         

         CFE_EVS_SendEvent(EID_INVALID_TBL_ID, CFE_EVS_ERROR, 
                           "Table %d failed. Valid ID range is %d to %d.",
                           tbl_id, oskfw::Cfg::TBL_ID_FIRST, m_next_available_id);     
      }
   }
      
   return ret_status;

} // End ValidTblId()


/******************************************************************************
** Function: LoadTblCmd
**
*/
TblMgr::LoadTblCmd::LoadTblCmd(TblMgr* tbl_mgr, uint16 data_len) :  CmdMsg(data_len), m_tbl_mgr(tbl_mgr) {}

boolean TblMgr::LoadTblCmd::Process(const CFE_SB_MsgPtr_t msg_ptr) {

   boolean ret_status = FALSE;
   
   Tbl*   tbl_ptr;
   const  TBLMGR_LoadTblCmdMsg *load_tbl = (const TBLMGR_LoadTblCmdMsg *) msg_ptr;

   CFE_EVS_SendEvent(EID_DEBUG, CFE_EVS_DEBUG, "TblMgr::LoadTblCmd() Entry. Table ID %d, File %s", 
                     load_tbl->id, load_tbl->filename);

   // Errors reported by utility so no need for else clause
   if (m_tbl_mgr->ValidTblId(load_tbl->id)) {
      
      tbl_ptr = m_tbl_mgr->m_tbl[load_tbl->id];
   
      // Errors reported by utility so no need for else clause
      if (FileUtil::VerifyFileForRead(load_tbl->filename)) {
      
         CFE_EVS_SendEvent(EID_DEBUG, CFE_EVS_DEBUG, "TblMgr::LoadTblCmd() Entry. Before Tbl->Load() call"); 

         ret_status = tbl_ptr->Load(load_tbl->load_option, std::string(load_tbl->filename));

         if (ret_status == TRUE) {
         
            CFE_EVS_SendEvent(EID_LOAD_SUCCESS, CFE_EVS_INFORMATION, 
                              "Sucessfully %sd table %d using file %s",
                              tbl_ptr->LoadOptionStr(load_tbl->load_option).c_str(),
                              load_tbl->id, load_tbl->filename);

         }
         
      } // End if vaid Filename
   } // End if ValidTblId
   
   return ret_status;
     
} // End LoadTblCmd::Process()


/******************************************************************************
** Function: DumpTblCmd
**
** Note:
**  1. This function must match CmdMsg::CmdFuncVoid type definition
** 
*/
TblMgr::DumpTblCmd::DumpTblCmd(TblMgr* tbl_mgr, uint16 data_len) : CmdMsg(data_len), m_tbl_mgr(tbl_mgr)  {}

boolean TblMgr::DumpTblCmd::Process(const CFE_SB_MsgPtr_t msg_ptr) {

   boolean ret_status = FALSE;
   
   Tbl*   tbl_ptr;
   const  TBLMGR_DumpTblCmdMsg *dump_tbl = (const TBLMGR_DumpTblCmdMsg *) msg_ptr;

   CFE_EVS_SendEvent(EID_DEBUG, CFE_EVS_DEBUG, "TblMgr::DumpTblCmd() Entry. Table ID %d, File %s", 
                     dump_tbl->id, dump_tbl->filename);

   // Errors reported by utility so no need for else clause
   if (m_tbl_mgr->ValidTblId(dump_tbl->id)) {
      
      tbl_ptr = m_tbl_mgr->m_tbl[dump_tbl->id];
   
      // Errors reported by utility so no need for else clause
      if (FileUtil::VerifyDirForWrite(dump_tbl->filename)) {
      
         CFE_EVS_SendEvent(EID_DEBUG, CFE_EVS_DEBUG, "TblMgr::LoadTblCmd() Entry. Before Tbl->Load() call"); 

         ret_status = tbl_ptr->Dump(dump_tbl->dump_option, std::string(dump_tbl->filename));

         if (ret_status == TRUE) {
         
            CFE_EVS_SendEvent(EID_DUMP_SUCCESS, CFE_EVS_INFORMATION, 
                              "Sucessfully dumped table %d to file %s",
                              dump_tbl->id, dump_tbl->filename);
         }
         
      } // End if vaid DirForWrite
   } // End if ValidTblId
   
   return ret_status;

} // End DumpTblCmd::Process()

} // End namespace oskfw