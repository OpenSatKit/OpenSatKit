/*
** $Id: $
**
** Title: Heater Control (HC) Platform Configuration Header File
**
** Purpose: Specification for the CFS HC application constants
**          that can be configured form one platform to another
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

#ifndef _hc_platform_cfg_h_
#define _hc_platform_cfg_h_


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC platform configuration parameters - application definitions  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \hccfg Heater Control Application Name
**
**  \par Description:
**       This definition must match the name used at startup by the cFE
**       Executive Services when creating the HC application.  Note that
**       application names are also an argument to certain cFE commands.
**       For example, the application name is needed to access tables
**       via cFE Table Services commands.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Executive Services
**       for specific information on limits related to application names.
*/
#define HC_APP_NAME                     "HC"


/** \hccfg Heater Control Command Pipe Name
**
**  \par Description:
**       This definition is the name used at startup when creating a cFE
**       Software Bus command pipe for the HC application.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Software Bus Services
**       for specific information on limits related to pipe names.
*/
#define HC_APP_PIPE_NAME                "HC_CMD_PIPE"


/** \hccfg Heater Control Command Pipe Depth
**
**  \par Description:
**       This definition sets the total number of packets that may queue
**       in the HC command pipe.  The limit for individual message types
**       in the queue is the default cFE Software Bus subscription limit
**       of four.
**
**  \par Limits:
**       The HC application limits this value to be no less than 4 and
**       no greater than 20 packets at any one time in the command pipe.
*/
#define HC_APP_PIPE_DEPTH               10


/** \hccfg Mission specific version number for HC application
**
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "hc_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define HC_MISSION_REV                  1

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC platform config parameters - Heater Control table definitions*/
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \hccfg HC Type Table Name - cFE object name
**
**  \par Description:
**       Table object name is required during table creation.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Table Services
**       for specific information on limits related to table names.
*/
#define HC_TABLE_CFE_NAME               "Type"


/** \hccfg HC Type Table Name - cFE object name
**
**  \par Description:
**       Table name is required during table creation.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Table Services
**       for specific information on limits related to table names.
*/
#define HC_TABLE_NAME                  "HC.Type"


/** \hccfg HC Type Table Name - filename with path
**
**  \par Description:
**       Table name with path is required to load table at startup.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  If the named table does not
**       exist or fails validation, the table load will fail.
*/
#define HC_TABLE_DEF_NAME               "/cf/hc_type.tbl"


/** \hccfg HC Type Table Name - filename without path
**
**  \par Description:
**       Table name without path defines the output name for the table
**       file created during the table make process.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  If the table name is not
**       valid then the make process may fail, or the table file may
**       be unloadable to the target hardware.
*/
#define HC_TABLE_FILENAME               "hc_type.tbl"


/** \hccfg HC Type Table Description
**
**  \par Description:
**       Table files contain headers that include descriptive text.
**       This text will be put into the file header during the table
**       make process.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to cFE Table Services
**       for limits related to table descriptive text.
*/
#define HC_TABLE_DEF_DESC               "HC Type"


/** \hccfg Number of HC Type Table Entries
**
**  \par Description:
**       This value defines the number of entries in the HC Type
**       table. There is one entry for each heater control type, 
**       so the number of entries will be set to the number of
**       heater control types. 
**
**  \par Limits:
**       HC limits this value to be not less than 1 and not greater than 16.
*/

#define HC_TABLE_ENTRY_COUNT               HC_NUM_CTRL_TYPES


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC platform config parameters - Component table definitions     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \hccfg HC Component Table Name - cFE object name
**
**  \par Description:
**       Table object name is required during table creation.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Table Services
**       for specific information on limits related to table names.
*/
#define HC_COM_TABLE_CFE_NAME               "Component"


/** \hccfg HC Component Table Name - cFE object name
**
**  \par Description:
**       Table name is required during table creation.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Table Services
**       for specific information on limits related to table names.
*/
#define HC_COM_TABLE_NAME                   "HC.Component"


/** \hccfg HC Component Table Name - filename with path
**
**  \par Description:
**       Table name with path is required to load table at startup.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  If the named table does not
**       exist or fails validation, the table load will fail.
*/
#define HC_COM_TABLE_DEF_NAME               "/cf/hc_component.tbl"


