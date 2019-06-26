/* 
** Purpose: Generic entry point function for OSK app framework library
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
** Includes
*/

#include "osapi.h"
#include "app_fw_cfg.h"
#include "app_fw_version.h"

/*
** Exported Functions
*/

/******************************************************************************
** Entry function
**
*/
uint32 OSK_APP_FwInit(void)
{

  CFE_EVS_SendEvent(APP_FW_INIT_INFO_EID, CFE_EVS_INFORMATION,
    "Application Framework Library Initialized. Version %d.%d.%d.%d",
    APP_FW_LIB_MAJOR_VERSION, APP_FW_LIB_MINOR_VERSION, 
    APP_FW_LIB_REVISION, APP_FW_LIB_MISSION_REV);
   
  return OS_SUCCESS;

} /* End OSK_APP_FwInit() */

