/*
** Purpose: Define configurations for the Cpp application.
**
** Notes:
**   1. These macros can only be built with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "OskCppFwCfg.h"
#include "osk_cpp_proto_platform_cfg.h"


//#define "CPP_PROTO_APP"


struct AppCfg {
   
   /*
   ** Version
   **
   ** 1.0 - Initial release
   ** 1.1 - Fixed cFE default table load issues
   */   
   static const int MAJOR_VER  = 1; 
   static const int MINOR_VER  = 1;
   static const int LOCAL_REV  = OSK_CPP_PROTO_PLATFORM_REV;

   // Commands
   static const uint16 EX_APP_OBJ_PROTO_CMD_FC = oskfw::Cfg::CMD_FC_ID_APP_FIRST;
  
   // Tables
   
   static const int EXTBL_MAX_ENTRY_ID = 32;
   
   static const int EXTBL_JSON_CHAR_OVERHEAD  = 300;  // Characters not in entry fields
   static const int EXTBL_JSON_CHAR_PER_ENTRY = 120;  // Characters per entry: 6 lines * 20 char/line is comfortable
   static const int EXTBL_JSON_CHAR_MAX = (EXTBL_JSON_CHAR_OVERHEAD + EXTBL_JSON_CHAR_PER_ENTRY*EXTBL_MAX_ENTRY_ID);
   

   // Event Message IDs
   //
   // Define the base event message IDs used by each object/component used by the
   // application. There are no automated checks to ensure an ID range is not
   // exceeded so it is the developer's responsibility to verify the ranges. 
   // 
   // One event number used for all debug messages
   //
  

   static const int OSK_CPP_PROTO_APP_BASE_EID  = oskfw::Cfg::APP_BASE_EID;
   static const int EXTBL_BASE_EID             = (OSK_CPP_PROTO_APP_BASE_EID + 10);
   static const int EX_APP_OBJ_BASE_EID        = (OSK_CPP_PROTO_APP_BASE_EID + 20);
   static const int EX_APP_DEBUG               = 999;

}; // End AppCfg

#endif /* _app_cfg_ */
