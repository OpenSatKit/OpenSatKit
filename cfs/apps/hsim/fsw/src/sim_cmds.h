/*
** $Id: $
**
** Title: CFS Simulator (SIM) Application Ground Commands Header File
**
** Purpose: Specification for the CFS SIM ground commands.
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

#ifndef _sim_cmds_h_
#define _sim_cmds_h_

#include "cfe.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM command handler function prototypes                         */
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
**  \sa #SIM_NOOP_CC, #SIM_Noop, #SIM_NoopCmd_t
**/
boolean SIM_NoopCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Reset Counters Command Handler Function
**
**  \par Description
**       This function resets the SIM housekeeping telemetry counters.
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
**  \sa #SIM_RESET_CC, #SIM_Reset, #SIM_ResetCmd_t
**/
boolean SIM_ResetCountersCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Send Data Packet Function
**
**  \par Description
**       This function sends a heater control data packet.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Temperature1 - The first temperature to be sent in the 
**        packet
**
**  \param [in]  Temperature2 - The second temperature to be sent in the 
**        packet
**
**  \param [in]  ErrorFlag - A flag to indicate if the data is stale or not 
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #SIM_SEND_DATA_PKT_CC, #SIM_SendDataPkt, #SIM_HeaterControlPkt_t
**/
void SIM_SendDataPktCmd(void);

boolean SIM_MisbehaveCmd(CFE_SB_MsgPtr_t MessagePtr);


#endif /* _sim_cmds_h_ */

/************************/
/*  End of File Comment */
/************************/

