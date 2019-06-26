/*
** Purpose: Define a comm interface between the 42fsw and f42 objects
**
** Notes:
**   1. All of the functions in this object are related to setting up 
**      a socket and transferring the required data to/from the FSW. The
**      f42 object provides a ground interface to the 42 FSW controller. 
**   2. This is not FSW and is designed to be compiled with 42.
**
** References:
**   1. See 42 open source repository at 
**      https://sourceforge.net/projects/fortytwospacecraftsimulation/
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/

#ifndef _f42_comm_
#define _f42_comm_

/*
** Includes
*/

#include "42types.h"

/*
** Constant Definitions
*/

#define F42_COMM_PORT 4242

/*
** Type Definitions
*/

/******************************************************************************
** F42 Communication Class
*/

typedef struct {
   
   double DeltaTime;
   
   unsigned long WriteSensorCount;
   unsigned long ReadActuatorCount;
   
   int Socket;
   FILE *Stream;
   
} F42_COMM_Class;


/*
** Exported Functions
*/

/******************************************************************************
** Function: F42_Comm_Init
**
** Initialize a F42_ADP object.
**
** Notes:
**   1. This must be called prior to any other F2_COMM function.
**
*/
void F42_Comm_Init(struct SCType *S);


/******************************************************************************
** Function: F42_Comm_FSW
**
** Interface with the 42FSW controller that is running in the cFS.
**
*/
void F42_Comm_FSW(struct SCType *S);


#endif /* _f42_comm_ */
