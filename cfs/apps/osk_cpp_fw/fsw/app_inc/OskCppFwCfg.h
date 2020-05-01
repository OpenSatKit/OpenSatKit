/*
** Purpose: Define configurations for the C++ application framework
**
** Notes:
**   1. Resources are statically allocated for each application's
**      data structures so these configurations muct be sized to 
**      accommodate the application requiring the most resources.
**   2. static std::string are not used due to ambiguous construction order
**   3. 0 is not used as valid ID value 
**   4. A debug enable/disable setting is provided for each subsystem. The
**      maintain app command (CMD_FC_MAINT_APP) can be used to configure 
**      this setting during runtime.  
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

#ifndef _osk_cpp_fw_cfg_
#define _osk_cpp_fw_cfg_

/*
** Includes
*/

extern "C" {
   #include "cfe.h"
}

#include "OskCppFwPlatformCfg.h"


namespace oskfw {


#define OSK_CPP_FW_CFG_UNDEF_STR "undefined"

struct Cfg {
   
   // Commands
   
   static const int CMD_FC_ID_FIRST     =  1; 
   static const int CMD_FC_NOOP         =  1;  
   static const int CMD_FC_RESET_APP    =  2;
   static const int CMD_FC_LOAD_TBL     =  3;
   static const int CMD_FC_DUMP_TBL     =  4;
   static const int CMD_FC_DEBUG_APP    =  5;
   static const int CMD_FC_ID_APP_FIRST = 10;  // First FC available for app
   static const int CMD_FC_ID_LAST      = PlatformCfg::CMD_FC_ID_LAST; 
   
   static const int CMD_DEBUG = PlatformCfg::CMD_DEBUG;

   // Tables
   
   static const int TBL_ID_FIRST  =  1;
   static const int TBL_ID_LAST   =  PlatformCfg::TBL_ID_LAST;
   static const int TBL_ID_UNDEF  = 99;

   static const int TBL_DEBUG = PlatformCfg::TBL_DEBUG;

   // JSON

   static const int JSON_DEBUG = PlatformCfg::JSON_DEBUG;

   // Fault Reporter
   
   static const int FAULTREP_ID_LAST = PlatformCfg::FAULTREP_ID_LAST;
   static const int FAULTREP_DEBUG   = PlatformCfg::FAULTREP_DEBUG;
   
   // Event Message IDs
   //
   // Define the base event message IDs used by each object/component used by the
   // application. There are no automated checks to ensure an ID range is not
   // exceeded so it is the developer's responsibility to verify the ranges. 


   static const int OSK_CPP_FW_BASE_EID =   1;   // Intentionally skip 0 
   static const int CMD_MSG_BASE_EID    =  10; 
   static const int CMD_MGR_BASE_EID    =  20;
   static const int TBL_BASE_EID        =  30;
   static const int TBL_MGR_BASE_EID    =  40;
   static const int JSON_BASE_EID       =  50;
   static const int FAULT_REP_BASE_EID  =  70;
   static const int UTIL_BASE_EID       =  80;
   static const int APP_BASE_EID        = 100;  // Starting ID for the App using the framework
   
}; // End Cfg

} // End oskfw namespace


#endif // _osk_cpp_fw_cfg_
