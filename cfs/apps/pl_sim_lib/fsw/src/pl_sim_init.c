/* 
** Purpose: Entry point function for OSK app framework library
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

#include "lib_cfg.h"
#include "pl_sim_lib.h"

/*****************/
/** Global Data **/
/*****************/

PL_SIM_LIB_Class_t  PlSimLib;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Entry function
**
*/
uint32 PL_SIM_LibInit(void)
{

   uint32 RetStatus = OS_ERROR;
   
   /*
   ** Initialize objects 
   */

   if (PL_SIM_LIB_Constructor(&PlSimLib))
   {

      RetStatus = OS_SUCCESS;
      OS_printf("Payload Simulator Library Initialized. Version %d.%d.%d\n",
                PL_SIM_LIB_MAJOR_VER, PL_SIM_LIB_MINOR_VER, PL_SIM_LIB_PLATFORM_REV);
   }
   else
   {
      OS_printf("Error initializing Payload Simulator Library. Version %d.%d.%d\n",
                PL_SIM_LIB_MAJOR_VER, PL_SIM_LIB_MINOR_VER, PL_SIM_LIB_PLATFORM_REV);      
   }
   
   return RetStatus;

} /* End PL_SIM_LibInit() */

