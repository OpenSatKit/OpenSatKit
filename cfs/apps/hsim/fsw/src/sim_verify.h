/*
** $Id: $
**
** Title: Simulator (SIM) Configuration Parameter Verification Header File
**
** Purpose:
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: $
*/

#ifndef _sim_verify_h_
#define _sim_verify_h_

#include "cfe.h"

#include "sim_platform_cfg.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM platform configuration parameters - application definitions */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Application name */
#ifndef SIM_APP_NAME
    #error SIM_APP_NAME must be defined!
#endif


/* Input command pipe */
#ifndef SIM_APP_PIPE_NAME
    #error SIM_APP_PIPE_NAME must be defined!
#endif


/* Number of packets that may queue in command pipe */
#ifndef SIM_APP_PIPE_DEPTH
    #error SIM_APP_PIPE_DEPTH must be defined!
#elif SIM_APP_PIPE_DEPTH < 4
    #error SIM_APP_PIPE_DEPTH cannot be less than 4
#elif SIM_APP_PIPE_DEPTH > 20
    #error SIM_APP_PIPE_DEPTH cannot be greater than 20
#endif


/* Mission specific portion of application revision number */
#ifndef SIM_MISSION_REV
    #error SIM_MISSION_REV must be defined!
#elif (SIM_MISSION_REV < 0)
    #error SIM_MISSION_REV must be greater than or equal to zero!
#endif

#endif /* _sim_verify_h_ */

/************************/
/*  End of File Comment */
/************************/
