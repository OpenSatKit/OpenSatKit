/* 
** Purpose: Implement base table class
**
** Notes:
**   1. Default methods are defined for virtual functions to allow stub
**      objects provide safe behavior in situations when default
**      configurations are needed. However for normal operations this
**      class is designed to be subclassed with concrete implementations.  
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

#include "OskCppFwCfg.h"
#include "Tbl.h"
using std::string;

// Event Message IDs

enum EID {
   
   EID_TBL_LOAD_STUB = oskfw::Cfg::TBL_BASE_EID,
   EID_TBL_DUMP_STUB

};

namespace oskfw {


/******************************************************************************
** Constructor 
**
*/
Tbl::Tbl() {

   m_last_action       = ACTION_CREATED;
   m_last_action_valid = true;
   
   m_loaded = false;
   m_filename = OSK_CPP_FW_CFG_UNDEF_STR;
   
} // End Tbl()


Tbl::~Tbl() {}


/******************************************************************************
** Load Functions
**
*/
const string Tbl::LoadOptionStr(LoadOption load_opt) {

   string LoadOptStr[3] = {
      "Replace",
      "Update",
      "Undefined" 
   };

   uint8 i = 2;
   
   if ( load_opt == LOAD_REPLACE ||
        load_opt == LOAD_UPDATE) {
   
      i = load_opt;
   
   }
        
   return LoadOptStr[i];

} // End LoadOptionStr()


boolean Tbl::Load(LoadOption load_opt, const std::string& p_filename) {

   m_last_action = ACTION_LOAD;   
   m_last_action_valid = FALSE;
   m_filename = p_filename;
   
   // Retain m_loaded state if this load fails
    
   m_last_action_valid = LoadContent(load_opt, p_filename);
   if (m_last_action_valid == TRUE) m_loaded = TRUE;
  
   return m_last_action_valid;

} // End Load()


boolean Tbl::LoadContent (LoadOption load_opt, const std::string& p_filename) {

   CFE_EVS_SendEvent (EID_TBL_LOAD_STUB, CFE_EVS_ERROR,
                      "Critical error. Attempt to load an undefined table from file %s",
                      p_filename.c_str());

   return FALSE;

} // End LoadContent()


/******************************************************************************
** Dump Functions
**
*/
boolean Tbl::Dump (DumpOption load_opt, const std::string& p_filename) {

   m_last_action = ACTION_DUMP;   
   m_last_action_valid = FALSE;
   m_filename = p_filename;

   // Could be a dump-only table so don't need to check whether it's been loaded   

   m_last_action_valid = DumpContent(p_filename);
      
   return m_last_action_valid;

} // End Load()


boolean Tbl::DumpContent(const std::string& p_filename) {
   
   CFE_EVS_SendEvent (EID_TBL_DUMP_STUB, CFE_EVS_ERROR,
                      "Critical error. Attempt to dump an undefined table to file %s",
                      p_filename.c_str());

   return FALSE;

} // End DumpContent()


} // End namespace oskfw
