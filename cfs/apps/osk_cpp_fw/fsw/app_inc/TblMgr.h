/*
** Purpose: Manage tables for an application
**
** Notes:
**   1. This utility does not dictate a specific table format. It 
**      only specifies an API for managing an application's table.
**   2. Table ID range oskfw::Cfg::TBL_ID_LAST..oskfw::Cfg::TBL_ID_LAST
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

#ifndef _tbl_mgr_
#define _tbl_mgr_

/*
** Includes
*/

#include <string>

#include "OskCppFwCfg.h"
#include "Tbl.h"
#include "CmdMgr.h"

namespace oskfw {
   
/*
** Load/Dump Table Commands
**
** Identifer is assigned at the time of table registration
*/

typedef uint8 TblId_t;

typedef struct {
   
   uint8   cmd_header[CFE_SB_CMD_HDR_SIZE]; // cFE Software Bus Command Message Header
   TblId_t id;                              // Table identifier
   Tbl::LoadOption  load_option;            // Replace or update table records
   char    filename[OS_MAX_PATH_LEN];       // ASCII text string of full path and filename

} TBLMGR_LoadTblCmdMsg;
#define TBLMGR_LOAD_TBL_CMD_DATA_LEN  (sizeof(TBLMGR_LoadTblCmdMsg) - CFE_SB_CMD_HDR_SIZE)

typedef struct {
   
   uint8   cmd_header[CFE_SB_CMD_HDR_SIZE]; // cFE Software Bus Command Message Header
   TblId_t id;                              // Table identifier
   Tbl::DumpOption  dump_option;            // Placeholder for user defined qualifier
   char    filename[OS_MAX_PATH_LEN];       // ASCII text string of full path and filename

} TBLMGR_DumpTblCmdMsg;
#define TBLMGR_DUMP_TBL_CMD_DATA_LEN  (sizeof(TBLMGR_DumpTblCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/* 
** Table Manager Class
*/


class TblMgr {
public:

   TblMgr();

   // Register a table with or without loading a default table.
   // Returns table ID assigned to new table or oskfw::Cfg::TBL_ID_MAX if no IDs left.
   TblId_t RegisterTbl(Tbl* p_tbl);
   TblId_t RegisterTbl(Tbl* p_tbl, const std::string tbl_filename);

   uint8  RegisterCmds(CmdMgr& cmd_mgr);  // Return number of commands registered

   void ResetState();

   const Tbl* GetTbl(TblId_t tbl_id) const;
   const Tbl* GetLastActionTbl() const;
   
   boolean debug_enabled() { return m_debug_enabled; }
   void set_debug_enabled(boolean debug_enabled) { m_debug_enabled = debug_enabled; }
   
private:

   class LoadTblCmd : public CmdMsg {
   public:
      LoadTblCmd(TblMgr* tbl_mgr, uint16 data_len);
      boolean Process(const CFE_SB_MsgPtr_t msg_ptr);
   private:
      TblMgr* m_tbl_mgr;
   };

   class DumpTblCmd : public CmdMsg {
   public:      
      DumpTblCmd(TblMgr* tbl_mgr, uint16 data_len);
      boolean Process(const CFE_SB_MsgPtr_t msg_ptr);
   private:
      TblMgr* m_tbl_mgr;
   };

   class TblStub : public Tbl {
   public:      
      void ResetState() {}
  
   protected:
      boolean LoadContent (Tbl::LoadOption load_opt, const std::string& p_filename) { return FALSE; }
      boolean DumpContent (const std::string& p_filename) { return FALSE; }
   };

   LoadTblCmd  m_load_tbl_cmd;
   DumpTblCmd  m_dump_tbl_cmd;
   
   TblStub  m_tbl_stub;      
   
   TblId_t  m_next_available_id;
   TblId_t  m_last_action_tbl_id;
   Tbl*     m_tbl[oskfw::Cfg::TBL_ID_LAST];

   boolean ValidTblId(TblId_t tbl_id) const;

   boolean m_debug_enabled;
   
};

} // End namespace oskfw

#endif // _tbl_mgr_
