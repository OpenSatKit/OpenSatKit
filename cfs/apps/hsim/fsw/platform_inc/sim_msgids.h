/*
** $Id: $
**
** Title: Simulator (SIM) Message ID Header File
**
** Purpose: Specification for the CFS SIM application software bus
**          message identifiers
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

#ifndef _sim_msgids_h_
#define _sim_msgids_h_

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/

/* SIM command packet message ID's */
#define SIM_CMD_MID                   0x19B1      /** < \brief SIM ground commands */
#define SIM_SEND_HK_MID               0x19B2      /** < \brief SIM send housekeeping */
#define SIM_SEND_DATA_MID             0x19B3      /** < \brief Send SIM thermal data */

/* SIM telemetry packet message ID's */
#define SIM_HK_TLM_MID                0x09B1      /** < \brief SIM housekeeping */
#define SIM_HC_DATA_MID               0x19B4      /** < \brief SIM heater control data */

#endif /* _sim_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
