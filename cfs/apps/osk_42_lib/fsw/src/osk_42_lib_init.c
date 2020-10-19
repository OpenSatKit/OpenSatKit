/* 
** Purpose: Generic entry point function for OSK 42 library
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
#include "osk_42_lib.h"
#include "osk_42_lib_ver.h"

/*
** Exported Functions
*/

/******************************************************************************
** Entry function
**
*/
int32 OSK_42_LIB_LibInit(void)
{

   OS_printf("OSK 42 Library Initialized. Version %d.%d.%d\n",
             OSK_42_LIB_MAJOR_VER, OSK_42_LIB_MINOR_VER, OSK_42_LIB_LOCAL_REV);
   
   return OS_SUCCESS;

} /* End OSK_42_LIB_LibInit() */

