/*
** Purpose: Implement 42's ThreeAxisFsw control law
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application. The goal is to be able to port
**      the 42 code code unchanged and have it run as a cFS app.
**   2. The targets and control modes are placeholders.
**
** References:
**   1. See 42 open source repository at https://sourceforge.net/projects/fortytwospacecraftsimulation/
**
** License:
**   This code is licensed under the NASA Open Source Agreement (NOSA).
*/

#ifndef _three_axis_fsw_
#define _three_axis_fsw_

/*
** Includes
*/

#include "app_cfg.h"


/*
** Control Modes
*/

#define TAF_CTRL_MODE_INIT   1
#define TAF_CTRL_MODE_TBD_1  2
#define TAF_CTRL_MODE_MAX    2

/*
** Event Message IDs
*/

#define THREEAXISFSW_DEBUG_EID  (THREEAXISFSW_BASE_EID + 0)


/*
** Type Definitions
*/


/*
** Exported Functions
*/

/******************************************************************************
** Function: InitFSW
**
** Initialize the 42 FSW controller
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void InitFSW(struct FSWType *FSW);


/******************************************************************************
** Function: ThreeAxisFSW
**
** Run the 42 simulator's control law
**
*/
void ThreeAxisFSW(struct FSWType *FSW);


#endif /* _three_axis_fsw_ */
