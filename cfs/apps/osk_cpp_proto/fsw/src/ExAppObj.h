/*
** Purpose: Define an example application object.
**
** Notes:
**   1. This object demonstrates how an application object is integrated into 
**      the C++ application framework.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _ex_app_obj_
#define _ex_app_obj_

/*
** Includes
*/

#include "AppCfg.h"
#include "AppObj.h"
#include "ExJsmnTbl.h"
#include "ExRapidTbl.h"
#include "ExTblStruct.h"
#include "CfeTbl.h"

/*
** Type Definitions
*/


/******************************************************************************
** Command Messages
*/

typedef struct
{

   uint8  cmd_header[CFE_SB_CMD_HDR_SIZE];
   uint8  table_id;
   uint8  entry_indx;

} ExAppObj_ProtoCmdMsg;
#define EX_APP_OBJ_PROTO_CMD_DATA_LEN  (sizeof(ExAppObj_ProtoCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** ExAppObj Class
*/

class ExAppObj : public oskfw::AppObj {
public:
   
   ExAppObj(uint16 event_id_base);
   
   void ResetState();
   void Execute();
   void Monitor();
   
   uint8 RegisterCmds (oskfw::CmdMgr& cmd_mgr);
   uint8 RegisterTbls (oskfw::TblMgr& tbl_mgr);
 
   uint8 exec_cnt()    { return m_exec_cnt; }
   oskfw::Json* json() { return m_ex_jsmn_tbl.json(); }
   
private:

   class ProtoCmd : public oskfw::CmdMsg {
   public:
      ProtoCmd(ExAppObj* ex_app_obj, uint16 data_len);
      boolean Process(const CFE_SB_MsgPtr_t msg_ptr);
   private:
      ExAppObj* m_ex_app_obj;
   };
   
   // Must match CFE_TBL_CallbackFuncPtr_t signature
   static int32 ValidateExCfeTbl(void* TblPtr);
   
   ProtoCmd  m_proto_cmd;
   uint16   m_exec_cnt;
   
   oskfw::TblId_t  m_ex_jsmn_tbl_id;
   ExJsmnTbl       m_ex_jsmn_tbl;
   
   oskfw::TblId_t  m_ex_rapid_tbl_id;
   ExRapidTbl      m_ex_rapid_tbl;
   
   oskfw::CfeTbl<ExTblParam> m_ex_cfe_tbl;
 
};


#endif // _exappobj_
