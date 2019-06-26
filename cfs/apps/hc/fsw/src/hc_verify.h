/*
** $Id: $
**
** Title: Heater Control (HC) Configuration Parameter Verification Header File
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

#ifndef _hc_verify_h_
#define _hc_verify_h_

#include "cfe.h"
#include "hc_platform_cfg.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC platform configuration parameters - application definitions  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Application name */
#ifndef HC_APP_NAME
    #error HC_APP_NAME must be defined!
#endif


/* Input command pipe */
#ifndef HC_APP_PIPE_NAME
    #error HC_APP_PIPE_NAME must be defined!
#endif


/* Number of packets that may queue in command pipe */
#ifndef HC_APP_PIPE_DEPTH
    #error HC_APP_PIPE_DEPTH must be defined!
#elif HC_APP_PIPE_DEPTH < 4
    #error HC_APP_PIPE_DEPTH cannot be less than 4
#elif HC_APP_PIPE_DEPTH > 20
    #error HC_APP_PIPE_DEPTH cannot be greater than 20
#endif


/* Mission specific portion of application revision number */
#ifndef HC_MISSION_REV
    #error HC_MISSION_REV must be defined!
#elif (HC_MISSION_REV < 0)
    #error HC_MISSION_REV must be greater than or equal to zero!
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC platform config parameters - HC Type table definitions       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* cFE object name for HC type table - not filename */
#ifndef HC_TABLE_CFE_NAME
    #error HC_TABLE_CFE_NAME must be defined!
#endif


/* HC type table name */
#ifndef HC_TABLE_NAME
    #error HC_TABLE_NAME must be defined!
#endif


/* HC type table filename - with path */
#ifndef HC_TABLE_DEF_NAME
    #error HC_TABLE_DEF_NAME must be defined!
#endif


/* HC type table filename - without path */
#ifndef HC_TABLE_FILENAME
    #error HC_TABLE_FILENAME must be defined!
#endif


/* Default description text for HC type table */
#ifndef HC_TABLE_DEF_DESC
    #error HC_TABLE_DEF_DESC must be defined!
#endif


/* Number of HC type table entries */
#ifndef HC_TABLE_ENTRY_COUNT
    #error HC_TABLE_ENTRY_COUNT must be defined!
#elif HC_TABLE_ENTRY_COUNT < 1
    #error HC_TABLE_ENTRY_COUNT cannot be less than 1
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC platform config parameters - HC Component table definitions  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* cFE object name for component table - not filename */
#ifndef HC_COM_TABLE_CFE_NAME
    #error HC_COM_TABLE_CFE_NAME must be defined!
#endif


/* Component table name */
#ifndef HC_COM_TABLE_NAME
    #error HC_COM_TABLE_NAME must be defined!
#endif


/* Component table filename - with path */
#ifndef HC_COM_TABLE_DEF_NAME
    #error HC_COM_TABLE_DEF_NAME must be defined!
#endif


/* Component table filename - without path */
#ifndef HC_COM_TABLE_FILENAME
    #error HC_COM_TABLE_FILENAME must be defined!
#endif


/* Default description text for component table */
#ifndef HC_COM_TABLE_DEF_DESC
    #error HC_COM_TABLE_DEF_DESC must be defined!
#endif


/* Number of component table entries */
#ifndef HC_COM_TABLE_ENTRY_COUNT
    #error HC_COM_TABLE_ENTRY_COUNT must be defined!
#elif HC_COM_TABLE_ENTRY_COUNT < 1
    #error HC_COM_TABLE_ENTRY_COUNT cannot be less than 1
#endif

/* Table validation error code */
#ifndef HC_TABLE_VALIDATION_ERR
    #error HC_TABLE_VALIDATION_ERR must be defined!
#endif


#endif /* _hc_verify_h_ */

/************************/
/*  End of File Comment */
/************************/
