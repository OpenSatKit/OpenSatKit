/*
** $Id: $
**
** Title: Simulator (SIM) Platform Configuration Header File
**
** Purpose: Specification for the CFS SIM application constants
**          that can be configured form one platform to another
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: $Removed defined constant that is no longer used
**	  Changed SIM_NUM_COMPONENTS, SIM_MAX_NUM_HEATERS, and SIM_MAX_NUM_THERMS
**	  to be defined based on HC defined constants
**	  Included hc_platform_cfg.h to be able to read HC defined constants
**	  cgesterling (22.Nov.2016)
*/

#ifndef _sim_platform_cfg_h_
#define _sim_platform_cfg_h_

#include "hc_platform_cfg.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM platform configuration parameters - application definitions */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \simcfg Simulator Application Name
**
**  \par Description:
**       This definition must match the name used at startup by the cFE
**       Executive Services when creating the SIM application.  Note that
**       application names are also an argument to certain cFE commands.
**
**  \par Limits:
**       SIM requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Executive Services
**       for specific information on limits related to application names.
*/
#define SIM_APP_NAME                     "SIM"


/** \simcfg Simulator Command Pipe Name
**
**  \par Description:
**       This definition is the name used at startup when creating a cFE
**       Software Bus command pipe for the SIM application.
**
**  \par Limits:
**       SIM requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Software Bus Services
**       for specific information on limits related to pipe names.
*/
#define SIM_APP_PIPE_NAME                "SIM_CMD_PIPE"


/** \simcfg Simulator Command Pipe Depth
**
**  \par Description:
**       This definition sets the total number of packets that may queue
**       in the SIM command pipe.  The limit for individual message types
**       in the queue is the default cFE Software Bus subscription limit
**       of four.
**
**  \par Limits:
**       The SIM application limits this value to be no less than 4 and
**       no greater than 20 packets at any one time in the command pipe.
*/
#define SIM_APP_PIPE_DEPTH               10


/** \simcfg Mission specific version number for SIM application
**  
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "sim_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define SIM_MISSION_REV                   1


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM hardware configuration parameters - application definitions */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \simcfg Number of components
**  
**  \par Description:
**       The number of components in the hardware.
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define SIM_NUM_COMPONENTS               HC_MAX_COMPONENTS

/** \simcfg Maximum number of heaters
**  
**  \par Description:
**       The maximum number of heaters per component.
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define SIM_MAX_NUM_HEATERS              HC_MAX_NUM_HEATERS


/** \simcfg Maximum number of thermistors
**  
**  \par Description:
**       The maximum number of thermistors per heater.
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define SIM_MAX_NUM_THERMS               HC_MAX_NUM_THERM


#endif /* _sim_platform_cfg_h_ */

/************************/
/*  End of File Comment */
/************************/
