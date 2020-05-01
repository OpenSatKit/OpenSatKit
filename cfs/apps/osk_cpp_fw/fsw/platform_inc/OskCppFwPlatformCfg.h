/*
** Purpose: Define configurations for the C++ application framework
**
** Notes:
**   1. Resources are statically allocated for each application's
**      data structures so these configurations must be sized to 
**      accommodate the 'largest' application.
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

#ifndef _platform_cfg_h_
#define _platform_cfg_h_


/*
** Includes
*/

#include "OskCppFwMissionCfg.h"


namespace oskfw {


struct PlatformCfg {
   
   // Commands
   
   static const int CMD_FC_ID_LAST = 20;
   static const int CMD_DEBUG      = FALSE;

   // Tables
   
   static const int TBL_ID_LAST =  5;
   static const int TBL_DEBUG   = FALSE;

   // JSON

   static const int JSON_DEBUG = FALSE;

   // Fault Reporter
   
   static const int FAULTREP_ID_LAST = 32;    // Must be a multiple of 8
   static const int FAULTREP_DEBUG   = FALSE;
   
}; // End Cfg

} // End oskfw namespace

#endif // _platform_cfg_h_ 
