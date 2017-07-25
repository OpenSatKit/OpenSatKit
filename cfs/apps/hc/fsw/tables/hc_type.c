/*
** $Id: $
**
** Title: Heater Control (HC) Type Table Data
**
** Purpose: Default table contents for the Heater Control Types
**          Table.
**    
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $ Changed low and high temperature set points on HC_PROPULSION_TYPE
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
#include "hc_custom.h"
#include "hc_msg.h"
#include "hc_platform_cfg.h"
#include "hc_tbl.h"

/*
** HC type table header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "HC_TypeTable", HC_TABLE_NAME,
    HC_TABLE_DEF_DESC, HC_TABLE_FILENAME, sizeof(HC_TypeTable_t)
};

/*
** HC type table data
**
** -- Enabled or disabled flag
** -- Heater control type
** -- High temperature set point
** -- Low temperature set point
** -- Function index
**
*/

HC_TypeTable_t HC_TypeTable =
{
  {
    {                                   /* - 0 - */
        HC_TYPE_DISABLED,               /* Enabled flag */
        HC_BATTERY_TYPE,                /* Heater control type */
        50.0,                           /* High temperature set point */
        20.0,                           /* Low temperature set point */
        HC_BATTERY_FUNC,                /* Function index */
    },
    
    {                                   /* - 1 - */
        HC_TYPE_DISABLED,               /* Enabled flag */
        HC_PROPULSION_TYPE,             /* Heater control type */
        35.0,                           /* High temperature set point */
        5.0,                            /* Low temperature set point */
        HC_PROPULSION_FUNC,             /* Function index */
    },
  },
};

/************************/
/*  End of File Comment */
/************************/
