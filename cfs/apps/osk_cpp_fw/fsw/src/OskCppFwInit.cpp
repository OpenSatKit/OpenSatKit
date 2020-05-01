/* 
** Purpose: "C" entry point function for OSK C++ app framework library
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


#include "OskCppFwCfg.h"
#include "OskCppFwVer.h"


/******************************************************************************
** Library entry function
**
*/

extern "C" uint32 OSK_CPP_FW_LibInit(void);
uint32 OSK_CPP_FW_LibInit(void) {

   OS_printf("OSK C++ Application Framework Library Initialized. Version %d.%d.%d\n",
             oskfw::Ver::MAJOR, oskfw::Ver::MINOR, oskfw::Ver::LOCAL_REV); 
   
   return OS_SUCCESS;

} // End OSK_CPP_FW_LibInit()

