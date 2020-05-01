/* 
** Purpose: Define App Object class methods
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

#include "AppObj.h"

namespace oskfw {
   
AppObj::AppObj(uint16 event_id_base) : m_event_id_base(event_id_base) {}
AppObj::~AppObj() {}

void AppObj::ResetState() {}
void AppObj::Execute() {}
      
uint8  AppObj::RegisterCmds (CmdMgr& cmd_mgr) { return 0; }
uint8  AppObj::RegisterTbls (TblMgr& tbl_mgr) { return 0; }
      
} // End namespace oskfw
