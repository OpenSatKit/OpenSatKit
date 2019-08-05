/* 
** File:
**   $Id:  $
**
** Purpose: Generic entry point function for expat library
**
** References:
**   1. Core Flight Executive Application Developers Guide.
**   2. http://expat.sourceforge.net/
**
**
** $Date:  $
** $Revision:  $
** $Log:  $
**
*/

/*
** Includes
*/

#include "osapi.h"

/*
** Exported Functions
*/

/******************************************************************************
** Entry function
**
*/
uint32 EXPAT_Init(void)
{

  OS_printf("EXPAT Library 2.1.0 Loaded\n");
  return OS_SUCCESS;

} /* End EXPAT_Init() */

