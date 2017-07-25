/*
** $Id: $
**
** Title: CFS Heater Control (HC) Application Ground Commands Header File
**
** Purpose: Specification for the CFS HC ground commands.
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

#ifndef _hc_cmds_h_
#define _hc_cmds_h_

#include "cfe.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC command handler function prototypes                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief No-operation Command Handler Function
**
**  \par Description
**       This function generates an event that displays the application version
**       numbers.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #HC_NOOP_CC, #HC_Noop, #HC_NoopCmd_t
**/
boolean HC_NoopCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Reset Counters Command Handler Function
**
**  \par Description
**       This function resets the HC housekeeping telemetry counters.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #HC_RESET_CC, #HC_Reset, #HC_ResetCmd_t
**/
boolean HC_ResetCountersCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Set Status Command Handler Function
**
**  \par Description
**       This function sets the status of a heater control type.
**       It will verify that the given heater control type and the 
**       given status are valid before setting the status. 
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #HC_SET_STATUS_CC, #HC_SetStatus, #HC_SetStatusCmd_t
**/
boolean HC_SetStatusCmd(CFE_SB_MsgPtr_t MessagePtr);

#endif /* _hc_cmds_h_ */

/************************/
/*  End of File Comment */
/************************/

