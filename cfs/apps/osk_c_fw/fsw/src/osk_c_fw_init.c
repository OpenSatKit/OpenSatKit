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

#include "osk_c_fw_cfg.h"
#include "osk_c_fw_ver.h"

/*
** Exported Functions
*/

/******************************************************************************
** Entry function
**
*/
uint32 OSK_C_FW_LibInit(void)
{

   OS_printf("OSK C Application Framework Library Initialized. Version %d.%d.%d\n",
             OSK_C_FW_MAJOR_VER, OSK_C_FW_MINOR_VER, OSK_C_FW_LOCAL_REV);
   
  return OS_SUCCESS;

} /* End OSK_C_FW_LibInit() */

