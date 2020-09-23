/*************************************************************************
** File:
**   $Id: lc_cmds.h 1.4 2017/01/22 17:24:53EST sstrege Exp  $
**
**  Copyright (c) 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   Specification for the CFS Limit Checker (LC) routines that
**   handle command processing
**
** Notes:
**
** 
**************************************************************************/
#ifndef _lc_cmds_
#define _lc_cmds_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Process a command pipe message
**  
**  \par Description
**       Processes a single software bus command pipe message. Checks
**       the message and command IDs and calls the appropriate routine
**       to handle the message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \returns
**  \retcode #CFE_SUCCESS   \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #LC_HousekeepingReq        \endcode
**  \endreturns
**
**  \sa #CFE_SB_RcvMsg
**
*************************************************************************/
int32 LC_AppPipe(CFE_SB_MsgPtr_t MessagePtr);
 
/************************************************************************/
/** \brief Reset HK counters
**  
**  \par Description
**       Utility function that resets housekeeping counters to zero
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \sa #LC_ResetCmd
**
*************************************************************************/
void LC_ResetCounters(void);

/************************************************************************/
/** \brief Reset AP results
**  
**  \par Description
**       Utility function that resets selected entries in actionpoint results table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   StartIndex   Start of area to reset (base zero)
**  \param [in]   EndIndex     End of area to reset (base zero)
**  \param [in]   ResetCmd     Reset AP stats command does not reset all fields
**       
**  \sa #LC_ResetAPStatsCmd
**
*************************************************************************/
void LC_ResetResultsAP(uint32 StartIndex, uint32 EndIndex, boolean ResetCmd);

/************************************************************************/
/** \brief Reset WP results
**  
**  \par Description
**       Utility function that resets selected entries in watchpoint results table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   StartIndex   Start of area to reset (base zero)
**  \param [in]   EndIndex     End of area to reset (base zero)
**  \param [in]   ResetCmd     Reset WP stats command does not reset all fields
**       
**  \sa #LC_ResetWPStatsCmd
**
*************************************************************************/
void LC_ResetResultsWP(uint32 StartIndex, uint32 EndIndex, boolean ResetCmd);

/************************************************************************/
/** \brief Write to Critical Data Store (CDS)
**  
**  \par Description
**       This function updates the CDS areas containing the watchpoint
**       results table, the actionpoint results table and the LC
**       application global data.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_ES_CopyToCDS  \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_UpdateTaskCDS(void);

/************************************************************************/
/** \brief Sample actionpoints request
**  
**  \par Description
**       Processes an on-board sample actionpoints request message.
**
**  \par Assumptions, External Events, and Notes:
**       This message does not affect the command execution counter
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
*************************************************************************/
void LC_SampleAPReq(CFE_SB_MsgPtr_t MessagePtr);
   
/************************************************************************/
/** \brief Housekeeping request
**  
**  \par Description
**       Processes an on-board housekeeping request message.
**
**  \par Assumptions, External Events, and Notes:
**       This message does not affect the command execution counter
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #LC_AcquirePointers     \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_HousekeepingReq(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Noop command
**  
**  \par Description
**       Processes a noop ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_NOOP_CC
**
*************************************************************************/
void LC_NoopCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset counters command
**  
**  \par Description
**       Processes a reset counters ground command which will reset
**       the following LC application counters to zero:
**         - Command counter
**         - Command error counter
**         - Actionpoint sample counter
**         - Monitored message counter 
**         - RTS execution counter
**         - Passive RTS execution counter
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_RESET_CC
**
*************************************************************************/
void LC_ResetCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Set LC state command
**  
**  \par Description
**       Processes a set LC application state ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_SET_LC_STATE_CC
**
*************************************************************************/
void LC_SetLCStateCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Set AP state command
**  
**  \par Description
**       Processes a set actionpoint state ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_SET_AP_STATE_CC
**
*************************************************************************/
void LC_SetAPStateCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Set AP permanently off command
**  
**  \par Description
**       Processes a set actionpoint permanently off ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_SET_AP_PERMOFF_CC
**
*************************************************************************/
void LC_SetAPPermOffCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset AP statistics command
**  
**  \par Description
**       Processes a reset actionpoint statistics ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_RESET_AP_STATS_CC
**
*************************************************************************/
void LC_ResetAPStatsCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset WP statistics command
**  
**  \par Description
**       Processes a reset watchpoint statistics ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_RESET_WP_STATS_CC
**
*************************************************************************/
void LC_ResetWPStatsCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Verify message length
**  
**  \par Description
**       Checks if the actual length of a software bus message matches 
**       the expected length and sends an error event if a mismatch
**       occures
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg              A #CFE_SB_MsgPtr_t pointer that
**                                 references the software bus message 
**
**  \param [in]   ExpectedLength   The expected length of the message
**                                 based upon the command code
**
**  \returns
**  \retstmt Returns TRUE if the length is as expected      \endcode
**  \retstmt Returns FALSE if the length is not as expected \endcode
**  \endreturns
**
**  \sa #LC_LEN_ERR_EID
**
*************************************************************************/
boolean LC_VerifyMsgLength(CFE_SB_MsgPtr_t msg, 
                           uint16          ExpectedLength);

int32 LC_ManageTables(void);

#endif /* _lc_cmds_ */

/************************/
/*  End of File Comment */
/************************/
