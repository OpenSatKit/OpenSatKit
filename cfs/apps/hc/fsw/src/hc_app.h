/*
** $Id: $
**
** Title: CFS Heater Control (HC) Application Header File
**
** Purpose: Unit specification for the CFS Heater Control Application.
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

#ifndef _hc_app_h_
#define _hc_app_h_

#include "cfe.h"
#include "hc_msg.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC application global function prototypes                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Application entry point and main process loop
**
**  \par Description
**
**       Register HC as a CFE application.
**       Invoke HC application initialization function.
**       Enter HC main process loop.
**         Pend (forever) on next Software Bus command packet.
**         Process received Software Bus command packet.
**         Repeat main process loop.
**       Allow CFE to terminate the HC application.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \sa #HC_AppInit, #CFE_ES_RunLoop, #CFE_SB_RcvMsg, #HC_ProcessPkt
**/
void HC_AppMain(void);


/**
**  \brief HC Application Initialization Function
**
**  \par Description
**
**       Initialize HC global data structure.
**       Register HC application for CFE Event Services.
**       Create Software Bus input pipe.
**       Subscribe to HC housekeeping request command packet.
**       Subscribe to HC ground command packet.
**       Invoke HC table initialization function.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS
**  \endcode
**  \retstmt Return codes from #CFE_EVS_Register, #CFE_SB_CreatePipe, #CFE_SB_Subscribe
**  \endcode
**  \endreturns
**
**  \sa #CFE_EVS_Register, #CFE_SB_CreatePipe, #CFE_SB_Subscribe
**/
int32 HC_AppInit(void);


/**
**  \brief Process Input Command Packets
**
**  \par Description
**
**       Branch to appropriate input packet handler: HK request or HC commands.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #HC_ReportHK, #HC_ProcessCmd 
**/
void HC_ProcessPkt(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Process HC Ground Commands
**
**  \par Description
**
**       Branch to the command specific handlers for HC ground commands.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #HC_Noop, #HC_Reset, #HC_SetStatus
**/
void HC_ProcessCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Housekeeping Request Command Handler
**
**  \par Description
**
**       Populate the HC application Housekeeping Telemetry packet.  Timestamp
**       the packet and send it to ground via the Software Bus.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #HC_HousekeepingCmd_t, #HC_HousekeepingPkt_t
**/
void HC_ReportHK(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Receive Data Packet Command Handler
**
**  \par Description
**
**       Handles receiving a data packet from SIM.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #HC_HeaterControlPkt_t
**/
void HC_ReceivePkt(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Perform Heater Control Command Handler
**
**  \par Description
**
**       Performs heater control upon receipt of a scheduler wakeup.
**       Check for heater control types with an enabled status and call
**       the mission defined custom function for those types. 
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #HC_PerformHC_t
**/
void HC_PerformHC(CFE_SB_MsgPtr_t MessagePtr);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC -- application global data structure                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \hctlm Application global data structure
**/
typedef struct
{
    HC_ComponentTable_t *ComponentTablePtr; /**< \brief Components Table Pointer */
    CFE_TBL_Handle_t ComponentTableHandle;  /**< \brief Components Table Handle */

    HC_TypeTable_t *TypeTablePtr;           /**< \brief Type Table Pointer */
    CFE_TBL_Handle_t TypeTableHandle;       /**< \brief Type Table Handle */

    CFE_SB_PipeId_t  CmdPipe;               /**< \brief cFE software bus command pipe */

    HC_HousekeepingPkt_t HousekeepingPkt;   /**< \brief Application housekeeping telemetry packet */

    HC_HeaterControlPkt_t ThermalDataPkt;   /**< \brief Thermal data packet from hardware */

} HC_GlobalData_t;

#endif /* _hc_app_h_ */

/************************/
/*  End of File Comment */
/************************/