/** \hccfg HC Component Table Name - filename without path
**
**  \par Description:
**       Table name without path defines the output name for the table
**       file created during the table make process.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  If the table name is not
**       valid then the make process may fail, or the table file may
**       be unloadable to the target hardware.
*/
#define HC_COM_TABLE_FILENAME               "hc_component.tbl"


/** \hccfg HC Component Table Description
**
**  \par Description:
**       Table files contain headers that include descriptive text.
**       This text will be put into the file header during the table
**       make process.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to cFE Table Services
**       for limits related to table descriptive text.
*/
#define HC_COM_TABLE_DEF_DESC               "HC Component Table"


/** \hccfg Number of Components Table Entries
**
**  \par Description:
**       This value defines the number of entries in the Component
**       table. There is one entry for each component, 
**       so the number of entries will be set to the number of
**       components. 
**
**  \par Limits:
**       HC limits this value to be not less than 1 and not greater than 16.
*/

#define HC_COM_TABLE_ENTRY_COUNT             HC_MAX_COMPONENTS


/** \hccfg HC Table Data Validation Error Code
**
**  \par Description:
**       Table data is verified during the table load process.  Should
**       the validation process fail, this value will be returned by
**       HC to cFE Table Services and displayed in an event message.
**
**  \par Limits:
**       HC requires that this value be defined, but otherwise places
**       no limits on the definition.  Refer to cFE Table Services
**       for limits related to error return values.
*/
#define HC_TABLE_VALIDATION_ERR             (0xCF000080L)



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC platform configuration parameters - heater definitions       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \hccfg Number of heater control types
**
**  \par Description:
**       The number of possible heater control types, such as battery
**       or propulsion.
**
**  \par Limits:
**       HC limits this value to be not less than 1. 
*/
#define HC_NUM_CTRL_TYPES                   2  


/** \hccfg Battery Heater Control Type
**
**  \par Description:
**       The heater control type for batteries.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.
*/
#define HC_BATTERY_TYPE                     0


/** \hccfg Propulsion Heater Control Type
**
**  \par Description:
**       The heater control type for propulsion.
**
**  \par Limits:
**       HC requires that this name be defined, but otherwise places
**       no limits on the definition.
*/
#define HC_PROPULSION_TYPE                  1


/** \hccfg Maximum number of battery components
**
**  \par Description:
**       The maximum number of components that are of the battery 
**       heater control type. 
**
**  \par Limits:
**       HC limits this value to be not less than 0. 
**
*/
#define HC_MAX_NUM_BATTERY                  1

 
/** \hccfg Maximum number of propulsion components
**
**  \par Description:
**       The maximum number of components that are of the propulsion
**       heater control type. 
**
**  \par Limits:
**       HC limits this value to be not less than 0. 
**
*/
#define HC_MAX_NUM_PROPULSION               1 


/** \hccfg Maximum number of heaters
**
**  \par Description:
**       The maximum number of heaters for one component. 
**
**  \par Limits:
**       HC limits this value to be not less than 1. 
**
*/
#define HC_MAX_NUM_HEATERS                  5


/** \hccfg Maximum number of thermistors
**
**  \par Description:
**       The maximum number of thermistors for one heater. 
**
**  \par Limits:
**       HC limits this value to be not less than 1. 
**
*/
#define HC_MAX_NUM_THERM                    3


/** \hccfg Maximum number of components
**
**  \par Description:
**       The maximum number of heater control components, 
**       found by adding the maximum number of battery components
**       to the maximum number of propulsion components. 
**
**  \par Limits:
**       HC limits this value to be not less than 1. 
**
*/
#define HC_MAX_COMPONENTS                  (HC_MAX_NUM_BATTERY + HC_MAX_NUM_PROPULSION)


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC enable flag definitions                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
** The definitions for the enable/disable flag in the heater control type.
** A heater control type can either be enabled or disabled. 
**/

#define HC_TYPE_DISABLED                    0
#define HC_TYPE_ENABLED                     1


#endif /* _hc_platform_cfg_h_ */

/************************/
/*  End of File Comment */
/************************/
