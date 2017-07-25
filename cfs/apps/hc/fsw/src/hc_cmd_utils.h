/*
** $Id: $
**
** Title: Heater Control (HC) Application Command Utility Function
**        Header File
**
** Purpose: Specification for the CFS Heater Control command utility
**          functions.
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

#ifndef _hc_cmd_utils_h_
#define _hc_cmd_utils_h_

#include "cfe.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC command handler utility function prototypes                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Verify Command Packet Length Function
**
**  \par Description
**       This function is invoked from each of the command handlers to verify the
**       length of the command packet.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  CmdPacket - Pointer to a CFE Software Bus command packet
**  \param [in]  ExpectedLength - Expected packet length (command specific)
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates a valid packet length  \endcode
**  \endreturns
**
**  \sa #CFE_SB_GetTotalMsgLength
**/
boolean HC_IsValidCmdPktLength(CFE_SB_MsgPtr_t CmdPacket, uint16 ExpectedLength, uint32 EventID, char *CmdText);


#endif /* _hc_cmd_utils_h_ */

/************************/
/*  End of File Comment */
/************************/
