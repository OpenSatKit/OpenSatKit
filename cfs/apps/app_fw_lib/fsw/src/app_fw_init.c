/* 
** Purpose: Generic entry point function for app framework library
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
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
uint32 APP_FW_Init(void)
{

  OS_printf("Application Framework Library Initialized. Version %d.%d.%d.%d",
             APP_FW_LIB_MAJOR_VERSION, APP_FW_LIB_MINOR_VERSION, 
             APP_FW_LIB_REVISION, APP_FW_LIB_MISSION_REV);

  return OS_SUCCESS;

} /* End APP_FW_Init() */

