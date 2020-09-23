/*
** $Id: $
**
** Title: Heater Control (HC) Message ID Header File
**
** Purpose: Specification for the CFS HC application software bus
**          message identifiers
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: $ Changed name of wakeup MID to reflect its new frequency of 1/10 Hz
**	   cgesterling (18.Nov.2016)
*/

#ifndef _hc_msgids_h_
#define _hc_msgids_h_

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/

/* HC command packet message ID's */
#define HC_CMD_MID                   0x19A0      /** < \brief HC ground commands */
#define HC_SEND_HK_MID               0x19A1      /** < \brief HC send housekeeping */
#define HC_01HZ_WAKEUP_MID           0x19A2      /** < \brief Wakeup from scheduler every 10 seconds */
 
/* HC telemetry packet message ID's */
#define HC_HK_TLM_MID                0x09A0      /** < \brief HC housekeeping */
#define HC_THERM_TLM_MID             0x09A1      /** < \brief Thermal telemetry */

#endif /* _hc_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
