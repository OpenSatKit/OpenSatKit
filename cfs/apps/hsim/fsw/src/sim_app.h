/*
** $Id: $
**
** Title: CFS Simulator (SIM) Application Header File
**
** Purpose: Unit specification for the CFS Simulator Application.
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

#ifndef _sim_app_h_
#define _sim_app_h_

#include "cfe.h"
#include "sim_msg.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application global function prototypes                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Application entry point and main process loop
**
**  \par Description
**
**       Register SIM as a CFE application.
**       Invoke SIM application initialization function.
**       Enter SIM main process loop.
**         Pend (forever) on next Software Bus command packet.
**         Process received Software Bus command packet.
**         Repeat main process loop.
**       Allow CFE to terminate the SIM application.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \sa #SIM_AppInit, #CFE_ES_RunLoop, #CFE_SB_RcvMsg, #SIM_ProcessPkt
**/
void SIM_AppMain(void);


/**
**  \brief SIM Application Initialization Function
**
**  \par Description
**
**       Initialize SIM global data structure.
**       Register SIM application for CFE Event Services.
**       Create Software Bus input pipe.
**       Subscribe to SIM housekeeping request command packets.
**       Subscribe to SIM ground command packets.
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
int32 SIM_AppInit(void);


/**
**  \brief Process Input Command Packets
**
**  \par Description
**
**       Branch to appropriate input packet handler: HK request or SIM commands.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #SIM_ReportHK, #SIM_ProcessCmd
**/
void SIM_ProcessPkt(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Process SIM Ground Commands
**
**  \par Description
**
**       Branch to the command specific handlers for SIM ground commands.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #SIM_Noop, #SIM_Reset
**/
void SIM_ProcessCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Housekeeping Request Command Handler
**
**  \par Description
**
**       Populate the SIM application Housekeeping Telemetry packet.  Timestamp
**       the packet and send it to ground via the Software Bus.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #SIM_HousekeepingCmd_t, #SIM_HousekeepingPkt_t
**/
void SIM_ReportHK(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Receive HC Command Packets
**
**  \par Description
**
**       Handle commands from HC. It will perform the command
**       specified heater control (turn a command specified heater
**       either on or off). It also will update the value of the 
**       integers used to represent heaters for the ground system
**       GUI. 
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \sa #SIM_ReceiveCmd
**/
boolean SIM_ReceiveCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Send Telemetry Data
**
**  \par Description
**
**       This function is designed to be called once every second
**       by the scheduler to send one thermal data packets. The thermal
**       packets are in the TlmData[] array in the global data. When this 
**       function is called, it will send one packet from that array. It 
**       will increment the global variable i, so that the next time the 
**       function is called, it will send the next packet in the array. 
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in]
**
**  \sa #SIM_SendTlmData
**/
void SIM_SendTlmData(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Set Telemetry Data
**
**  \par Description
**
**       This function is a place for the telemetry data to 
**       be hard coded in. Each entry in the telemetry data array
**       has four uint8 temperatures and a boolean flag that are hard
**       coded in this function.
**
**  \par Assumptions, External Events, and Notes: None
**
**  \param [in] 
**
**  \sa #SIM_SetData
**/
void SIM_SetData(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM -- application global data structure                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \simtlm Application global data structure
**/
typedef struct
{
    CFE_SB_PipeId_t  CmdPipe;                /**< \brief cFE software bus command pipe */

    SIM_HousekeepingPkt_t HousekeepingPkt;   /**< \brief Application housekeeping telemetry packet */

    SIM_HeaterControlPkt_t TempData;	     /**< \brief Thermistor temperature data */

    uint8  Counter;			     /**< \brief Counter for incrementing temp data */

    boolean Misbehave;

} SIM_GlobalData_t;

#endif /* _sim_app_h_ */

/************************/
/*  End of File Comment */
/************************/
