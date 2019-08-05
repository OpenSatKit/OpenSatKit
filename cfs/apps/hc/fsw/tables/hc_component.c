/*
** $Id: $
**
** Title: Heater Control (HC) Component Table Data
**
** Purpose: Default table contents for the Heater Control Components 
**          Table.
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $ Changed structure of ComponentTable Data to include array of
**	   thermistors per heater (number of entries in array should match
**	   number of heaters for that component)
**	   cgesterling (18.Nov.2016)
**
*/

/*************************************************************************
**
** Include section
**
**************************************************************************/
#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "hc_app.h"
#include "hc_msg.h"
#include "hc_platform_cfg.h"
#include "hc_tbl.h"

/*
** HC components table header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "HC_ComponentTable", HC_COM_TABLE_NAME,
    HC_COM_TABLE_DEF_DESC, HC_COM_TABLE_FILENAME, sizeof(HC_ComponentTable_t)
};

/*
** HC components table data
**
** -- Heater control type
** -- Number of heaters
** -- Array of thermistors per heater
**
*/

HC_ComponentTable_t HC_ComponentTable =
{
  {
    {                                   /* - 0 - */
        HC_BATTERY_TYPE,                /* Heater control type */
        3,                              /* Number of heaters */
        {2,1,2},                        /* Array of thermistors per heater */        
    },
    
    {                                   /* - 1 - */
        HC_PROPULSION_TYPE,             /* Heater control type */
        2,                              /* Number of heaters */
        {3,2},                          /* Array of thermistors per heater */
    },
  },
};

/************************/
/*  End of File Comment */
/************************/
