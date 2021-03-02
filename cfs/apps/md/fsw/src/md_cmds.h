/*************************************************************************
** File: md_cmds.h
**
** NASA Docket No. GSC-18,450-1, identified as “Core Flight Software System (CFS)
** Memory Dwell Application Version 2.3.3” 
**
** Copyright © 2019 United States Government as represented by the Administrator of
** the National Aeronautics and Space Administration. All Rights Reserved. 
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
** http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License. 
*
** Purpose: 
**   Specification for the CFS Memory Dwell ground commands.
**
**
** Notes:
**
** 
*************************************************************************/
/*
** Ensure that header is included only once...
*/
#ifndef _md_cmds_h_
#define _md_cmds_h_

/*************************************************************************
** Includes
*************************************************************************/

/* md_msg needs to be included for MD_SymAddr_t definition */ 
#include "md_msg.h"

/*****************************************************************************/
/**
** \brief Process Memory Dwell Start Command
**
** \par Description
**          Extract command arguments, take appropriate actions,
**          issue event, and increment the command counter or 
**          error counter as appropriate.
** 
** \par Assumptions, External Events, and Notes:
**          Correct message length has been verified.
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**                                      
** \retval None
******************************************************************************/
void MD_ProcessStartCmd(CFE_SB_MsgPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Memory Dwell Stop Command
**
** \par Description
**          Extract command arguments, take appropriate actions,
**          issue event, and increment the command counter or 
**          error counter as appropriate.
** 
** \par Assumptions, External Events, and Notes:
**          Correct message length has been verified.
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**                                      
** \retval None
******************************************************************************/
void MD_ProcessStopCmd(CFE_SB_MsgPtr_t MessagePtr);


/*****************************************************************************/
/**
** \brief Process Memory Dwell Jam Command
**
** \par Description
**          Extract command arguments, take appropriate actions,
**          issue event, and increment the command counter or 
**          error counter as appropriate.
** 
** \par Assumptions, External Events, and Notes:
**          Correct message length has been verified.
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**                                      
** \retval None
******************************************************************************/
void MD_ProcessJamCmd(CFE_SB_MsgPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Set Signature Command
**
** \par Description
**          Extract command arguments, take appropriate actions,
**          issue event, and increment the command counter or 
**          error counter as appropriate.
** 
** \par Assumptions, External Events, and Notes:
**          Correct message length has been verified.
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**                                      
** \retval None
******************************************************************************/
void MD_ProcessSignatureCmd(CFE_SB_MsgPtr_t MessagePtr);


#endif /* _md_cmds_ */
/************************/
/*  End of File Comment */
/************************/
